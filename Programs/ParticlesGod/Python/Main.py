import subprocess
import struct
import psutil
import shutil
import ctypes
import yaml
import json
import time
import os

import SceneFile
import SceneFilePolygonGroups

LIBRARY_PARAMS_FILENAME = "ParticlesGodLibrary.json"
LIBRARY_FILENAME = "ParticlesGodLibrary.dll"

INJECTOR_PATHNAME = os.path.join(os.path.dirname(__file__), "ParticlesGodInjector.exe")

WOTBLITZ_PROCESS_NAME = "wotblitz.exe"
WOTBLITZ_EXECUTABLE_VERSION = "11.17.0.744"

SHARED_FILE = '../Shared.yaml'

DEBUG_LIBRARY = False

def InjectDLL(procId, dllPath) -> int:
    """
    Inject a dynamic-link library (DLL) into a running process.
    """
    result = subprocess.run(
        [INJECTOR_PATHNAME, str(procId), dllPath],
        capture_output=True, text=True
    )
    print(result.stdout)
    if result.returncode != 0:
        raise RuntimeError(f"Injection failed: {result.stderr or result.stdout}")
    
    return result.returncode

def IsLoadingDLL(procId, dllName):
    """
    Check whether a specific DLL is currently processing in a target process.
    """
    process = psutil.Process(procId)
    try:
        for module in process.memory_maps():
            if dllName.lower() in module.path.lower():
                return True
        return False
    except Exception:
        return False
    
def WaitLoadingDLL(procId, dllName, poll_interval=1):
    """
    Block execution while a specific DLL remains loaded in a target process.
    """
    while IsLoadingDLL(procId, dllName):
        time.sleep(poll_interval)

def GetExecutableVersion(filepath):
    """
    Returns the version of a Windows executable as a string 'major.minor.build.revision'.
    Parameters:
        filepath (str): Full path to the .exe file.

    Returns:
        str: Version string like '1.2.3.4', or None if version info not found.
    """
    
    size = ctypes.windll.version.GetFileVersionInfoSizeW(filepath, None)
    if not size:
        return None

    res = ctypes.create_string_buffer(size)
    if not ctypes.windll.version.GetFileVersionInfoW(filepath, 0, size, res):
        return None

    lptr = ctypes.c_void_p()
    uLen = ctypes.c_uint()
    if not ctypes.windll.version.VerQueryValueW(res, "\\", ctypes.byref(lptr), ctypes.byref(uLen)):
        return None

    class VS_FIXEDFILEINFO(ctypes.Structure):
        _fields_ = [
            ("dwSignature", ctypes.c_uint32),
            ("dwStrucVersion", ctypes.c_uint32),
            ("dwFileVersionMS", ctypes.c_uint32),
            ("dwFileVersionLS", ctypes.c_uint32),
            ("dwProductVersionMS", ctypes.c_uint32),
            ("dwProductVersionLS", ctypes.c_uint32),
            ("dwFileFlagsMask", ctypes.c_uint32),
            ("dwFileFlags", ctypes.c_uint32),
            ("dwFileOS", ctypes.c_uint32),
            ("dwFileType", ctypes.c_uint32),
            ("dwFileSubtype", ctypes.c_uint32),
            ("dwFileDateMS", ctypes.c_uint32),
            ("dwFileDateLS", ctypes.c_uint32),
        ]

    version_info = ctypes.cast(lptr, ctypes.POINTER(VS_FIXEDFILEINFO)).contents
    version = f"{version_info.dwFileVersionMS >> 16}.{version_info.dwFileVersionMS & 0xFFFF}." \
              f"{version_info.dwFileVersionLS >> 16}.{version_info.dwFileVersionLS & 0xFFFF}"
    return version

