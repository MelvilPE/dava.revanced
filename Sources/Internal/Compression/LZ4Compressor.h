#pragma once

#include "Compression/Compressor.h"

namespace DAVA
{
class LZ4Compressor : public Compressor
{
public:
    bool Compress(const Vector<uint8>& in, Vector<uint8>& out) const override;
    // you should resize output to correct size before call this method
    bool Decompress(const Vector<uint8>& in, Vector<uint8>& out) const override;
};

class LZ4HCCompressor : public LZ4Compressor
{
public:
    bool Compress(const Vector<uint8>& in, Vector<uint8>& out) const override;
};

class LZ4StreamCompressor : public LZ4HCCompressor
{
public:
    struct Block
    {
        uint64 seekPosInDvpl = 0;
        uint32 compressedCrc32 = 0;
        uint32 compressedSize = 0;
        uint32 uncompressedCrc32 = 0;
        uint32 uncompressedSize = 0;
    };

    struct LZ4StreamHeader
    {
        uint32 numOfParts = 0;
        Vector<Block> parts;
    };

    static bool LZ4StreamCompress(const Vector<uint8>& in, Vector<uint8>& out);
    static bool LZ4StreamDecompress(const Vector<uint8>& in, Vector<uint8>& out);
};

} // end namespace DAVA
