#include "ResourceDVPL.h"

#include <Debug/DVAssertDefaultHandlers.h>

#include <stdlib.h>
#include <signal.h>

using namespace DAVA;

void PrintUsage()
{
    printf(
    "Usage of ResourceDVPL:\n\n"

    "Method 0 = Unpack single DVPL file\n"
    "    -i  Input file path (DVPL)\n"
    "    -d  Delete original file\n\n"

    "Method 1 = Unpack all DVPL files from directory and subdirectories\n"
    "    -i  Input directory\n"
    "    -d  Delete original files\n\n"

    "Method 2 = Pack single file to DVPL\n"
    "    -i  Input file path\n"
    "    -c  Compression type\n"
    "           0 = No compression\n"
    "           1 = Lz4\n"
    "           2 = Lz4HC\n"
    "           3 = RFC1951\n"
    "           4 = Lz4Stream\n"
    "    -d  Delete original file\n\n"

    "Method 3 = Pack all files from directory and subdirectories to DVPL\n"
    "    -i  Input directory\n"
    "    -c  Compression type\n"
    "           0 = No compression\n"
    "           1 = Lz4\n"
    "           2 = Lz4HC\n"
    "           3 = RFC1951\n"
    "           4 = Lz4Stream\n"
    "    -d  Delete original files\n\n"

    "Examples:\n"
    "   Unpack single DVPL file\n"
    "      ResourceDVPL -m 0 -d -i E:\\OS\\input.sc2.dvpl\n\n"
    "   Unpack all DVPL files from directory and subdirectories\n"
    "      ResourceDVPL -m 1 -d -i E:\\OS\\\n\n"
    "   Pack single file to DVPL\n"
    "      ResourceDVPL -m 2 -d -i E:\\OS\\input.sc2 -c 2\n\n"
    "   Pack all files from directory and subdirectories to DVPL\n"
    "      ResourceDVPL -m 3 -d -i E:\\OS\\ -c 2\n\n");
}

void Process(DAVA::Vector<DAVA::String> cmdline)
{
    printf("\n");
    PrintUsage();
    printf("======================");
    printf("\n");

    ResourceDVPL::eMethodUsed usedMethod = ResourceDVPL::eMethodUsed::Count;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-m" && i + 1 < cmdline.size())
        {
            DAVA::String method = cmdline[i + 1];
            ++i;

            if (method == std::to_string(ResourceDVPL::eMethodUsed::UnpackFile))
            {
                usedMethod = ResourceDVPL::eMethodUsed::UnpackFile;
            }

            if (method == std::to_string(ResourceDVPL::eMethodUsed::UnpackDirectory))
            {
                usedMethod = ResourceDVPL::eMethodUsed::UnpackDirectory;
            }

            if (method == std::to_string(ResourceDVPL::eMethodUsed::PackFile))
            {
                usedMethod = ResourceDVPL::eMethodUsed::PackFile;
            }

            if (method == std::to_string(ResourceDVPL::eMethodUsed::PackDirectory))
            {
                usedMethod = ResourceDVPL::eMethodUsed::PackDirectory;
            }
        }
    }

    if (usedMethod == ResourceDVPL::eMethodUsed::Count)
    {
        printf("[Main::Process] Error: no method input (-m)\n");
        return;
    }

    switch (usedMethod)
    {
        case ResourceDVPL::eMethodUsed::UnpackFile:
        {
            if (!ResourceDVPL::ProcessUnpackFile(cmdline))
            {
                printf("[Main::Process] ProcessUnpackFile failed!\n");
            }
            break;
        }
        case ResourceDVPL::eMethodUsed::UnpackDirectory:
        {
            if (!ResourceDVPL::ProcessUnpackDirectory(cmdline))
            {
                printf("[Main::Process] ProcessUnpackDirectory failed!\n");
            }
            break;
        }
        case ResourceDVPL::eMethodUsed::PackFile:
        {
            if (!ResourceDVPL::ProcessPackFile(cmdline))
            {
                printf("[Main::Process] ProcessPackFile failed!\n");
            }
            break;
        }
        case ResourceDVPL::eMethodUsed::PackDirectory:
        {
            if (!ResourceDVPL::ProcessPackDirectory(cmdline))
            {
                printf("[Main::Process] ProcessPackDirectory failed!\n");
            }
            break;
        }
    }

    printf("[Main::Process] Ended\n");
}

