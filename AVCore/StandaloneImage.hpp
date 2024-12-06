///
/// \file      StandaloneImage.hpp
/// \brief     SubIT standalone image type.
/// \details   I mean, since JPEG uses YUV420p then why we still convert it to RGB on CPU side.
///            This texture format is a solution to that problem.
/// \author    HenryDu
/// \date      10.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <cstdint>
#include <cstddef>
#include <memory_resource>
#include <iosfwd>

namespace SubIT {
    
    //==============================================
    // This class is the core part of SubAV
    //==============================================
    class SbStandaloneImage {
    public:
        // For method mode selection
        enum PlaneType : uint8_t { Luma = 0, ChromaBlue = 1, ChromaRed = 2 };

        size_t    width;
        size_t    height;
        uint8_t * data;
        // For custom allocation.
        std::pmr::polymorphic_allocator<uint8_t> allocator;

        // You should manage this memory yourself.
        SbStandaloneImage(std::pmr::polymorphic_allocator<uint8_t> allo = {});
        SbStandaloneImage(size_t w, size_t h, std::pmr::polymorphic_allocator<uint8_t> allo = {});
        SbStandaloneImage(const SbStandaloneImage&);
        SbStandaloneImage(SbStandaloneImage&&) noexcept;
        SbStandaloneImage& operator=(const SbStandaloneImage&) = default;
        SbStandaloneImage& operator=(SbStandaloneImage&&)      = default;
        ~SbStandaloneImage();

        size_t     PaddedValue(size_t n, PlaneType p)   const;
        // For copy purpose.
        size_t     PlaneSize(PlaneType p)              const;
        // Locate data pointer to Y plane / Cb plane / Cr plane.
        uint8_t*   PlaneAt(PlaneType p)                const;
        // off is 0 for width and 1 for height
        size_t     PlaneDelta(PlaneType p, size_t off) const;
        size_t     TotalSize()                         const;
        
        void       Allocate();
        
        // Below algorithms assume you already have memory buffer allocated.
        // Otherwise, we don't know what would happen.

        //==========================================================================
        // Currently we acquire all images' width and height can is divisible by 16!
        //==========================================================================

        // Forward compression -- JPEG DCT + JPEG Quantization + Huffman coding (Without Zig-Zag RLE).
        void LossyShrinkBlock8x8(float* beg, size_t rowSize, PlaneType p);
        void LossyShrinkPlane(PlaneType p);

        // Backward decompression -- Huffman decoding + JPEG Inverse Quantization + JPEG Inverse DCT.
        void LossyExpandBlock8x8(float* beg, size_t rowSize, PlaneType p);
        void LossyExpandPlane(PlaneType p);
    };

    //===============================================
    //        SBSI yuv texture description
    //===============================================
    //    Bytes     |  Description  |     Value     |
    //==============|===============|================
    //    [0,7)     |    Header     |   SB-AV-SI   |
    //==============|===============|================
    //    [7,15)    |    Width      |  64 bit uint  |
    //==============|===============|================
    //    [15,23)   |    Height     |  64 bit uint  |
    //==============|===============|================
    //    [23,31)   | Bits Encoded  |  64 bit uint  |
    //==============|===============|================
    //    [31,32)   | Tree Size (N) |   8 bit uint  |
    //==============|===============|================
    //   [32,32+N)  |   Tree Nodes  |   byte array  |
    //==============|===============|================
    //  [32+N,EOF)  | Encoded Bits  |   bit stream  |
    //==============|===============|================
    //        Class implemented all above.
    //===============================================
    class SbSIFactory {
    public:
        // Just bind the image you want to operate on this, and you can start reading or writing it.
        SbStandaloneImage* image;
        // Compressed input and output, results would be stored inside image.
        
        // We assume there are no data inside image.
        void operator()(std::istream* in);
        // We assume there already have data inside image.
        void operator()(std::ostream* out);
    };

}