import os
import subprocess
import shutil
import sys

GENERATOR         = "Visual Studio 17 2022"
ARCHITECTURE      = "x64"
GEN_MODE          = "Release"
TOOLS_BUILD_DIR   = "_build"
PRODUCTION        = True

PARTICLES_GOD     = "Update particle emitters from selected library file"

SCRIPT_PATH       = os.path.abspath(__file__)
SCRIPT_DIRECTORY  = os.path.dirname(SCRIPT_PATH)
PARENT_DIRECTORY  = os.path.dirname(SCRIPT_DIRECTORY)
DEPLOY_DIRECTORY  = os.path.join(SCRIPT_DIRECTORY, "_build")

BUILD_TOOLS = {
    "ResourceEditor": True,
    "FormatEditorCLI": True,
    "ResourceDLC": True,
    "ResourceDVPL": True,
    "ParticlesGod": True,
}

def RunCommand(cmd, cwd=None):
    """
    Execute a command in a subprocess and handle errors
    """
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd)
    if result.returncode != 0:
        print(f"❌ Command failed with return code {result.returncode}")
        return False
    
    return True

def Checks():
    """
    Checks if cmake is available in PATH
    """
    print(os.environ["PATH"])
    cmake_path = shutil.which("cmake")
    if cmake_path is None:
        print("❌ CMake not found in PATH. Please install CMake or add it to your PATH.")
        sys.exit()
    else:
        print(f"✅ CMake found: {cmake_path}")

def DeletePDBFiles():
    print("✅ Deleting all pdb files from deploy directory.")

    result = True
    for root, dirs, files in os.walk(DEPLOY_DIRECTORY):
        for file in files:
            if file.endswith(".pdb"):
                file_path = os.path.join(root, file)
                try:
                    os.remove(file_path)
                    print(f"✅ Deleted: {file_path}")
                except:
                    print(f"❌ Failed to delete {file_path}")
                    result = False
    return result

def BuildProgram(programName):
    dir_progr = os.path.join(PARENT_DIRECTORY, "Programs", f"{programName}")
    dir_build = os.path.join(dir_progr, TOOLS_BUILD_DIR)
    dir_deploy = os.path.join(dir_build, f"_{programName}")
    print(f"{programName} directory program:", dir_progr)
    print(f"{programName} directory build:", dir_build)
    print(f"{programName} directory deploy:", dir_deploy)

    # Si le répertoire existe déjà on le supprime pour un build propre
    if os.path.exists(dir_build):
        print(f"✅ Build directory already exists for {programName}, deleting it for clean.")
        try:
            shutil.rmtree(dir_build)
        except:
            print(f"❌ Failed to delete build directory for {programName}.")
            return
    
    # Le répertoire n’existe pas → on le génère
    print(f"🔧 Creating directory for {programName}: {dir_build}")
    try:
        os.makedirs(dir_build, exist_ok=True)
    except:
        print(f"❌ Failed to create build directory for {programName}.")
        return

    print(f"🔨 Running CMake generation for {programName}...")

    cmake_command = [
        f"cmake",
        f"-G{GENERATOR}",
        f"-A{ARCHITECTURE}",
        f"-DCMAKE_GENERATOR_PLATFORM={ARCHITECTURE}",
        f"-DCMAKE_BUILD_TYPE={GEN_MODE}",
        f"-DCMAKE_POLICY_DEFAULT_CMP0084=OLD",
        f"-DUNITY_BUILD=true",
        f"-DDEPLOY=true",
        f'-DDEPLOY_DIR={dir_deploy}',
        dir_progr,
        f"-B{dir_build}"
    ]
    
    if not RunCommand(cmake_command, cwd=dir_build):
        print("❌ CMake generation failed")
        return
    
    print(f"✅ CMake generation for {programName} succeeded.")

    print(f"⚙️  Building {programName} project...")
    if not RunCommand(["cmake", "--build", ".", "--config", GEN_MODE], cwd=dir_build):
        print(f"❌ Build failed for {programName}")
        return
    
    print(f"✅ Compilation of {programName} succeeded.")

    try:
        shutil.copytree(dir_deploy, DEPLOY_DIRECTORY, dirs_exist_ok=True)
    except:
        print(f"❌ Failed to copy compiled {programName} to deploy directory.")
        return
    
    print(f"✅ Copy of {programName} to deploy directory succeeded.")

