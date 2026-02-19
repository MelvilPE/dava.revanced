import struct

import KeyedArchive

SCENE_FILE_HEADER_SIGNATURE = b"SFV2"

MINIMAL_SUPPORTED_VERSION = 30

SCENE_FILE_HEADER_SIZE = 12

def Deserialize(inputFile):
    result = {}

    header = inputFile.read(SCENE_FILE_HEADER_SIZE)
    if len(header) < SCENE_FILE_HEADER_SIZE:
        print("[SceneFile::Deserialize] Error: File too short!")
        return False

    signature, version, nodeCount = struct.unpack("<4sII", header)
    if signature != SCENE_FILE_HEADER_SIGNATURE:
        print("[SceneFile::Deserialize] Error: Wrong signature!")
        return False
    
    if version < MINIMAL_SUPPORTED_VERSION:
        print(f"[SceneFile::Deserialize] Error: Version is too old for that plugin : {version} required : {MINIMAL_SUPPORTED_VERSION}")
        return False
    
    tags = KeyedArchive.Load(inputFile)
    if tags == False:
        print("[SceneFile::Deserialize] Error: Tags archive is wrong!")
        return False

    descriptor = {
        "descriptorSize": struct.unpack("<I", inputFile.read(4))[0],
        "descriptorType": struct.unpack("<I", inputFile.read(4))[0]
    }

    if descriptor["descriptorSize"] >= 12:
        descriptor["geometryIdHash"] = struct.unpack("<Q", inputFile.read(8))[0]

    if descriptor["descriptorSize"] >= 20:
        descriptor["geometryDataHash"] = struct.unpack("<Q", inputFile.read(8))[0]

    sceneArchive = KeyedArchive.Load(inputFile)
    if sceneArchive == False:
        print("[SceneFile::Deserialize] Error: Scene archive is wrong!")
        return False

    result["header"] = {
        "signature": signature,
        "version": version,
        "nodeCount": nodeCount,
    }
    result["tags"] = tags
    result["descriptor"] = descriptor
    result["sceneArchive"] = sceneArchive

    return result

def Serialize(sceneFile, outputPath):
    with open(outputPath, "wb") as outputFile:
        header = sceneFile["header"]
        outputFile.write(struct.pack(
            "<4sII",
            header["signature"],
            header["version"],
            header["nodeCount"]
        ))

        OK = KeyedArchive.Save(outputFile, sceneFile["tags"])
        if OK == False:
            print("[SceneFile::Serialize] Error: Failed to save scene tags!")
            return False
        
        descriptor = sceneFile["descriptor"]
        outputFile.write(struct.pack("<I", descriptor["descriptorSize"]))
        outputFile.write(struct.pack("<I", descriptor["descriptorType"]))

        if descriptor["descriptorSize"] >= 12:
            outputFile.write(struct.pack(
                "<Q",
                descriptor.get("geometryIdHash", 0)
            ))

        if descriptor["descriptorSize"] >= 20:
            outputFile.write(struct.pack(
                "<Q",
                descriptor.get("geometryDataHash", 0)
            ))

        OK = KeyedArchive.Save(outputFile, sceneFile["sceneArchive"])
        if OK == False:
            print("[SceneFile::Serialize] Error: Failed to save scene archive!")
            return False

if __name__ == "__main__":
    deserialized = Deserialize("Leopard1.sc2")
    if deserialized == False:
        print("Error: Failed to deserialize the scene file!")
    else:
        Serialize(deserialized, "Leopard1_rebuild.sc2")
