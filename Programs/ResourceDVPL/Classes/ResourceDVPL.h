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

class ResourceDVPL
{
public:
    enum eMethodUsed
    {
        UnpackFile = 0,
        UnpackDirectory,
        PackFile,
        PackDirectory,
        Count
    };

    struct DVPLFooterBlock
    {
        uint32_t sizeUncompressed;
        uint32_t sizeCompressed;
        uint32_t crc32Compressed;
        DAVA::Compressor::Type type;
        std::array<char, 4> packMarkerLite;
    };

    static bool ProcessUnpackFile(DAVA::Vector<DAVA::String> cmdline);
    static bool ProcessUnpackDirectory(DAVA::Vector<DAVA::String> cmdline);
    static bool ProcessPackFile(DAVA::Vector<DAVA::String> cmdline);
    static bool ProcessPackDirectory(DAVA::Vector<DAVA::String> cmdline);

    static bool UnpackFileFromSource(DAVA::String sourceInput);
    static bool PackFileFromSource(DAVA::String sourceInput, DAVA::Compressor::Type compressionType);
};
