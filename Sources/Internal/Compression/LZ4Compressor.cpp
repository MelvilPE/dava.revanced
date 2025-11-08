#include "Compression/LZ4Compressor.h"
#include "Logger/Logger.h"

#include <lz4/lz4.h>
#include <lz4/lz4hc.h>

namespace DAVA
{
Compressor::~Compressor() = default; // only one virtual table(fix warning)

bool LZ4Compressor::Compress(const Vector<uint8>& in, Vector<uint8>& out) const
{
    if (in.size() > LZ4_MAX_INPUT_SIZE)
    {
        Logger::Error("LZ4 compress failed too big input buffer");
        return false;
    }
    uint32 maxSize = static_cast<uint32>(LZ4_compressBound(static_cast<uint32>(in.size())));
    if (out.size() < maxSize)
    {
        out.resize(maxSize);
    }
    int32 compressedSize = LZ4_compress(reinterpret_cast<const char*>(in.data()), reinterpret_cast<char*>(out.data()), static_cast<uint32>(in.size()));
    if (compressedSize == 0)
    {
        return false;
    }
    out.resize(static_cast<uint32>(compressedSize));
    return true;
}

bool LZ4Compressor::Decompress(const Vector<uint8>& in, Vector<uint8>& out) const
{
    int32 decompressResult = LZ4_decompress_fast(reinterpret_cast<const char*>(in.data()), reinterpret_cast<char*>(out.data()), static_cast<uint32>(out.size()));
    if (decompressResult < 0)
    {
        Logger::Error("LZ4 decompress failed");
        return false;
    }
    return true;
}

bool LZ4HCCompressor::Compress(const Vector<uint8>& in, Vector<uint8>& out) const
{
    if (in.size() > LZ4_MAX_INPUT_SIZE)
    {
        Logger::Error("LZ4 compress failed too big input buffer");
        return false;
    }
    if (in.empty())
    {
        Logger::Error("LZ4 can't compress empty buffer");
        return false;
    }
    uint32 maxSize = static_cast<uint32>(LZ4_compressBound(static_cast<uint32>(in.size())));
    if (out.size() < maxSize)
    {
        out.resize(maxSize);
    }
    int32 compressedSize = LZ4_compressHC(reinterpret_cast<const char*>(in.data()), reinterpret_cast<char*>(out.data()), static_cast<uint32>(in.size()));
    if (compressedSize == 0)
    {
        return false;
    }
    out.resize(static_cast<uint32>(compressedSize));
    return true;
}

bool LZ4StreamCompressor::LZ4StreamCompress(const Vector<uint8>& in, Vector<uint8>& out)
{
    if (in.empty())
    {
        Logger::Error("LZ4 can't compress empty buffer");
        return false;
    }

    const size_t inSize = in.size();
    const size_t blockSize = 1 << 20;

    uint32_t numParts = static_cast<uint32_t>((inSize + blockSize - 1) / blockSize);

    LZ4StreamHeader header;
    header.parts.clear();
    header.parts.resize(numParts);
    header.numOfParts = numParts;

    Vector<Vector<uint8>> blocksData;
    blocksData.reserve(numParts);

    const uint64_t headerSize = 4u + static_cast<uint64_t>(numParts) * static_cast<uint64_t>(sizeof(Block));

    uint64_t nextBlockOffset = headerSize;
    size_t readPos = 0;

    for (uint32_t partIndex = 0; partIndex < numParts; ++partIndex)
    {
        const size_t remaining = inSize - readPos;
        const size_t curUncompSize = std::min(remaining, blockSize);

        if (curUncompSize > 0x7E000000u)
        {
            Logger::Error("LZ4 compress failed too big input buffer");
            return false;
        }

        const uint8* srcPtr = in.data() + readPos;

        Vector<uint8> compBuf;
        compBuf.resize(static_cast<size_t>(LZ4_compressBound(static_cast<int>(curUncompSize))));

        Vector<uint8> inBlock;
        inBlock.resize(curUncompSize);
        std::memcpy(inBlock.data(), srcPtr, curUncompSize);

        bool ok = LZ4HCCompressor().Compress(inBlock, compBuf);
        if (!ok)
        {
            Logger::Error("LZ4 compression failed for a block (HC)");
            return false;
        }

        Block blk;
        blk.seekPosInDvpl = nextBlockOffset;
        blk.compressedSize = static_cast<uint32_t>(compBuf.size());
        blk.uncompressedSize = static_cast<uint32_t>(curUncompSize);
        blk.compressedCrc32 = CRC32::ForBuffer(compBuf.data(), compBuf.size());
        blk.uncompressedCrc32 = CRC32::ForBuffer(inBlock.data(), inBlock.size());

        header.parts[partIndex] = blk;

        blocksData.emplace_back(std::move(compBuf));

        nextBlockOffset += blk.compressedSize;
        readPos += curUncompSize;
    }

    const uint64_t totalCompressedDataSize = nextBlockOffset - headerSize;
    const uint64_t totalOutSize64 = headerSize + totalCompressedDataSize;
    if (totalOutSize64 > std::numeric_limits<size_t>::max())
    {
        Logger::Error("Resulting output size too large");
        return false;
    }
    size_t totalOutSize = static_cast<size_t>(totalOutSize64);

    out.clear();
    out.resize(totalOutSize);

    std::memcpy(out.data(), &header.numOfParts, sizeof(header.numOfParts));

    uint8* writePtr = out.data() + 4;
    for (uint32_t i = 0; i < numParts; ++i)
    {
        std::memcpy(writePtr, &header.parts[i], sizeof(Block));
        writePtr += sizeof(Block);
    }

    for (const auto& comp : blocksData)
    {
        std::memcpy(writePtr, comp.data(), comp.size());
        writePtr += comp.size();
    }

    if (static_cast<size_t>(writePtr - out.data()) != totalOutSize)
    {
        Logger::Error("Internal error: output size mismatch after writing blocks");
        return false;
    }

    return true;
}

bool LZ4StreamCompressor::LZ4StreamDecompress(const Vector<uint8>& in, Vector<uint8>& out)
{
    if (in.empty())
    {
        Logger::Error("LZ4StreamCompressor::Decompress: input is empty");
        return false;
    }

    if (in.size() < sizeof(uint32_t))
    {
        Logger::Error("LZ4StreamCompressor::Decompress: too small input for header");
        return false;
    }

    uint32_t numParts = 0;
    std::memcpy(&numParts, in.data(), sizeof(numParts));

    const size_t partsTableSize = static_cast<size_t>(numParts) * sizeof(Block);
    const size_t headerSize = sizeof(uint32_t) + partsTableSize;
    if (in.size() < headerSize)
    {
        Logger::Error("LZ4StreamCompressor::Decompress: input too small for parts table (numParts = %u)", numParts);
        return false;
    }

    LZ4StreamHeader localHeader;
    localHeader.numOfParts = numParts;
    localHeader.parts.resize(numParts);
    std::memcpy(localHeader.parts.data(), in.data() + sizeof(uint32_t), partsTableSize);

    uint64_t totalUncompressed = 0;
    for (uint32_t i = 0; i < numParts; ++i)
    {
        uint32_t uSize = localHeader.parts[i].uncompressedSize;
        totalUncompressed += uSize;
        if (totalUncompressed > std::numeric_limits<size_t>::max())
        {
            Logger::Error("LZ4StreamCompressor::Decompress: total uncompressed size overflow");
            return false;
        }
    }

    out.clear();
    out.resize(static_cast<size_t>(totalUncompressed));

    size_t writeOffset = 0;
    for (uint32_t i = 0; i < numParts; ++i)
    {
        const Block& blk = localHeader.parts[i];

        uint64_t seek = blk.seekPosInDvpl;
        uint64_t compSize = blk.compressedSize;
        uint64_t uncompSize = blk.uncompressedSize;

        if (seek + compSize > in.size())
        {
            Logger::Error("LZ4StreamCompressor::Decompress: compressed block %u out of bounds (seek=%llu, compSize=%llu, inSize=%zu)",
                          i, static_cast<unsigned long long>(seek), static_cast<unsigned long long>(compSize), in.size());
            return false;
        }
        if (writeOffset + uncompSize > out.size())
        {
            Logger::Error("LZ4StreamCompressor::Decompress: output overflow when writing block %u", i);
            return false;
        }

        Vector<uint8> tmpCompressedBlock;
        tmpCompressedBlock.resize(static_cast<size_t>(compSize));
        std::memcpy(tmpCompressedBlock.data(), in.data() + static_cast<size_t>(seek), static_cast<size_t>(compSize));

        if (blk.compressedCrc32 != 0)
        {
            uint32_t crcComp = CRC32::ForBuffer(tmpCompressedBlock.data(), tmpCompressedBlock.size());
            if (crcComp != blk.compressedCrc32)
            {
                Logger::Error("LZ4StreamCompressor::Decompress: compressed CRC mismatch on block %u (got 0x%08X expected 0x%08X)",
                              i, crcComp, blk.compressedCrc32);
                return false;
            }
        }

        Vector<uint8> blockOut;
        blockOut.resize(static_cast<size_t>(uncompSize));

        bool ok = LZ4Compressor().Decompress(tmpCompressedBlock, blockOut);
        if (!ok)
        {
            Logger::Error("LZ4StreamCompressor::Decompress: lz4 decompression failed for block %u", i);
            return false;
        }

        if (blk.uncompressedCrc32 != 0)
        {
            uint32_t crcUncomp = CRC32::ForBuffer(blockOut.data(), blockOut.size());
            if (crcUncomp != blk.uncompressedCrc32)
            {
                Logger::Error("LZ4StreamCompressor::Decompress: uncompressed CRC mismatch on block %u (got 0x%08X expected 0x%08X)",
                              i, crcUncomp, blk.uncompressedCrc32);
                return false;
            }
        }

        std::memcpy(out.data() + writeOffset, blockOut.data(), blockOut.size());
        writeOffset += blockOut.size();
    }

    if (writeOffset != out.size())
    {
        Logger::Error("LZ4StreamCompressor::Decompress: final output size mismatch (written=%zu expected=%zu)", writeOffset, out.size());
        return false;
    }

    return true;
}

} // end namespace DAVA