bool ResourceDVPL::ProcessUnpackFile(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::String sourceInput = "";

    bool deleteOriginalFiles = false;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            sourceInput = cmdline[i + 1];
            ++i;
        }

        if (cmdline[i] == "-d")
        {
            deleteOriginalFiles = true;
        }
    }

    if (sourceInput.empty())
    {
        printf("[ResourceDVPL::ProcessUnpackFile] Error: no input (-i <sourcepath>)\n");
        return false;
    }

    if (!UnpackFileFromSource(sourceInput))
    {
        printf("[ResourceDVPL::ProcessUnpackFile] Failed to unpack input file\n");
        return false;
    }

    FileSystem* fs = GetEngineContext()->fileSystem;
    if (deleteOriginalFiles)
    {
        if (!fs->DeleteFile(FilePath(sourceInput)))
        {
            printf("[ResourceDVPL::ProcessUnpackFile] Failed deleting original file: %s\n", sourceInput.c_str());
            return false;
        }
    }
    
    return true;
}

bool ResourceDVPL::ProcessUnpackDirectory(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::String sourceInput = "";

    bool deleteOriginalFiles = false;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            sourceInput = cmdline[i + 1];
            ++i;
        }

        if (cmdline[i] == "-d")
        {
            deleteOriginalFiles = true;
        }
    }

    if (sourceInput.empty())
    {
        printf("[ResourceDVPL::ProcessUnpackDirectory] Error: no input (-i <sourcepath>)\n");
        return false;
    }

    FilePath inputPath(sourceInput);
    if (!inputPath.Exists())
    {
        printf("[ResourceDVPL::ProcessUnpackDirectory] Given input directory doesn't exist: %s\n", sourceInput.c_str());
        return false;
    }
    if (!inputPath.IsDirectoryPathname())
    {
        printf("[ResourceDVPL::ProcessUnpackDirectory] Given input path should be a directory: %s\n", sourceInput.c_str());
        return false;
    }

    FileSystem* fs = GetEngineContext()->fileSystem;
    Vector<FilePath> filePaths = fs->EnumerateFilesInDirectory(inputPath);

    uint32 filesProcessedCount = 0;
    bool filesProcessedResult = true;

    for (auto& filePath : filePaths)
    {
        if (filePath.GetExtension() != ".dvpl")
        {
            continue;
        }

        String absolutePath = filePath.GetAbsolutePathname();
        printf("[ResourceDVPL::ProcessUnpackDirectory] Processing file: %s\n", absolutePath.c_str());

        if (!UnpackFileFromSource(absolutePath))
        {
            printf("[ResourceDVPL::ProcessUnpackDirectory] Failed processing file: %s\n", absolutePath.c_str());
            filesProcessedResult = false;
            continue;
        }

        if (deleteOriginalFiles)
        {
            if (!fs->DeleteFile(filePath))
            {
                printf("[ResourceDVPL::ProcessUnpackDirectory] Failed deleting original file: %s\n", absolutePath.c_str());
                filesProcessedResult = false;
                continue;
            }
        }

        filesProcessedCount++;
    }

    printf("[ResourceDVPL::ProcessUnpackDirectory] Successfully wrote: %d files\n", filesProcessedCount);
    return filesProcessedResult;
}

bool ResourceDVPL::ProcessPackFile(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::String sourceInput = "";
    Compressor::Type compression = Compressor::Type::Count;

    bool deleteOriginalFiles = false;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            sourceInput = cmdline[i + 1];
            ++i;
        }

        if (cmdline[i] == "-c" && i + 1 < cmdline.size())
        {
            DAVA::String content = cmdline[i + 1];
            ++i;

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::None)))
            {
                compression = Compressor::Type::None;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4)))
            {
                compression = Compressor::Type::Lz4;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4HC)))
            {
                compression = Compressor::Type::Lz4HC;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::RFC1951)))
            {
                compression = Compressor::Type::RFC1951;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4Stream)))
            {
                compression = Compressor::Type::Lz4Stream;
            }
        }

        if (cmdline[i] == "-d")
        {
            deleteOriginalFiles = true;
        }
    }

    if (compression == Compressor::Type::Count)
    {
        printf("[ResourceDVPL::ProcessPackFile] Error: unknown or invalid or not set compression type (-c)\n");
        return false;
    }

    if (sourceInput.empty())
    {
        printf("[ResourceDVPL::ProcessPackFile] Error: no input (-i <sourcepath>)\n");
        return false;
    }

    if (!PackFileFromSource(sourceInput, compression))
    {
        printf("[ResourceDVPL::ProcessPackFile] Failed to pack input file\n");
        return false;
    }

    FileSystem* fs = GetEngineContext()->fileSystem;
    if (deleteOriginalFiles)
    {
        if (!fs->DeleteFile(FilePath(sourceInput)))
        {
            printf("[ResourceDVPL::ProcessPackFile] Failed deleting original file: %s\n", sourceInput.c_str());
            return false;
        }
    }

    return true;
}

