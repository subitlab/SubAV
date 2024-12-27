///
/// \file      OwlVision.hpp
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
#include <iosfwd>

namespace SubIT {

    class SbOwlVisionConstants {
    public:
        static constexpr float quantTablesInv8x8[2][64] = { {
                16.F, 11.F, 10.F, 16.F, 24.F, 40.F, 51.F, 61.F,
                12.F, 12.F, 14.F, 19.F, 26.F, 58.F, 60.F, 55.F,
                14.F, 13.F, 16.F, 24.F, 40.F, 57.F, 69.F, 56.F,
                14.F, 17.F, 22.F, 29.F, 51.F, 87.F, 80.F, 62.F,
                18.F, 22.F, 37.F, 56.F, 68.F, 109.F, 103.F, 77.F,
                24.F, 35.F, 55.F, 64.F, 81.F, 104.F, 113.F, 92.F,
                49.F, 64.F, 78.F, 87.F, 103.F, 121.F, 120.F, 101.F,
                72.F, 92.F, 95.F, 98.F, 112.F, 100.F, 103.F, 99.F
            }, {
                17.F, 18.F, 24.F, 47.F, 99.F, 99.F, 99.F, 99.F,
                18.F, 21.F, 26.F, 66.F, 99.F, 99.F, 99.F, 99.F,
                24.F, 26.F, 56.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                47.F, 66.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F
            }
        };

        static constexpr float quantTablesFwd[2][64] = { {
                0.062500F, 0.090909F, 0.100000F, 0.062500F, 0.041667F, 0.025000F, 0.019608F, 0.016393F,
                0.083333F, 0.083333F, 0.071429F, 0.052632F, 0.038462F, 0.017241F, 0.016667F, 0.018182F,
                0.071429F, 0.076923F, 0.062500F, 0.041667F, 0.025000F, 0.017544F, 0.014493F, 0.017857F,
                0.071429F, 0.058824F, 0.045455F, 0.034483F, 0.019608F, 0.011494F, 0.012500F, 0.016129F,
                0.055556F, 0.045455F, 0.027027F, 0.017857F, 0.014706F, 0.009174F, 0.009709F, 0.012987F,
                0.041667F, 0.028571F, 0.018182F, 0.015625F, 0.012346F, 0.009615F, 0.008850F, 0.010870F,
                0.020408F, 0.015625F, 0.012821F, 0.011494F, 0.009709F, 0.008264F, 0.008333F, 0.009901F,
                0.013889F, 0.010870F, 0.010526F, 0.010204F, 0.008929F, 0.010000F, 0.009709F, 0.010101F
            }, {
                0.058824F, 0.055556F, 0.041667F, 0.021277F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.055556F, 0.047619F, 0.038462F, 0.015152F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.041667F, 0.038462F, 0.017857F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.021277F, 0.015152F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F,
                0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F, 0.010101F
        }
        };
        
    };
    
    //==============================================
    // This class is the core part of SubAV
    //==============================================
    class SbOwlVisionCoreImage {
    public:
        // For method mode selection
        enum PlaneType : uint8_t { Luma = 0, ChromaBlue = 1, ChromaRed = 2 };

        size_t    width;
        size_t    height;
        uint8_t * data;

        // You should manage memory allocation your self, which means constructor won't allocate and destructor won't free memory.
        // However, this class provided a function to help you allocate image quickly.
        SbOwlVisionCoreImage() = default;
        SbOwlVisionCoreImage(size_t w, size_t h);
        SbOwlVisionCoreImage(const SbOwlVisionCoreImage&);
        SbOwlVisionCoreImage(SbOwlVisionCoreImage&&) noexcept;
        SbOwlVisionCoreImage& operator=(const SbOwlVisionCoreImage&) = default;
        SbOwlVisionCoreImage& operator=(SbOwlVisionCoreImage&&)      = default;
        ~SbOwlVisionCoreImage() = default;

        // ============================================================================
        //  This is a restriction that raw image's width and height is divisible by 16.
        // ============================================================================
        bool       SatisfyRestriction() const;
        
        // For copy purpose.
        size_t     PlaneSize(PlaneType p)              const;
        // Locate data pointer to Y plane / Cb plane / Cr plane.
        size_t     PlaneOffset(PlaneType p)            const;
        // off is 0 for width and 1 for height
        size_t     PlaneDelta(PlaneType p, size_t off) const;
        size_t     TotalSize()                         const;

        // Allocate memory to image data
        void       Allocate(void*(*alloc)(size_t size));

        // This is used to do multi thread optimization of this process.
        // however it's not strictly necessary.
        static void TransformAndQuantizeRowTask8x8(const bool dir, const size_t pTabId, float* beg,  const size_t rowSize);
        
        // // (I)DCT + (De)Quantization to a single plane, plane is floating point buffer that would store data.
        void TransformAndQuantizePlane8x8(const bool dir, PlaneType p, float* plane);
    };

    
    //========================================================
    //        SUB AV "OVC" file description
    //========================================================
    //    Bytes     |  Description  |     Value              |
    //==============|===============|=========================
    //    [0,7)     |    Header     |  "SBAV-OVC"            |
    //==============|===============|=========================
    //    [7,15)    |    Width      |  64 bit little endian  |
    //==============|===============|=========================
    //    [15,23)   |    Height     |  64 bit little endian  |
    //==============|===============|=========================
    //    [23,31)   | Bits Encoded  |  64 bit little endian  |
    //==============|===============|=========================
    //    [31,32)   | Table Size(N) |   8 bit                |
    //==============|===============|=========================
    //   [32,32+N)  |  Table Data   |   byte array           |
    //==============|===============|=========================
    //  [32+N,EOF)  | Encoded Bits  |  bit stream big endian |
    //==============|===============|=========================
    //        Class implemented all above.
    //========================================================
    class SbOwlVisionContainer {
    public:
        // Just bind the image you want to operate on this, and you can start reading or writing it.
        SbOwlVisionCoreImage* image;
        // Compressed input and output, results would be stored inside image.
        
        // We assume there are no data inside image.
        float* operator()(std::istream* in, void*(*alloc)(size_t));
        // We assume there already have data inside image.
        float* operator()(std::ostream* out, void*(*alloc)(size_t));
    };

}
