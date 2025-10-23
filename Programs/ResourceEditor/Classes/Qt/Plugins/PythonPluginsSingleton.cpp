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
}

PythonPluginsSingleton::~PythonPluginsSingleton()
{
    DAVA::SafeRelease(object);
}

void PythonPluginsSingleton::SetOpenProjectPath(const std::string& openProjectPath)
{
    object->SetString("openProjectPath", openProjectPath);
}

void PythonPluginsSingleton::SetLastLoadedScenePath(const std::string& lastLoadedScenePath)
{
    object->SetString("lastLoadedScenePath", lastLoadedScenePath);
}

void PythonPluginsSingleton::SetLastSavedScenePath(const std::string& lastSavedScenePath)
{
    object->SetString("lastSavedScenePath", lastSavedScenePath);
}

void PythonPluginsSingleton::SetLastExportedBatchesDirectoryPath(const std::string& lastExportedBatchesDirectoryPath)
{
    object->SetString("lastExportedBatchesDirectoryPath", lastExportedBatchesDirectoryPath);
}

bool PythonPluginsSingleton::Serialize(DAVA::FilePath path)
{
    return object->SaveToYamlFile(path);
}
