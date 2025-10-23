#pragma once

#include <FileSystem/KeyedArchive.h>
#include <string>

class PythonPluginsSingleton
{
private:
    DAVA::KeyedArchive* object;

    PythonPluginsSingleton();
    ~PythonPluginsSingleton();

public:
    PythonPluginsSingleton(const PythonPluginsSingleton&) = delete;
    PythonPluginsSingleton& operator=(const PythonPluginsSingleton&) = delete;

    static PythonPluginsSingleton* GetInstance();

    void SetOpenProjectPath(const std::string& openProjectPath);
    void SetLastLoadedScenePath(const std::string& lastLoadedScenePath);
    void SetLastSavedScenePath(const std::string& lastSavedScenePath);
    void SetLastExportedBatchesDirectoryPath(const std::string& lastExportedBatchesDirectoryPath);
    bool Serialize(DAVA::FilePath path);
};