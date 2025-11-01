#pragma once

#include <Base/BaseTypes.h>

#include <Engine/Engine.h>
#include <Debug/DVAssertDefaultHandlers.h>
#include <Logger/Logger.h>

#include "Job/JobManager.h"

#include "Utils/CRC32.h"
#include "Compression/Compressor.h"
#include "Compression/LZ4Compressor.h"
#include "Compression/ZipCompressor.h"

#include "FileSystem/KeyedArchive.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"
#include "Scene3D/SceneFileV2.h"

#include "Utils/StringFormat.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <array>

namespace DAVA
{
class Engine;
class Window;
} // namespace DAVA

class ResourceDLC
{
public:
    enum eMethodUsed
    {
        Unpack = 0,
        Count
    };

    

    struct FooterBlock
    {
        std::array<char, 8> reserved;
        uint32_t metaDataCrc32;
        uint32_t metaDataSize;
        uint32_t infoCrc32;

        struct FooterBlockInfo
        {
            uint32_t numFiles;
            uint32_t namesSizeCompressed;
            uint32_t namesSizeOriginal;
            uint32_t filesTableSize;
            uint32_t filesTableCrc32;
            std::array<char, 4> packArchiveMarker;
        };

        FooterBlockInfo info;
    };

    struct FilesTableBlock
    {
        struct Names
        {
            std::vector<char> compressedNames;
            uint32_t compressedCrc32;
        };

        struct FilesData
        {
            struct Data
            {
                uint64_t startPosition;
                uint32_t compressedSize;
                uint32_t originalSize;
                uint32_t compressedCrc32;
                DAVA::Compressor::Type type;
                uint32_t originalCrc32;
                uint32_t metaIndex;
            };

            std::vector<Data> files;
        };

        FilesData data;
        Names names;
    };
    
    static DAVA::String GetMethodUsedDescription(eMethodUsed usedMethod);

    static bool ProcessUnpack(DAVA::Vector<DAVA::String> cmdline);
};