bool ResourceDVPL::ProcessPackDirectory(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::String sourceInput = "";
    Compressor::Type compression = Compressor::Type::Count;

    bool deleteOriginalFiles = false;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            sourceInput = cmdline[i + 1];
            ++i;
        }

        if (cmdline[i] == "-c" && i + 1 < cmdline.size())
        {
            DAVA::String content = cmdline[i + 1];
            ++i;

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::None)))
            {
                compression = Compressor::Type::None;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4)))
            {
                compression = Compressor::Type::Lz4;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4HC)))
            {
                compression = Compressor::Type::Lz4HC;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::RFC1951)))
            {
                compression = Compressor::Type::RFC1951;
            }

            if (content == std::to_string(static_cast<uint32_t>(Compressor::Type::Lz4Stream)))
            {
                compression = Compressor::Type::Lz4Stream;
            }
        }

        if (cmdline[i] == "-d")
        {
            deleteOriginalFiles = true;
        }
    }

    if (compression == Compressor::Type::Count)
    {
        printf("[ResourceDVPL::ProcessPackDirectory] Error: unknown or invalid or not set compression type (-c)\n");
        return false;
    }

    FilePath inputPath(sourceInput);
    if (!inputPath.Exists())
    {
        printf("[ResourceDVPL::ProcessPackDirectory] Given input directory doesn't exist: %s\n", sourceInput.c_str());
        return false;
    }
    if (!inputPath.IsDirectoryPathname())
    {
        printf("[ResourceDVPL::ProcessPackDirectory] Given input path should be a directory: %s\n", sourceInput.c_str());
        return false;
    }

    FileSystem* fs = GetEngineContext()->fileSystem;
    Vector<FilePath> filePaths = fs->EnumerateFilesInDirectory(inputPath);

    uint32 filesProcessedCount = 0;
    bool filesProcessedResult = true;

    for (auto& filePath : filePaths)
    {
        if (filePath.GetExtension() == ".dvpl")
        {
            continue;
        }

        String absolutePath = filePath.GetAbsolutePathname();
        printf("[ResourceDVPL::ProcessPackDirectory] Processing file: %s\n", absolutePath.c_str());

        if (!PackFileFromSource(absolutePath, compression))
        {
            printf("[ResourceDVPL::ProcessPackDirectory] Failed processing file: %s\n", absolutePath.c_str());
            filesProcessedResult = false;
            continue;
        }

        FileSystem* fs = GetEngineContext()->fileSystem;
        if (deleteOriginalFiles)
        {
            if (!fs->DeleteFile(filePath))
            {
                printf("[ResourceDVPL::ProcessPackDirectory] Failed deleting original file: %s\n", absolutePath.c_str());
                filesProcessedResult = false;
                continue;
            }
        }

        filesProcessedCount++;
    }

    printf("[ResourceDVPL::ProcessPackDirectory] Successfully wrote: %d files\n", filesProcessedCount);
    return filesProcessedResult;
}

