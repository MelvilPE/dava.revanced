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

    void SetOpenProjectPath(const std::string& value);
    void SetLastLoadedScenePath(const std::string& value);
    void SetLastSavedScenePath(const std::string& value);
    void SetLastExportedBatchesDirectoryPath(const std::string& value);
    void SetLastLibrarySelectedFileSc2(const std::string& value);
    bool Serialize(DAVA::FilePath path);
};