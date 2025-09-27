#include "FormatEditorCLI.h"

#include <Debug/DVAssertDefaultHandlers.h>
#include <Engine/Engine.h>
#include <Logger/Logger.h>

#include <stdlib.h>
#include <signal.h>

using namespace DAVA;

void PrintUsage()
{
    printf("Usage:\n");
    printf("FormatEditorCLI -i {inputfile} -o {outputfile} -m {method}\n\n");

    printf("Options:\n");
    printf("\t-i Input file path\n");
    printf("\t-o Output file path\n");
    printf("\t-m Method used:\n");

    printf("\t\t0 = Keyed archive to yaml\n");
    printf("\t\t1 = Yaml to keyed archive\n");
    printf("\t\t2 = Scene file to yaml\n");
    printf("\t\t3 = Yaml to scene file\n");

    printf("\n");
    printf("Example for scene file to yaml\n");
    printf("FormatEditorCLI -i E:\\OS\\input.sc2 -o E:\\OS\\output.yaml -m 2\n");
    printf("\n");
}

DAVA::String FormatEditorCLI::GetMethodUsedDescription(eMethodUsed usedMethod)
{
    switch (usedMethod)
    {
    case FormatEditorCLI::eMethodUsed::KeyedArchiveToYaml:
        return "Keyed Archive To Yaml";
    case FormatEditorCLI::eMethodUsed::YamlToKeyedArchive:
        return "Yaml To Keyed Archive";
    case FormatEditorCLI::eMethodUsed::SceneFileToYaml:
        return "Scene File To Yaml";
    case FormatEditorCLI::eMethodUsed::YamlToSceneFile:
        return "Yaml To Scene File";
    }

    return "[FormatEditorCLI::GetMethodUsedDescription] Unknown method description";
}

void Process(DAVA::Vector<DAVA::String> cmdline)
{
    printf("\n");
    PrintUsage();
    printf("======================");
    printf("\n");


    DAVA::String inputFile;
    DAVA::String outputFile;

    FormatEditorCLI::eMethodUsed usedMethod = FormatEditorCLI::eMethodUsed::Count;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            inputFile = cmdline[i + 1];
            ++i;
        }
        else if (cmdline[i] == "-o" && i + 1 < cmdline.size())
        {
            outputFile = cmdline[i + 1];
            ++i;
        }
        else if (cmdline[i] == "-m" && i + 1 < cmdline.size())
        {
            DAVA::String method = cmdline[i + 1];
            ++i;

            if (method == std::to_string(FormatEditorCLI::eMethodUsed::KeyedArchiveToYaml))
            {
                usedMethod = FormatEditorCLI::eMethodUsed::KeyedArchiveToYaml;
            }
            else if (method == std::to_string(FormatEditorCLI::eMethodUsed::YamlToKeyedArchive))
            {
                usedMethod = FormatEditorCLI::eMethodUsed::YamlToKeyedArchive;
            }
            else if (method == std::to_string(FormatEditorCLI::eMethodUsed::SceneFileToYaml))
            {
                usedMethod = FormatEditorCLI::eMethodUsed::SceneFileToYaml;
            }
            else if (method == std::to_string(FormatEditorCLI::eMethodUsed::YamlToSceneFile))
            {
                usedMethod = FormatEditorCLI::eMethodUsed::YamlToSceneFile;
            }
        }
    }

    if (inputFile.empty())
    {
        printf("[Main::Process] Error: no input file (-i <filepath>)\n");
        return;
    }
    if (outputFile.empty())
    {
        printf("[Main::Process] Error: no output file (-o <filepath>)\n");
        return;
    }

    FilePath inputFilePath(inputFile);
    if (!inputFilePath.Exists())
    {
        printf("[Main::Process] Given input file doesn't exist: %s\n", inputFile.c_str());
        return;
    }

    if (usedMethod == FormatEditorCLI::eMethodUsed::Count)
    {
        printf("[Main::Process] Error: no method input (-m)\n");
        return;
    }

    printf("[Main::Process] Input file: %s\n", inputFile.c_str());
    printf("[Main::Process] Output file: %s\n", outputFile.c_str());
    printf("[Main::Process] Method used: %s\n", FormatEditorCLI::GetMethodUsedDescription(usedMethod).c_str());

    FilePath inputPath(inputFile);
    FilePath outputPath(outputFile);

    switch (usedMethod)
    {
    case FormatEditorCLI::eMethodUsed::KeyedArchiveToYaml:
        FormatEditorCLI::ProcessKeyedArchiveToYaml(inputPath, outputPath);
        break;
    case FormatEditorCLI::eMethodUsed::YamlToKeyedArchive:
        FormatEditorCLI::ProcessYamlToKeyedArchive(inputPath, outputPath);
        break;
    case FormatEditorCLI::eMethodUsed::SceneFileToYaml:
        FormatEditorCLI::ProcessSceneFileToYaml(inputPath, outputPath);
        break;
    case FormatEditorCLI::eMethodUsed::YamlToSceneFile:
        FormatEditorCLI::ProcessYamlToSceneFile(inputPath, outputPath);
        break;
    }

    printf("[Main::Process] Ended\n");
}

bool FormatEditorCLI::ProcessKeyedArchiveToYaml(DAVA::FilePath inputPath, DAVA::FilePath outputPath)
{
    ScopedPtr<KeyedArchive> archive(new KeyedArchive());
    if (!archive->Load(inputPath))
    {
        printf("[FormatEditorCLI::ProcessKeyedArchiveToYaml] Failed to load keyed archive file !\n");
        return false;
    }

    if (!archive->SaveToYamlFile(outputPath))
    {
        printf("[FormatEditorCLI::ProcessKeyedArchiveToYaml] Failed to save yaml file !\n");
        return false;
    }

    return true;
}

