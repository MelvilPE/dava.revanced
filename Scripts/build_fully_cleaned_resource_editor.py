import os
import subprocess
import shutil
import sys

GENERATOR         = "Visual Studio 17 2022"
ARCHITECTURE      = "x64"
GEN_MODE          = "Release"
TOOLS_BUILD_DIR   = "_build"
PRODUCTION        = True

SCRIPT_PATH       = os.path.abspath(__file__)          # C:\Users\France\Documents\build_resource_editor.py
SCRIPT_DIRECTORY  = os.path.dirname(SCRIPT_PATH)       # C:\Users\France\Documents
PARENT_DIRECTORY  = os.path.dirname(SCRIPT_DIRECTORY)  # C:\Users\France
DEPLOY_DIRECTORY  = os.path.join(SCRIPT_DIRECTORY, "_build")

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

def SpoofExecutables():
    print("✅ Spoofing all exe files from deploy directory.")

    old_sequence = b"Melvil"
    new_sequence = b"France"

    result = True
    for root, dirs, files in os.walk(DEPLOY_DIRECTORY):
        for file in files:
            if file.endswith(".exe"):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'rb') as file:
                        data = file.read()

                    if old_sequence in data:
                        data = data.replace(old_sequence, new_sequence)
                        with open(file_path, 'wb') as file:
                            file.write(data)

                        print(f"✅ Updated: {file_path}")
                except:
                    print(f"❌ Failed to update {file_path}")
                    result = False
    return result

def Main():
    print("🚀 RESOURCE EDITOR BUILD SCRIPT STARTED")

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

    # Build de ResourceEditor
    dir_progr = os.path.join(PARENT_DIRECTORY, "Programs", "ResourceEditor")
    dir_build = os.path.join(dir_progr, TOOLS_BUILD_DIR)
    dir_deploy = os.path.join(dir_build, f"_ResourceEditor")
    print("ResourceEditor directory program:", dir_progr)
    print("ResourceEditor directory build:", dir_build)
    print("ResourceEditor directory deploy:", dir_deploy)

    # Si le répertoire existe déjà on le supprime pour un build propre
    if os.path.exists(dir_build):
        print("✅ Build directory already exists for ResourceEditor, deleting it for clean.")
        try:
            shutil.rmtree(dir_build)
        except:
            print("❌ Failed to delete build directory for ResourceEditor.")
            return
    
    # Le répertoire n’existe pas → on le génère
    print(f"🔧 Creating directory for ResourceEditor: {dir_build}")
    try:
        os.makedirs(dir_build, exist_ok=True)
    except:
        print("❌ Failed to create build directory for ResourceEditor.")
        return

    print("🔨 Running CMake generation for ResourceEditor...")

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
    
    print("✅ CMake generation for ResourceEditor succeeded.")

    print("⚙️  Building ResourceEditor project...")
    if not RunCommand(["cmake", "--build", ".", "--config", GEN_MODE], cwd=dir_build):
        print("❌ Build failed")
        return
    
    print("✅ Compilation of ResourceEditor succeeded.")

    try:
        shutil.copytree(dir_deploy, DEPLOY_DIRECTORY, dirs_exist_ok=True)
    except:
        print("❌ Failed to copy compiled ResourceEditor to deploy directory.")
        return
    
    print("✅ Copy of ResourceEditor to deploy directory succeeded.")
    
    # Build de FormatEditorCLI
    dir_progr = os.path.join(PARENT_DIRECTORY, "Programs", "FormatEditorCLI")
    dir_build = os.path.join(dir_progr, TOOLS_BUILD_DIR)
    dir_deploy = os.path.join(dir_build, f"_FormatEditorCLI")
    print("FormatEditorCLI directory program:", dir_progr)
    print("FormatEditorCLI directory build:", dir_build)
    print("FormatEditorCLI directory deploy:", dir_deploy)

    # Si le répertoire existe déjà on le supprime pour un build propre
    if os.path.exists(dir_build):
        print("✅ Build directory already exists for FormatEditorCLI, deleting it for clean.")
        try:
            shutil.rmtree(dir_build)
        except:
            print("❌ Failed to delete build directory for FormatEditorCLI.")
            return
    
    # Le répertoire n’existe pas → on le génère
    print(f"🔧 Creating directory for FormatEditorCLI: {dir_build}")
    try:
        os.makedirs(dir_build, exist_ok=True)
    except:
        print("❌ Failed to create build directory for FormatEditorCLI.")
        return

    print("🔨 Running CMake generation for FormatEditorCLI...")

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
    
    print("✅ CMake generation for FormatEditorCLI succeeded.")

    print("⚙️  Building FormatEditorCLI project...")
    if not RunCommand(["cmake", "--build", ".", "--config", GEN_MODE], cwd=dir_build):
        print("❌ Build failed")
        return
    
    print("✅ Compilation of FormatEditorCLI succeeded.")
    
    FormatEditorCLI_exe = os.path.join(dir_deploy, "FormatEditorCLI.exe")
    
    try:
        shutil.copy2(FormatEditorCLI_exe, DEPLOY_DIRECTORY)
    except:
        print("❌ Failed to copy FormatEditorCLI.exe file to deploy directory.")
        return

    print("✅ Copy of FormatEditorCLI to deploy directory succeeded.")

    if not DeletePDBFiles():
        print("❌ Some pdb files could not be deleted.")
        return
    
    if not SpoofExecutables():
        print("❌ Some exe files could not be spoofed.")
        return

    print("✅ Deploy has finished.")

if __name__ == "__main__":
    Main()