def GetMaxDataNodesId(inputFileName, descriptor, sceneArchive):
    """
    Determine the maximum data node ID on the scene

    If descriptor.geometryIdHash != NULL then scene polygon group file exists
    In this scenario, find maximum polygon group ID in `.scg` file

    We go through each data nodes and find highest ID found
    """
    import VariantType

    maxDataNodesId = 0

    if descriptor["descriptorSize"] >= 12 and descriptor["geometryIdHash"] != 0:
        polygonsFilePath = inputFileName.replace(".sc2", ".scg")
        if not os.path.isfile(polygonsFilePath):
            return False

        with open(polygonsFilePath, 'rb') as polygonsFile:
            maxDataNodesId = SceneFilePolygonGroups.GetMaxPolygonGroupsIds(polygonsFile)

    if maxDataNodesId == False:
        return False
    
    dataNodes = sceneArchive["#dataNodes"][VariantType.TYPENAME_VARIANT_VECTOR]
    for dataNode in dataNodes:
        nodeArch = dataNode[VariantType.TYPENAME_KEYED_ARCHIVE]
        nodeId = struct.unpack('<Q', nodeArch['#id'][VariantType.TYPENAME_BYTE_ARRAY])[0]
        if nodeId > maxDataNodesId:
            maxDataNodesId = nodeId

    return maxDataNodesId

def ReplaceSpritePaths(dataNode):
    """
    Recursively update sprite paths inside a particle emitter data node.

    This function traverses a nested structure (dicts and lists) and searches
    for keys named "spritePath". When found, it converts absolute paths that
    contain "DataSource" or "Data" into engine resources relative paths.
    """
    import VariantType

    if isinstance(dataNode, dict):
        for key, value in dataNode.items():
            if key == "spritePath" and isinstance(value, dict) and VariantType.TYPENAME_STRING in value:
                path = value[VariantType.TYPENAME_STRING]
                if "DataSource" in path:
                    new_path = path.split("DataSource", 1)[-1]
                    new_path = new_path.lstrip("/\\") 
                    value[VariantType.TYPENAME_STRING] = "~res:/" + new_path.replace("\\", "/")
                elif "Data" in path:
                    new_path = path.split("Data", 1)[-1]
                    new_path = new_path.lstrip("/\\") 
                    value[VariantType.TYPENAME_STRING] = "~res:/" + new_path.replace("\\", "/")
            else:
                ReplaceSpritePaths(value)
    elif isinstance(dataNode, list):
        for item in dataNode:
            ReplaceSpritePaths(item)

def ResolveEmitterYamlPath(emitter_path, inputFileName):
    """
    Attempt to resolve a valid path to an emitter's YAML file
    from a path (relative or absolute) and the source .sc2 file.
    If not found, try resolving '~res:' paths relative to the DataSource folder.
    """
    candidates = []

    # 1. Absolute path as-is
    if os.path.isabs(emitter_path):
        candidates.append(emitter_path)

    # 2. Relative to the .sc2 file directory
    sc2_dir = os.path.dirname(os.path.abspath(inputFileName))
    candidates.append(os.path.join(sc2_dir, emitter_path))

    # 3. Absolute normalized path
    candidates.append(os.path.abspath(emitter_path))

    # 4. Try all candidates
    for path in candidates:
        normalized = os.path.normpath(path)
        if os.path.isfile(normalized):
            return normalized

    # 5. If path starts with '~res:', try resolving relative to DataSource folder
    if emitter_path.startswith('~res:'):
        # Remove the '~res:' prefix
        rel_path = emitter_path[len('~res:'):]
        
        # Find the DataSource folder in the .sc2 path
        sc2_dir_parts = os.path.abspath(inputFileName).split(os.sep)
        if 'DataSource' in sc2_dir_parts:
            ds_index = sc2_dir_parts.index('DataSource')
            data_source_path = os.sep.join(sc2_dir_parts[:ds_index + 1])
            candidate = os.path.normpath(os.path.join(data_source_path, rel_path.lstrip('/\\')))
            if os.path.isfile(candidate):
                return candidate

    # If nothing found, return None
    return None

