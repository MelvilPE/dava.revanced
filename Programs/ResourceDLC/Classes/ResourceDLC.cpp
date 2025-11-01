#include "ResourceDLC.h"

#include <Debug/DVAssertDefaultHandlers.h>

#include <stdlib.h>
#include <signal.h>

using namespace DAVA;

void PrintUsage()
{
    printf(
        "Usage of ResourceDLC:\n\n"

        "Method 0 = Unpack DVPK / DVPM+DVPD to output directory:\n"
        "  Unpacking:\n"
        "    -i  Input file path (DVPK / DVPM+DVPD)\n"
        "    -o  Output directory\n"
        "    -m  Method used : 0\n\n"

        "Example:\n"
        "  ResourceDLC -i E:\\OS\\input.dvpm -o E:\\OS\\Output\\ -m 0\n\n"
    );
}

DAVA::String ResourceDLC::GetMethodUsedDescription(eMethodUsed usedMethod)
{
    switch (usedMethod)
    {
    case ResourceDLC::eMethodUsed::Unpack:
        return "Unpack DVPK / DVPM+DVPD to output directory";
    }

    return "[ResourceDLC::GetMethodUsedDescription] Unknown method description";
}

void Process(DAVA::Vector<DAVA::String> cmdline)
{
    printf("\n");
    PrintUsage();
    printf("======================");
    printf("\n");

    ResourceDLC::eMethodUsed usedMethod = ResourceDLC::eMethodUsed::Count;

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-m" && i + 1 < cmdline.size())
        {
            DAVA::String method = cmdline[i + 1];
            ++i;

            if (method == std::to_string(ResourceDLC::eMethodUsed::Unpack))
            {
                usedMethod = ResourceDLC::eMethodUsed::Unpack;
            }
        }
    }

    if (usedMethod == ResourceDLC::eMethodUsed::Count)
    {
        printf("[Main::Process] Error: no method input (-m)\n");
        return;
    }

    switch (usedMethod)
    {
    case ResourceDLC::eMethodUsed::Unpack:
        if (!ResourceDLC::ProcessUnpack(cmdline))
        {
            printf("[Main::Process] ProcessUnpack failed!\n");
        }
        break;
    }

    printf("[Main::Process] Ended\n");
}