bool FormatEditorCLI::ProcessYamlToKeyedArchive(DAVA::FilePath inputPath, DAVA::FilePath outputPath)
{
    ScopedPtr<KeyedArchive> archive(new KeyedArchive());
    if (!archive->LoadFromYamlFile(inputPath))
    {
        printf("[FormatEditorCLI::ProcessYamlToKeyedArchive] Failed to load yaml from file !\n");
        return false;
    }

    if (!archive->Save(outputPath))
    {
        printf("[FormatEditorCLI::ProcessYamlToKeyedArchive] Failed to save keyed archive to file !\n");
        return false;
    }

    return true;
}

bool FormatEditorCLI::ProcessSceneFileToYaml(DAVA::FilePath inputPath, DAVA::FilePath outputPath)
{
    const ScopedPtr<File> file(File::Create(inputPath, File::OPEN | File::READ));
    if (!file)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to open scene file !\n");
        return false;
    }

    SceneFileV2::Header header;
    const bool headerValid = SceneFileV2::ReadHeader(header, file);
    if (!headerValid)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to read scene file header !\n");
        return false;
    }

    if (header.version < WORLD_OF_TANKS_BLITZ_7_8_VERSION)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to read scene file header - version is too old for this tool (30+ required for now)!\n");
        return false;
    }

    VersionInfo::SceneVersion dumb;
    dumb.version = header.version;

    const bool headerTagsValid = SceneFileV2::ReadVersionTags(dumb, file);
    if (!headerTagsValid)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to read scene file header version tags!\n");
        return false;
    }

    SceneFileV2::Descriptor descriptor;
    const bool descriptorValid = SceneFileV2::ReadDescriptor(file, descriptor);
    if (!descriptorValid)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to read scene file descriptor!\n");
        return false;
    }

    ScopedPtr<KeyedArchive> sceneArchive(new KeyedArchive());
    const bool loadedArchive = sceneArchive->Load(file);
    if (!loadedArchive)
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to read scene archive in scene file!\n");
        return false;
    }

    ScopedPtr<KeyedArchive> wholeArchive(new KeyedArchive());
    ScopedPtr<KeyedArchive> headeArchive(new KeyedArchive());
    ScopedPtr<KeyedArchive> descrArchive(new KeyedArchive());

    headeArchive->SetUInt32("version", header.version);
    headeArchive->SetUInt32("nodeCount", header.nodeCount);
    descrArchive->SetUInt32("size", descriptor.size);
    descrArchive->SetUInt32("fileType", descriptor.fileType);
    descrArchive->SetUInt64("geometryIdHash", descriptor.geometryIdHash);
    descrArchive->SetUInt64("geometryDataHash", descriptor.geometryDataHash);

    wholeArchive->SetArchive("header", headeArchive);
    wholeArchive->SetArchive("descriptor", descrArchive);
    wholeArchive->SetArchive("sceneArchive", sceneArchive);

    if (!wholeArchive->SaveToYamlFile(outputPath))
    {
        printf("[FormatEditorCLI::ProcessSceneFileToYaml] Failed to save yaml representation of scene file!\n");
        return false;
    }

    return true;
}

bool FormatEditorCLI::ProcessYamlToSceneFile(DAVA::FilePath inputPath, DAVA::FilePath outputPath)
{
    ScopedPtr<KeyedArchive> wholeArchive(new KeyedArchive());
    if (!wholeArchive->LoadFromYamlFile(inputPath))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to load scene file content from yaml file!\n");
        return false;
    }

    if (!wholeArchive->IsKeyExists("header") || !wholeArchive->IsKeyExists("descriptor"))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Header || VersionTags || Descriptor parts are missing in loaded scene file yaml content!\n");
        return false;
    }

    KeyedArchive* headeArchive = wholeArchive->GetArchive("header");
    KeyedArchive* descrArchive = wholeArchive->GetArchive("descriptor");
    KeyedArchive* sceneArchive = wholeArchive->GetArchive("sceneArchive");

    SceneFileV2::Header header;
    header.signature = Array<char, 4>{ 'S', 'F', 'V', '2' };
    header.version = static_cast<int32>(headeArchive->GetUInt32("version"));
    header.nodeCount = headeArchive->GetUInt32("nodeCount", header.nodeCount);

    VersionInfo::SceneVersion version;
    version.version = header.version;

    SceneFileV2::Descriptor descriptor;
    descriptor.size = descrArchive->GetUInt32("size");
    descriptor.fileType = descrArchive->GetUInt32("fileType");
    descriptor.geometryIdHash = descrArchive->GetUInt64("geometryIdHash");
    descriptor.geometryDataHash = descrArchive->GetUInt64("geometryDataHash");

    ScopedPtr<File> file(File::Create(outputPath, File::CREATE | File::WRITE));
    if (!file)
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to create scene file\n");
        return false;
    }

    if (!SceneFileV2::WriteHeader(file, header))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to write header to scene file\n");
        return false;
    }

    if (!SceneFileV2::WriteVersionTags(file))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to write version tags to scene file\n");
        return false;
    }

    if (!SceneFileV2::WriteDescriptor(file, descriptor))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to write descriptor to scene file\n");
        return false;
    }

    if (!sceneArchive->Save(file))
    {
        printf("[FormatEditorCLI::ProcessYamlToSceneFile] Failed to write scene archive to scene file\n");
        return false;
    }

    return true;
}

int DAVAMain(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::Engine e;
    e.Init(eEngineRunMode::CONSOLE_MODE, {}, nullptr);

    e.update.Connect([&e, cmdline](float32)
    {
        Process(cmdline); 
        e.QuitAsync(0);
     });

    return e.Run();
}
