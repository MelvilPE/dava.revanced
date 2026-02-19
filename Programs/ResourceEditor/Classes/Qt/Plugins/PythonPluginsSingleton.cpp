#include "Classes/Qt/Plugins/PythonPluginsSingleton.h"
#include <FileSystem/KeyedArchive.h>

PythonPluginsSingleton* PythonPluginsSingleton::GetInstance()
{
    static PythonPluginsSingleton instance;
    return &instance;
}

PythonPluginsSingleton::PythonPluginsSingleton()
{
    object = new DAVA::KeyedArchive();
    object->SetString("lastLoadedScenePath", "");
    object->SetString("lastSavedScenePath", "");
    object->SetString("openProjectPath", "");
    object->SetString("lastExportedBatchesDirectoryPath", "");
    object->SetString("lastLibrarySelectedFileSc2", "");
}

PythonPluginsSingleton::~PythonPluginsSingleton()
{
    DAVA::SafeRelease(object);
}

void PythonPluginsSingleton::SetOpenProjectPath(const std::string& value)
{
    object->SetString("openProjectPath", value);
}

void PythonPluginsSingleton::SetLastLoadedScenePath(const std::string& value)
{
    object->SetString("lastLoadedScenePath", value);
}

void PythonPluginsSingleton::SetLastSavedScenePath(const std::string& value)
{
    object->SetString("lastSavedScenePath", value);
}

void PythonPluginsSingleton::SetLastExportedBatchesDirectoryPath(const std::string& value)
{
    object->SetString("lastExportedBatchesDirectoryPath", value);
}

void PythonPluginsSingleton::SetLastLibrarySelectedFileSc2(const std::string& value)
{
    object->SetString("lastLibrarySelectedFileSc2", value);
}

bool PythonPluginsSingleton::Serialize(DAVA::FilePath path)
{
    return object->SaveToYamlFile(path);
}