bool ResourceDLC::ProcessUnpack(DAVA::Vector<DAVA::String> cmdline)
{
    const Array<char8, 4> DVPK_FILE_MARKER{ { 'D', 'V', 'P', 'K' } };
    const Array<char8, 4> DVPM_FILE_MARKER{ { 'D', 'V', 'P', 'M' } };
    const Array<char8, 4> DVPD_FILE_MARKER{ { 'D', 'V', 'P', 'D' } };

    DAVA::String sourceImport = "";
    DAVA::String sourceOutput = "";

    for (size_t i = 0; i < cmdline.size(); ++i)
    {
        if (cmdline[i] == "-i" && i + 1 < cmdline.size())
        {
            sourceImport = cmdline[i + 1];
            ++i;
        }
        else if (cmdline[i] == "-o" && i + 1 < cmdline.size())
        {
            sourceOutput = cmdline[i + 1];
            ++i;
        }
    }

    if (sourceImport.empty())
    {
        printf("[ResourceDLC::ProcessUnpack] Error: no input (-i <sourcepath>)\n");
        return false;
    }

    if (sourceOutput.empty())
    {
        printf("[ResourceDLC::ProcessUnpack] Error: no output (-o <sourcepath>)\n");
        return false;
    }

    FilePath inputPath(sourceImport);
    if (!inputPath.Exists())
    {
        printf("[ResourceDLC::ProcessUnpack] Given input file path doesn't exist: %s\n", sourceImport.c_str());
        return false;
    }

    FilePath outputPath(sourceOutput);
    if (!outputPath.Exists() || !outputPath.IsDirectoryPathname())
    {
        printf("[ResourceDLC::ProcessUnpack] Given output directory doesn't exist: %s\n", sourceOutput.c_str());
        return false;
    }

    printf("[ResourceDLC::ProcessUnpack] Input: %s\n", sourceImport.c_str());
    printf("[ResourceDLC::ProcessUnpack] Output: %s\n", sourceOutput.c_str());

    bool isDvpk = false;
    bool isDvpm = false;
    if (inputPath.IsEqualToExtension(".dvpk"))
    {
        isDvpk = true;
    }
    if (inputPath.IsEqualToExtension(".dvpm"))
    {
        isDvpm = true;
    }
    if (!isDvpk && !isDvpm)
    {
        printf("[ResourceDLC::ProcessUnpack] Given input file path doesn't have dvpk or dvpm extension: %s\n", sourceImport.c_str());
        return false;
    }

    if (isDvpk)
    {
        ScopedPtr<File> dvpkFile(File::Create(inputPath, File::OPEN | File::READ));
        if (!dvpkFile)
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to open input dvpk file: %s\n", sourceImport.c_str());
            return false;
        }

        uint64_t dvpkFileSize = dvpkFile->GetSize();

        FooterBlock footerBlock;
        if (dvpkFileSize < sizeof(footerBlock))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Footer Block because file size less then pack footer\n");
            return false;
        }
        
        if (!dvpkFile->Seek(dvpkFileSize - sizeof(footerBlock), File::SEEK_FROM_START))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Footer Block\n");
            return false;
        }
        
        if (sizeof(footerBlock) != dvpkFile->Read(&footerBlock, sizeof(footerBlock)))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to read Footer Block\n");
            return false;
        }

        if (footerBlock.info.packArchiveMarker != DVPK_FILE_MARKER)
        {
            printf("[ResourceDLC::ProcessUnpack] FooterBlock has incorrect DVPK marker\n");
            return false;
        }

        printf("[ResourceDLC::ProcessUnpack] Number of files : %d \n", footerBlock.info.numFiles);

        if (!dvpkFile->Seek(dvpkFileSize - (sizeof(footerBlock) + footerBlock.info.filesTableSize), File::SEEK_FROM_START))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Files Table Block\n");
            return false;
        }

        Vector<FilesTableBlock::FilesData::Data> filesDatas;
        for (uint32_t fileIndex = 0; fileIndex < footerBlock.info.numFiles; fileIndex++)
        {
            FilesTableBlock::FilesData::Data fileData;
            if (sizeof(fileData) != dvpkFile->Read(&fileData, sizeof(fileData)))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to read a files data from table\n");
                return false;
            }

            filesDatas.push_back(fileData);
        }

        Vector<uint8> compressedNames;

        Vector<uint8> uncompressedNames;
        uncompressedNames.resize(footerBlock.info.namesSizeOriginal);

        for (uint32_t byteIndex = 0; byteIndex < footerBlock.info.namesSizeCompressed; byteIndex++)
        {
            uint8 byteAppend;
            if (sizeof(byteAppend) != dvpkFile->Read(&byteAppend, sizeof(byteAppend)))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to read compressedNames\n");
                return false;
            }

            compressedNames.push_back(byteAppend);
        }

        uint32 compressedNamesCRC32 = NULL;
        if (sizeof(compressedNamesCRC32) != dvpkFile->Read(&compressedNamesCRC32, sizeof(compressedNamesCRC32)))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to read compressedNamesCRC32\n");
            return false;
        }

        if (!LZ4HCCompressor().Decompress(compressedNames, uncompressedNames))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressedNames\n");
            return false;
        }

        Vector<String> namesList;
        String currentName;
        for (uint8 byte : uncompressedNames)
        {
            if (byte == 0x00)
            {
                if (!currentName.empty())
                {
                    namesList.push_back(currentName);
                    currentName.clear();
                }
            }
            else
            {
                currentName += static_cast<char>(byte);
            }
        }
        if (!currentName.empty())
        {
            namesList.push_back(currentName);
        }

        for (uint32_t fileIndex = 0; fileIndex < footerBlock.info.numFiles; fileIndex++)
        {
            String fileName = namesList[fileIndex];
            String filePath = String(outputPath.GetAbsolutePathname().c_str()) + String(fileName.c_str());
            FilePath realFilePath = FilePath(filePath);

            auto fileData = filesDatas[fileIndex];

            if (!dvpkFile->Seek(fileData.startPosition, File::SEEK_FROM_START))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to seek to file buffer\n");
                return false;
            }

            Vector<uint8> compressedBuffer;

            Vector<uint8> uncompressedBuffer;
            uncompressedBuffer.resize(fileData.originalSize);

            for (uint32_t byteIndex = 0; byteIndex < fileData.compressedSize; byteIndex++)
            {
                uint8 byteAppend;
                if (sizeof(byteAppend) != dvpkFile->Read(&byteAppend, sizeof(byteAppend)))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to read byte of compressed file buffer\n");
                    return false;
                }

                compressedBuffer.push_back(byteAppend);
            }

            Compressor::Type compressionType = fileData.type;
            switch (compressionType)
            {
                case Compressor::Type::None:
                {
                    uncompressedBuffer = compressedBuffer;
                    break;
                }
                case Compressor::Type::Lz4:
                case Compressor::Type::Lz4HC:
                {
                    if (!LZ4Compressor().Decompress(compressedBuffer, uncompressedBuffer))
                    {
                        printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressed file buffer (LZ4Compressor)\n");
                        return false;
                    }
                    break;
                }
                case Compressor::Type::RFC1951:
                {
                    if (!ZipCompressor().Decompress(compressedBuffer, uncompressedBuffer))
                    {
                        printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressed file buffer (ZipCompressor)\n");
                        return false;
                    }
                    break;
                }
                default:
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to uncompress file buffer, unknown compression type\n");
                    return false;
                }
            }

            if (FileSystem::DIRECTORY_CANT_CREATE == GetEngineContext()->fileSystem->CreateDirectory(realFilePath.GetDirectory(), true))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to create recursive directory for file %s\n", filePath.c_str());
                return false;
            }

            ScopedPtr<File> outputFile(File::Create(realFilePath, File::CREATE | File::WRITE));
            if (!outputFile)
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to write uncompressed file buffer %s\n", filePath.c_str());
                return false;
            }

            for (auto& writeByte : uncompressedBuffer)
            {
                if (sizeof(writeByte) != outputFile->Write(&writeByte, sizeof(writeByte)))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to write byte from uncompressed file buffer %s\n", filePath.c_str());
                    return false;
                }
            }

            printf("[ResourceDLC::ProcessUnpack] Successfully wrote %s\n", filePath.c_str());
        }
    }

    if (isDvpm)
    {
        ScopedPtr<File> dvpmFile(File::Create(inputPath, File::OPEN | File::READ));
        if (!dvpmFile)
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to open input dvpm file: %s\n", sourceImport.c_str());
            return false;
        }

        FilePath dvpdPath(inputPath);
        dvpdPath.ReplaceExtension(".dvpd");
        ScopedPtr<File> dvpdFile(File::Create(dvpdPath, File::OPEN | File::READ));
        if (!dvpdFile)
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to open input dvpd file: %s\n", dvpdPath.GetAbsolutePathname().c_str());
            return false;
        }

        uint64_t dvpmFileSize = dvpmFile->GetSize();

        FooterBlock footerBlock;
        if (dvpmFileSize < sizeof(footerBlock))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Footer Block because file size less then pack footer\n");
            return false;
        }

        if (!dvpmFile->Seek(dvpmFileSize - sizeof(footerBlock), File::SEEK_FROM_START))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Footer Block\n");
            return false;
        }

        if (sizeof(footerBlock) != dvpmFile->Read(&footerBlock, sizeof(footerBlock)))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to read Footer Block\n");
            return false;
        }

        if (footerBlock.info.packArchiveMarker != DVPM_FILE_MARKER)
        {
            printf("[ResourceDLC::ProcessUnpack] FooterBlock has incorrect DVPM marker\n");
            return false;
        }

        printf("[ResourceDLC::ProcessUnpack] Number of files : %d \n", footerBlock.info.numFiles);

        if (!dvpmFile->Seek(dvpmFileSize - (sizeof(footerBlock) + footerBlock.info.filesTableSize), File::SEEK_FROM_START))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to seek to Files Table Block\n");
            return false;
        }

        Vector<FilesTableBlock::FilesData::Data> filesDatas;
        for (uint32_t fileIndex = 0; fileIndex < footerBlock.info.numFiles; fileIndex++)
        {
            FilesTableBlock::FilesData::Data fileData;
            if (sizeof(fileData) != dvpmFile->Read(&fileData, sizeof(fileData)))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to read a files data from table\n");
                return false;
            }

            filesDatas.push_back(fileData);
        }

        Vector<uint8> compressedNames;

        Vector<uint8> uncompressedNames;
        uncompressedNames.resize(footerBlock.info.namesSizeOriginal);

        for (uint32_t byteIndex = 0; byteIndex < footerBlock.info.namesSizeCompressed; byteIndex++)
        {
            uint8 byteAppend;
            if (sizeof(byteAppend) != dvpmFile->Read(&byteAppend, sizeof(byteAppend)))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to read compressedNames\n");
                return false;
            }

            compressedNames.push_back(byteAppend);
        }

        uint32 compressedNamesCRC32 = NULL;
        if (sizeof(compressedNamesCRC32) != dvpmFile->Read(&compressedNamesCRC32, sizeof(compressedNamesCRC32)))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to read compressedNamesCRC32\n");
            return false;
        }

        if (!LZ4HCCompressor().Decompress(compressedNames, uncompressedNames))
        {
            printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressedNames\n");
            return false;
        }

        Vector<String> namesList;
        String currentName;
        for (uint8 byte : uncompressedNames)
        {
            if (byte == 0x00)
            {
                if (!currentName.empty())
                {
                    namesList.push_back(currentName);
                    currentName.clear();
                }
            }
            else
            {
                currentName += static_cast<char>(byte);
            }
        }
        if (!currentName.empty())
        {
            namesList.push_back(currentName);
        }

        for (uint32_t fileIndex = 0; fileIndex < footerBlock.info.numFiles; fileIndex++)
        {
            String fileName = namesList[fileIndex];
            String filePath = String(outputPath.GetAbsolutePathname().c_str()) + String(fileName.c_str());
            FilePath realFilePath = FilePath(filePath);

            auto fileData = filesDatas[fileIndex];

            // TODO: Should be parsed from meta
            // Currently skip because file is empty or DVPD file content is not present in itself
            if (fileData.compressedCrc32 == NULL)
            {
                continue;
            }

            if (!dvpdFile->Seek(fileData.startPosition, File::SEEK_FROM_START))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to seek to file buffer\n");
                return false;
            }

            Vector<uint8> compressedBuffer;

            Vector<uint8> uncompressedBuffer;
            uncompressedBuffer.resize(fileData.originalSize);

            for (uint32_t byteIndex = 0; byteIndex < fileData.compressedSize; byteIndex++)
            {
                uint8 byteAppend;
                if (sizeof(byteAppend) != dvpdFile->Read(&byteAppend, sizeof(byteAppend)))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to read byte of compressed file buffer\n");
                    return false;
                }

                compressedBuffer.push_back(byteAppend);
            }

            Compressor::Type compressionType = fileData.type;
            switch (compressionType)
            {
            case Compressor::Type::None:
            {
                uncompressedBuffer = compressedBuffer;
                break;
            }
            case Compressor::Type::Lz4:
            case Compressor::Type::Lz4HC:
            {
                if (!LZ4Compressor().Decompress(compressedBuffer, uncompressedBuffer))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressed file buffer (LZ4Compressor)\n");
                    return false;
                }
                break;
            }
            case Compressor::Type::RFC1951:
            {
                if (!ZipCompressor().Decompress(compressedBuffer, uncompressedBuffer))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to uncompress compressed file buffer (ZipCompressor)\n");
                    return false;
                }
                break;
            }
            default:
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to uncompress file buffer, unknown compression type\n");
                return false;
            }
            }

            if (FileSystem::DIRECTORY_CANT_CREATE == GetEngineContext()->fileSystem->CreateDirectory(realFilePath.GetDirectory(), true))
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to create recursive directory for file %s\n", filePath.c_str());
                return false;
            }

            ScopedPtr<File> outputFile(File::Create(realFilePath, File::CREATE | File::WRITE));
            if (!outputFile)
            {
                printf("[ResourceDLC::ProcessUnpack] Failed to write uncompressed file buffer %s\n", filePath.c_str());
                return false;
            }

            for (auto& writeByte : uncompressedBuffer)
            {
                if (sizeof(writeByte) != outputFile->Write(&writeByte, sizeof(writeByte)))
                {
                    printf("[ResourceDLC::ProcessUnpack] Failed to write byte from uncompressed file buffer %s\n", filePath.c_str());
                    return false;
                }
            }

            printf("[ResourceDLC::ProcessUnpack] Successfully wrote %s\n", filePath.c_str());
        }
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