def FindEmitterFileNames(archive):
    """
    Recursively collect all ParticleEffectComponent's emitter.filename's from the scene archive.
    It also recursively searches through any child archives in the `#hierarchy` field.
    """
    import VariantType

    emitterFileNames = []

    if "components" in archive:
        componentsArch = archive["components"][VariantType.TYPENAME_KEYED_ARCHIVE]
        componentCount = componentsArch["count"][VariantType.TYPENAME_UINT32]

        for componentIndex in range(componentCount):
            componentIndexFormat = str(componentIndex).zfill(4)
            componentArch = componentsArch[componentIndexFormat][VariantType.TYPENAME_KEYED_ARCHIVE]
            componentType = componentArch["comp.typename"][VariantType.TYPENAME_STRING]

            if componentType != "ParticleEffectComponent":
                continue

            nestedEmitters = False
            if "pe.nestedEmitters" in componentArch:
                nestedEmitters = componentArch["pe.nestedEmitters"][VariantType.TYPENAME_BOOLEAN]

            if nestedEmitters:
                continue

            emittersCount = componentArch["pe.emittersCount"][VariantType.TYPENAME_UINT32]
            emitters = componentArch["pe.emitters"][VariantType.TYPENAME_KEYED_ARCHIVE]

            for emitterIndex in range(emittersCount):
                emitterIndexFormat = str(emitterIndex).zfill(4)
                emitterArch = emitters[emitterIndexFormat][VariantType.TYPENAME_KEYED_ARCHIVE]

                if "emitter.filename" not in emitterArch:
                    raise KeyError(
                        f"Missing 'emitter.filename' key in ParticleEffectComponent index {componentIndex}"
                    )

                emitterFileName = emitterArch["emitter.filename"][VariantType.TYPENAME_STRING]
                emitterFileNames.append(emitterFileName)

    # Recurse into children
    if "#hierarchy" in archive:
        hierarchy = archive["#hierarchy"][VariantType.TYPENAME_VARIANT_VECTOR]
        for child in hierarchy:
            childArchive = child[VariantType.TYPENAME_KEYED_ARCHIVE]
            emitterFileNames.extend(FindEmitterFileNames(childArchive))

    return emitterFileNames