def BuildParticlesGod():
    library_name = "ParticlesGodLibrary"

    dir_particles_god = os.path.join(PARENT_DIRECTORY, "Programs", "ParticlesGod")
    dir_scr_particles_god = os.path.join(dir_particles_god, "Python")
    dir_lib_particles_god = os.path.join(dir_particles_god, library_name)
    sln_lib_particles_god = os.path.join(dir_lib_particles_god, library_name + ".sln")

    generation_mode = "Release"

    msbuild_command = [
        f"msbuild",
        f"{sln_lib_particles_god}",
        f"/p:Configuration={generation_mode}",
        f"/p:Platform=x86"
    ]
    
    if not RunCommand(msbuild_command):
        print("❌ MSBuild generation failed for ParticlesGod")
        return
    
    GOD_DIRECTORY = os.path.join(DEPLOY_DIRECTORY, "Data", "ResourceEditor", "Plugins", PARTICLES_GOD)
    try:
        os.makedirs(GOD_DIRECTORY, exist_ok=True)
    except Exception as e:
        print(f"❌ Failed to create particles god directory: {e}")
        return
    
    dll_particles_god = os.path.join(dir_lib_particles_god, generation_mode, library_name + ".dll")
    print(f"Source: {dll_particles_god}")
    print(f"Destination: {GOD_DIRECTORY}")
    
    if not os.path.exists(dll_particles_god):
        print(f"❌ Source DLL not found: {dll_particles_god}")
        return
    
    try:
        shutil.copy(dll_particles_god, GOD_DIRECTORY)
        print(f"✅ Successfully copied {library_name}.dll")
    except Exception as e:
        print(f"❌ Failed to copy compiled {library_name} to deploy particle god directory: {e}")
        return
    
    try:
        for item in os.listdir(dir_scr_particles_god):
            source_item = os.path.join(dir_scr_particles_god, item)
            dest_item = os.path.join(GOD_DIRECTORY, item)
            if os.path.isfile(source_item):
                shutil.copy(source_item, dest_item)
            elif os.path.isdir(source_item):
                shutil.copytree(source_item, dest_item)

        print(f"✅ Successfully copied {library_name} scripts")
    except Exception as e:
        print(f"❌ Failed to copy {library_name} scripts to deploy particle god directory: {e}")
        return
    
def Main():
    print("🚀 TOOLSET BUILD SCRIPT STARTED")

    Checks()

    print("SCRIPT_PATH      :", SCRIPT_PATH)
    print("SCRIPT_DIRECTORY :", SCRIPT_DIRECTORY)
    print("PARENT_DIRECTORY :", PARENT_DIRECTORY)

    # Create clean deploy directory
    if os.path.exists(DEPLOY_DIRECTORY):
        print("✅ Deploy directory already exists, deleting it for clean.")
        try:
            shutil.rmtree(DEPLOY_DIRECTORY)
        except:
            print("❌ Failed to delete deploy directory.")
            return
        
    print(f"🔧 Creating deploy directory: {DEPLOY_DIRECTORY}")
    try:
        os.makedirs(DEPLOY_DIRECTORY, exist_ok=True)
    except:
        print("❌ Failed to create deploy directory.")
        return

    if BUILD_TOOLS["ResourceEditor"]:
        BuildProgram("ResourceEditor")

    if BUILD_TOOLS["FormatEditorCLI"]:
        BuildProgram("FormatEditorCLI")

    if BUILD_TOOLS["ResourceDLC"]:
        BuildProgram("ResourceDLC")

    if BUILD_TOOLS["ResourceDVPL"]:
        BuildProgram("ResourceDVPL")

    if BUILD_TOOLS["ParticlesGod"]:
        BuildParticlesGod()

    if not DeletePDBFiles():
        print("❌ Some pdb files could not be deleted.")
        return

    print("✅ Deploy has finished.")

if __name__ == "__main__":
    Main()