bool ResourceDVPL::UnpackFileFromSource(DAVA::String sourceInput)
{
    const Array<char8, 4> DVPL_FILE_MARKER{ { 'D', 'V', 'P', 'L' } };

    FilePath inputPath(sourceInput);
    if (!inputPath.Exists())
    {
        printf("[ResourceDVPL::UnpackFile] Given input file path doesn't exist: %s\n", sourceInput.c_str());
        return false;
    }
    if (inputPath.IsDirectoryPathname())
    {
        printf("[ResourceDVPL::UnpackFile] Given input file path shouldn't be a directory: %s\n", sourceInput.c_str());
        return false;
    }
    if (!inputPath.IsEqualToExtension(".dvpl"))
    {
        printf("[ResourceDVPL::UnpackFile] Given input file path should have an dvpl extension: %s\n", sourceInput.c_str());
        return false;
    }

    ScopedPtr<File> dvplFile(File::Create(inputPath, File::OPEN | File::READ));
    if (!dvplFile)
    {
        printf("[ResourceDVPL::UnpackFile] Failed to open input dvpl file: %s\n", sourceInput.c_str());
        return false;
    }

    uint64_t dvplFileSize = dvplFile->GetSize();

    DVPLFooterBlock footerBlock;
    if (dvplFileSize < sizeof(footerBlock))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to seek to DVPL Footer Block because file size less then footer\n");
        return false;
    }

    if (!dvplFile->Seek(dvplFileSize - sizeof(footerBlock), File::SEEK_FROM_START))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to seek to DVPL Footer Block\n");
        return false;
    }

    if (sizeof(footerBlock) != dvplFile->Read(&footerBlock, sizeof(footerBlock)))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to read DVPL Footer Block\n");
        return false;
    }

    if (footerBlock.packMarkerLite != DVPL_FILE_MARKER)
    {
        printf("[ResourceDVPL::UnpackFile] DVPL Footer Block has incorrect DVPL marker\n");
        return false;
    }

    if (!dvplFile->Seek(0, File::SEEK_FROM_START))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to seek to start of DVPL file\n");
        return false;
    }

    Vector<uint8> compressedBuffer;
    compressedBuffer.resize(footerBlock.sizeCompressed);

    Vector<uint8> uncompressedBuffer;
    uncompressedBuffer.resize(footerBlock.sizeUncompressed);

    if (footerBlock.sizeCompressed != dvplFile->Read(compressedBuffer.data(), footerBlock.sizeCompressed))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to read compressed file buffer\n");
        return false;
    }

    Compressor::Type compressionType = footerBlock.type;
    switch (compressionType)
    {
        case Compressor::Type::None:
        {
            uncompressedBuffer = compressedBuffer;
            break;
        }
        case Compressor::Type::Lz4:
        {
            if (!LZ4Compressor().Decompress(compressedBuffer, uncompressedBuffer))
            {
                printf("[ResourceDVPL::UnpackFile] Failed to uncompress compressed file buffer (LZ4Compressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::Lz4HC:
        {
            if (!LZ4HCCompressor().Decompress(compressedBuffer, uncompressedBuffer))
            {
                printf("[ResourceDVPL::UnpackFile] Failed to uncompress compressed file buffer (LZ4HCCompressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::RFC1951:
        {
            if (!ZipCompressor().Decompress(compressedBuffer, uncompressedBuffer))
            {
                printf("[ResourceDVPL::UnpackFile] Failed to uncompress compressed file buffer (ZipCompressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::Lz4Stream:
        {
            if (!LZ4StreamCompressor::LZ4StreamDecompress(compressedBuffer, uncompressedBuffer))
            {
                printf("[ResourceDVPL::UnpackFile] Failed to uncompress compressed file buffer (LZ4StreamCompressor)\n");
                return false;
            }
            break;
        }
        default:
        {
            printf("[ResourceDVPL::UnpackFile] Failed to uncompress file buffer, unknown compression type %d\n", compressionType);
            return false;
        }
    }

    FilePath outputPath(inputPath);
    outputPath.ReplaceExtension("");

    ScopedPtr<File> outputFile(File::Create(outputPath, File::CREATE | File::WRITE));
    if (!outputFile)
    {
        printf("[ResourceDVPL::UnpackFile] Failed to write uncompressed file buffer %s\n", sourceInput.c_str());
        return false;
    }

    if (footerBlock.sizeUncompressed != outputFile->Write(uncompressedBuffer.data(), footerBlock.sizeUncompressed))
    {
        printf("[ResourceDVPL::UnpackFile] Failed to write uncompressed file buffer %s\n", sourceInput.c_str());
        return false;
    }

    printf("[ResourceDVPL::UnpackFile] Successfully wrote %s\n", sourceInput.c_str());
    return true;
}

bool ResourceDVPL::PackFileFromSource(DAVA::String sourceInput, Compressor::Type compressionType)
{
    const Array<char8, 4> DVPL_FILE_MARKER{ { 'D', 'V', 'P', 'L' } };

    FilePath inputPath(sourceInput);
    if (!inputPath.Exists())
    {
        printf("[ResourceDVPL::PackFileFromSource] Given input file path doesn't exist: %s\n", sourceInput.c_str());
        return false;
    }
    if (inputPath.IsDirectoryPathname())
    {
        printf("[ResourceDVPL::PackFileFromSource] Given input file path shouldn't be a directory: %s\n", sourceInput.c_str());
        return false;
    }

    ScopedPtr<File> inputFile(File::Create(inputPath, File::OPEN | File::READ));
    if (!inputFile)
    {
        printf("[ResourceDVPL::PackFileFromSource] Failed to open input file: %s\n", sourceInput.c_str());
        return false;
    }

    uint64_t inputFileSize = inputFile->GetSize();

    Vector<uint8> uncompressedBuffer;
    uncompressedBuffer.resize(inputFileSize);

    Vector<uint8> compressedBuffer;

    if (inputFileSize != inputFile->Read(uncompressedBuffer.data(), static_cast<uint32>(inputFileSize)))
    {
        printf("[ResourceDVPL::PackFileFromSource] Failed to read uncompressed file buffer\n");
        return false;
    }

    switch (compressionType)
    {
        case Compressor::Type::None:
        {
            compressedBuffer = uncompressedBuffer;
            break;
        }
        case Compressor::Type::Lz4:
        {
            if (!LZ4Compressor().Compress(uncompressedBuffer, compressedBuffer))
            {
                printf("[ResourceDVPL::PackFileFromSource] Failed to compress uncompressed file buffer (LZ4Compressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::Lz4HC:
        {
            if (!LZ4HCCompressor().Compress(uncompressedBuffer, compressedBuffer))
            {
                printf("[ResourceDVPL::PackFileFromSource] Failed to compress uncompressed file buffer (LZ4HCCompressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::RFC1951:
        {
            if (!ZipCompressor().Compress(uncompressedBuffer, compressedBuffer))
            {
                printf("[ResourceDVPL::PackFileFromSource] Failed to compress uncompressed file buffer (ZipCompressor)\n");
                return false;
            }
            break;
        }
        case Compressor::Type::Lz4Stream:
        {
            if (!LZ4StreamCompressor::LZ4StreamCompress(uncompressedBuffer, compressedBuffer))
            {
                printf("[ResourceDVPL::PackFileFromSource] Failed to compress uncompressed file buffer (LZ4StreamCompressor)\n");
                return false;
            }
            break;
        }
        default:
        {
            printf("[ResourceDVPL::PackFileFromSource] Failed to compress file buffer, unknown compression type %d\n", compressionType);
            return false;
        }
    }

    DVPLFooterBlock footerBlock;
    uint32 compressedBufferSize = static_cast<uint32>(compressedBuffer.size());
    footerBlock.sizeUncompressed = static_cast<uint32>(inputFileSize);
    footerBlock.sizeCompressed = compressedBufferSize;
    footerBlock.crc32Compressed = CRC32::ForBuffer(compressedBuffer.data(), compressedBufferSize);
    footerBlock.type = compressionType;
    footerBlock.packMarkerLite = DVPL_FILE_MARKER;

    FilePath outputPath(inputPath);
    outputPath.ReplaceExtension(outputPath.GetExtension() + ".dvpl");
    String sourceOutput = outputPath.GetAbsolutePathname();

    ScopedPtr<File> outputFile(File::Create(outputPath, File::CREATE | File::WRITE));
    if (!outputFile)
    {
        printf("[ResourceDVPL::PackFileFromSource] Failed to write compressed file buffer %s\n", sourceOutput.c_str());
        return false;
    }

    if (compressedBufferSize != outputFile->Write(compressedBuffer.data(), compressedBufferSize))
    {
        printf("[ResourceDVPL::PackFileFromSource] Failed to write compressed file buffer\n");
        return false;
    }

    if (sizeof(footerBlock) != outputFile->Write(&footerBlock, sizeof(footerBlock)))
    {
        printf("[ResourceDVPL::PackFileFromSource] Failed to write DVPL Footer Block %s\n", sourceOutput.c_str());
        return false;
    }

    printf("[ResourceDVPL::PackFileFromSource] Successfully wrote %s\n", sourceOutput.c_str());
    return true;
}

int DAVAMain(DAVA::Vector<DAVA::String> cmdline)
{
    DAVA::Engine e;
    e.Init(eEngineRunMode::CONSOLE_MODE, {}, nullptr);

    e.update.Connect([&e, cmdline](float32)
    {
        // For debugging
        // std::cout << "Appuyez sur Entrée pour continuer..." << std::endl;
        // std::cin.get();

        Process(cmdline); 
        e.QuitAsync(0);
     });

    return e.Run();
}