def UpdateNestedEmitters(archive, maxDataNodeId):
    """
    Update ParticleEffectComponent's in the archive to nested emitters.
    Assigns each emitter a new unique ID based on `maxDataNodeId`, incrementing it as emitters are updated.
    Recursively processes all child archives in the `#hierarchy` vector.
    """
    import VariantType

    if "components" in archive:
        componentsArch = archive["components"][VariantType.TYPENAME_KEYED_ARCHIVE]
        componentCount = componentsArch["count"][VariantType.TYPENAME_UINT32]

        for componentIndex in range(componentCount):
            componentIndexFormat = str(componentIndex).zfill(4)
            componentArch = componentsArch[componentIndexFormat][VariantType.TYPENAME_KEYED_ARCHIVE]
            componentType = componentArch["comp.typename"][VariantType.TYPENAME_STRING]

            if componentType != "ParticleEffectComponent":
                continue

            nestedEmitters = componentArch.get("pe.nestedEmitters", {}).get(VariantType.TYPENAME_BOOLEAN, False)

            if "pe.emitters" not in componentArch:
                continue

            emitters = componentArch["pe.emitters"][VariantType.TYPENAME_KEYED_ARCHIVE]
            emittersCount = len(emitters)

            updateNeeded = False
            for emitterIndex in range(emittersCount):
                emitterIndexFormat = str(emitterIndex).zfill(4)
                emitterArch = emitters[emitterIndexFormat][VariantType.TYPENAME_KEYED_ARCHIVE]
                if "emitter.filename" in emitterArch:
                    updateNeeded = True
                    break

            if nestedEmitters or not updateNeeded:
                continue

            componentArch["pe.nestedEmitters"] = {VariantType.TYPENAME_BOOLEAN: True}
            componentArch["pe.updatedFromGame"] = {VariantType.TYPENAME_BOOLEAN: True}

            newEmittersVector = []

            for emitterIndex in range(emittersCount):
                emitterIndexFormat = str(emitterIndex).zfill(4)
                emitterArch = emitters[emitterIndexFormat][VariantType.TYPENAME_KEYED_ARCHIVE]

                maxDataNodeId += 1

                keyedArchive = {
                    '##name': {
                        VariantType.TYPENAME_STRING: "<Unknown class>"
                    },
                    'emitter.data': {
                        VariantType.TYPENAME_VARIANT_VECTOR: [
                            {
                                VariantType.TYPENAME_KEYED_ARCHIVE: {
                                    "emitter.id": {VariantType.TYPENAME_UINT64: maxDataNodeId}
                                }
                            }
                        ]
                    },
                    'emitter.position': {
                        VariantType.TYPENAME_VECTOR3: {"x": 0.0, "y": 0.0, "z": 0.0}
                    },
                    'emitter.rotation': {
                        VariantType.TYPENAME_VECTOR4: {"x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0}
                    },
                    'emitter.scale': {
                        VariantType.TYPENAME_VECTOR3: {"x": 1.0, "y": 1.0, "z": 1.0}
                    },
                    # Keep emitter filename for ResourceEditor rendering (pe.updatedFromGame)
                    'emitter.filename': {
                        VariantType.TYPENAME_STRING: emitterArch["emitter.filename"][VariantType.TYPENAME_STRING]
                    },
                }

                newEmittersVector.append({
                    VariantType.TYPENAME_KEYED_ARCHIVE: keyedArchive
                })

            componentArch["pe.emitters"] = {
                VariantType.TYPENAME_VARIANT_VECTOR: newEmittersVector
            }

    # Recursive update
    if "#hierarchy" in archive:
        hierarchy = archive["#hierarchy"][VariantType.TYPENAME_VARIANT_VECTOR]
        for child in hierarchy:
            childArchive = child[VariantType.TYPENAME_KEYED_ARCHIVE]
            maxDataNodeId = UpdateNestedEmitters(
                childArchive,
                maxDataNodeId,
            )

    return maxDataNodeId

def Main():
    import VariantType
    import KeyedArchive

    print("[ParticlesGod] Script started!")

    editor_params = {}
    with open(SHARED_FILE, 'r') as file:
        editor_params = yaml.safe_load(file)
    
    if len(editor_params) == 0:
        print("[ParticlesGod] failed to load Shared.yaml")
        return
    
    inputFileName = editor_params["keyedArchive"]["lastLibrarySelectedFileSc2"]['string']
    if inputFileName == "":
        print("[ParticlesGod] library selected path sc2 is empty")
        return
    
    print(f"[ParticlesGod] Successfully found input file at: {inputFileName}")
    
    if not os.path.isfile(inputFileName):
        print("[ParticlesGod] library selected path sc2 has not been found")
        return
    
    if not os.path.isfile(LIBRARY_FILENAME):
        print(f"[ParticlesGod] Library {LIBRARY_FILENAME} is missing")
        return False
    
    print(f"[ParticlesGod] Successfully found library: {LIBRARY_FILENAME}")
    
    processId, executable = None, ""
    for proc in psutil.process_iter(['pid', 'name', 'exe']):
        try:
            if proc.info['name'].lower() == WOTBLITZ_PROCESS_NAME.lower():
                processId, executable = proc.info['pid'], proc.info['exe']
        except:
            print(f"[ParticlesGod] Error while getting {WOTBLITZ_PROCESS_NAME} pid and executable path")
            return False
        
    if processId == None:
        print(f"[ParticlesGod] Game process {WOTBLITZ_PROCESS_NAME} is not running!")
        return False
        
    print(f"[ParticlesGod] Game is currently running on PID: {processId}")

    version = GetExecutableVersion(executable)
    if version != WOTBLITZ_EXECUTABLE_VERSION:
        print(f"[ParticlesGod] Error {version} is not compatible with {WOTBLITZ_EXECUTABLE_VERSION}")
        return False
    
    print(f"[ParticlesGod] Version of the game matches: {version}")

    if processId == None:
        print(f"[ParticlesGod] Error {WOTBLITZ_PROCESS_NAME} is not running")
        return False

    directoryExecutable = os.path.dirname(executable)
    
    with open(inputFileName, 'rb') as inputFile:
        sceneFile = SceneFile.Deserialize(inputFile)
        if sceneFile == False:
            print("[ParticlesGod] Failed to load scene file")
            return False
        
        print(f"[ParticlesGod] Successfully loaded scene file content")
        
        if sceneFile["header"]["version"] < SceneFile.MINIMAL_SUPPORTED_VERSION:
            print(f"[ParticlesGod] Selected scene file must have a version greater than {SceneFile.MINIMAL_SUPPORTED_VERSION}")
            return False
        
        descriptor, sceneArchive = sceneFile["descriptor"], sceneFile["sceneArchive"]

        maxDataNodesId = GetMaxDataNodesId(inputFileName, descriptor, sceneArchive)
        if maxDataNodesId == False:
            print("[ParticlesGod] Failed to get max datanodes ids")
            return False
        
        print(f"[ParticlesGod] Max data nodes id is {maxDataNodesId}")

        emitterFilePaths = []
        try:
            hierarchy = sceneArchive["#hierarchy"][VariantType.TYPENAME_VARIANT_VECTOR]
            for child in hierarchy:
                archive = child[VariantType.TYPENAME_KEYED_ARCHIVE]
                emitterFilePaths.extend(FindEmitterFileNames(archive))
        except Exception as e:
            print(f"[ParticlesGod] Error while reading particle effects yaml paths: {e}")
            return False

        if len(emitterFilePaths) == 0:
            print(f"[ParticlesGod] No emitter yaml paths found, ending script")
            return False
        
        # resolvedEmitterPathsMap could be usefull but finally unused
        resolvedEmitterPathsMap = {}

        for index in range(len(emitterFilePaths)):
            emitterFilePath = emitterFilePaths[index]
            resolveFilePath = ResolveEmitterYamlPath(emitterFilePath, inputFileName)
            if not resolveFilePath:
                print(f"[ParticlesGod] Emitter YAML file not found: {emitterFilePath}")
                return False

            emitterFilePaths[index] = resolveFilePath
            resolvedEmitterPathsMap[emitterFilePath] = resolveFilePath
            
        print("[ParticlesGod] All emitter files found will be written")
        for emitterFilePath in emitterFilePaths:
            print(emitterFilePath)
        
        params = {
            "emitterFilePaths": emitterFilePaths,
            "functions": {
                "ParticleEmitter__LoadEmitterFromFile": "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 51 56 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 6A 64 E8 ? ? ? ? 83 C4 04 89 45 F0 C7 45 ? ? ? ? ? 85 C0 74 0B 8B C8 E8 ? ? ? ? 8B F0 EB 02 33 F6 C7 45 ? ? ? ? ? FF 75 0C FF 75 08 56 E8 ? ? ? ? 83 C4 0C 8B C6 8B 4D F4 64 89 0D ? ? ? ? 59 5E 8B E5 5D C3",
                "ParticleSerializer__SaveEmitter": "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 2C A1 ? ? ? ? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B 5D 0C 8B 75 08 53 56 E8 ? ? ? ?",
                "KeyedArchive__KeyedArchive": "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 51 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 89 7D F0 E8 ? ? ? ? C7 45 ? ? ? ? ? 8D 4F 08 E8 ? ? ? ? C6 45 FC 01 8D 4F 0C C7 07 ? ? ? ?",
                "KeyedArchive__Save": "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 0C 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F1 FF 35 ? ? ? ? 8D 4D E8 6A 04 68 ? ? ? ? E8 ? ? ? ?",
                "FilePath__FilePath": "55 8B EC 51 8B 55 08 8B C2 56 8B F1 57 89 75 FC 8D 78 01 8A 08 40 84 C9 75 F9 2B C7 8B CE 50 52 E8 ? ? ? ? 5F 8B C6 5E 8B E5 5D C2 04 00"
            },
            "debugLibrary": DEBUG_LIBRARY
        }

        libraryParamsFilePath = os.path.join(directoryExecutable, LIBRARY_PARAMS_FILENAME)
        with open(libraryParamsFilePath, "w") as paramsFile:
            json.dump(params, paramsFile, indent=4)

        print("[ParticlesGod] Prepared parameters for library at:", libraryParamsFilePath)

        library = os.path.abspath(LIBRARY_FILENAME)
        try:
            InjectDLL(processId, library)
            print("[ParticlesGod] Injection successful")
        except:
            print(f"[ParticlesGod] Failed to inject library in the game {WOTBLITZ_PROCESS_NAME}")
            return False

        print("[ParticlesGod] Library has been injected to the game, waiting for processing update")
        
        WaitLoadingDLL(processId, LIBRARY_FILENAME)

        print("[ParticlesGod] Library has finished processing, continue the script")

        updDataNodesId = maxDataNodesId
        for emitterFilePath in emitterFilePaths:
            emitterFilePathUpdated = emitterFilePath.replace(".yaml", "_upd.ka")

            if not os.path.isfile(emitterFilePathUpdated):
                print(f"[ParticlesGod] Emitter updated keyed archive file not found: {emitterFilePathUpdated}")
                return False
            
            dataNode = None
            with open(emitterFilePathUpdated, 'rb') as archiveFile:
                dataNode = KeyedArchive.Load(archiveFile)
                if dataNode == False:
                    print(f"[ParticlesGod] Failed to load updated emitter keyed archive file: {emitterFilePathUpdated}")
                    return False
                
            updDataNodesId += 1
            dataNode["##name"] = {VariantType.TYPENAME_STRING: "ParticleEmitterNode"}
            dataNode["#id"] = {VariantType.TYPENAME_BYTE_ARRAY: struct.pack("<Q", updDataNodesId)}
            dataNode = dict(sorted(dataNode.items()))

            ReplaceSpritePaths(dataNode)

            dataNodes = sceneArchive["#dataNodes"][VariantType.TYPENAME_VARIANT_VECTOR]
            dataNodes.append({VariantType.TYPENAME_KEYED_ARCHIVE: dataNode})

        print("[ParticlesGod] Particle Emitter Nodes has been prepared for hierarchy")

        try:
            hierarchy = sceneArchive["#hierarchy"][VariantType.TYPENAME_VARIANT_VECTOR]
            for child in hierarchy:
                archive = child[VariantType.TYPENAME_KEYED_ARCHIVE]
                maxDataNodesId = UpdateNestedEmitters(
                    archive,
                    maxDataNodesId,
                )
        except Exception as e:
            print("[ParticlesGod] Error while updating nested emitters:", e)
            return False
        
        print("[ParticlesGod] ParticleEffectComponent's has been updated successfully!")

        outputFileName = inputFileName.replace(".sc2", "_rebuild.sc2")
        OK = SceneFile.Serialize(sceneFile, outputFileName)
        if OK == False:
            print("[ParticlesGod] Failed to resave scene file")
            return False
        
        print("[ParticlesGod] SceneFile has been updated successfully!")

        if descriptor['geometryIdHash'] == 0:
            print("[ParticlesGod] Geometry ID hash in SceneFile descriptor is NULL - geometry file copy skipped")
        else:
            print("[ParticlesGod] Geometry ID hash in SceneFile descriptor is not NULL")

            inputGeometryFileName = inputFileName.replace(".sc2", ".scg")
            if not os.path.isfile(inputGeometryFileName):
                print(f"[ParticlesGod] Geometry file not found: {inputGeometryFileName}")
                return False
            
            outputGeometryFileName = outputFileName.replace(".sc2", ".scg")

            shutil.copy(inputGeometryFileName, outputGeometryFileName)
            if not os.path.isfile(outputGeometryFileName):
                print(f"[ParticlesGod] Geometry file has not been copied: {outputGeometryFileName}")
                return False
            
            print(f"[ParticlesGod] Geometry file has been copied successfully: {outputGeometryFileName}")

        print("[ParticlesGod] Script ended!")

if __name__ == "__main__":
    Main()
