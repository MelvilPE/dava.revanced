#include "Main.h"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

const std::string PROJECT_TITLE = "ParticlesGodLibrary";

void Main::Run()
{
    wchar_t executable[MAX_PATH];

    DWORD len = GetModuleFileNameW(NULL, executable, MAX_PATH);
    if (len <= 0)
    {
        MessageBox(NULL, "Failed to get process executable path", PROJECT_TITLE.c_str(), 16);
        return;
    }

    std::wstring paramsName = std::wstring(PROJECT_TITLE.begin(), PROJECT_TITLE.end()) + L".json";
    PathRemoveFileSpecW(executable);
    PathAppendW(executable, paramsName.c_str());

    FILE* paramsFile = nullptr;
    if (_wfopen_s(&paramsFile, executable, L"rb") != 0 || !paramsFile)
    {
        MessageBox(NULL, "Failed to load params file", PROJECT_TITLE.c_str(), 16);
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(paramsFile, buffer, sizeof(buffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(paramsFile);

    if (doc.HasParseError())
    {
        MessageBox(NULL, "Failed to load params file content", PROJECT_TITLE.c_str(), 16);
        return;
    }

    Config config;
    if (!config.Parse(doc))
    {
        MessageBox(NULL, "Failed to load params as library config ", PROJECT_TITLE.c_str(), 16);
        return;
    }
    
    InternalFunctionsCaller caller;
    caller.Init(config);

    for (const std::string& emitterPath : config.EMITTER_PATHS)
    {
        std::filesystem::path replaced(emitterPath);
        std::filesystem::path output = replaced.parent_path() / (replaced.stem().string() + "_upd.ka");
        std::string outputPath = output.string();

        void* filePath = ::operator new(config.TYPESIZE_FILE_PATH);
        caller.DAVA__FilePath__FilePath(filePath, emitterPath.c_str());
        if (!filePath)
        {
            MessageBox(NULL, "Failed to create input DAVA::FilePath", PROJECT_TITLE.c_str(), 16);
            return;
        }

        void* emitter = caller.DAVA__ParticleEmitter__LoadEmitterFromFile(filePath, true);
        if (!emitter)
        {
            MessageBox(NULL, "Failed to create DAVA::ParticleEmitter and load yaml", PROJECT_TITLE.c_str(), 16);
            return;
        }

        void* archive = ::operator new(config.TYPESIZE_KEYED_ARCHIVE);
        caller.DAVA__KeyedArchive__KeyedArchive(archive);

        caller.DAVA__ParticleSerializer__SaveEmitter(emitter, archive);

        void* outputFilePath = ::operator new(config.TYPESIZE_FILE_PATH);
        caller.DAVA__FilePath__FilePath(outputFilePath, outputPath.c_str());
        if (!outputFilePath)
        {
            MessageBox(NULL, "Failed to create DAVA::FilePath for saving output", PROJECT_TITLE.c_str(), 16);
            return;
        }

        bool OK = caller.DAVA__KeyedArchive__Save(archive, outputFilePath);
        if (!OK)
        {
            MessageBox(NULL, "Failed to save archive output as yaml file", PROJECT_TITLE.c_str(), 16);
            return;
        }
    }
}