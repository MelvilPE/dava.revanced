#pragma once

#include <Base/BaseTypes.h>

#include <Engine/Engine.h>
#include <Debug/DVAssertDefaultHandlers.h>
#include <Logger/Logger.h>

#include "Job/JobManager.h"

#include "FileSystem/KeyedArchive.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"
#include "Scene3D/SceneFileV2.h"

#include "Utils/StringFormat.h"

#include <iostream>
#include <string>
#include <vector>

namespace DAVA
{
class Engine;
class Window;
} // namespace DAVA

class FormatEditorCLI
{
public:
    enum eMethodUsed
    {
        KeyedArchiveToYaml = 0,
        YamlToKeyedArchive,
        SceneFileToYaml,
        YamlToSceneFile,
        Count
    };
    
    static DAVA::String GetMethodUsedDescription(eMethodUsed usedMethod);

    static bool ProcessKeyedArchiveToYaml(DAVA::FilePath inputPath, DAVA::FilePath outputPath);
    static bool ProcessYamlToKeyedArchive(DAVA::FilePath inputPath, DAVA::FilePath outputPath);
    static bool ProcessSceneFileToYaml(DAVA::FilePath inputPath, DAVA::FilePath outputPath);
    static bool ProcessYamlToSceneFile(DAVA::FilePath inputPath, DAVA::FilePath outputPath);
};
