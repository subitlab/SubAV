///
/// \file      DCT.hpp
/// \brief     2D Desecrate Cosine Transform
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include <cstddef>

namespace SubIT {
    
     //===========================
    // Desecrate Cosine Transform
    //===========================
    class SbDCT {
    public:
        static constexpr bool dirForward = true;
        static constexpr bool dirInverse = false;
        
        // Iterate range.
        float     *src;
        // For extension purpose.
        ptrdiff_t  step;

        SbDCT(float* beg, ptrdiff_t s = 1);
        SbDCT(const SbDCT&)            = default;
        SbDCT(SbDCT&&)                 = default;
        SbDCT& operator=(const SbDCT&) = default;
        SbDCT& operator=(SbDCT&&)      = default;

        // We currently only have standard JPEG dct implementation.
        // Also, we assume all your transform is in-placed which means all data will be written to begin.
        template <bool Dir> void Transform8();
        template <bool Dir> void Transform16();
        template <bool Dir> void Transform32();

        // For The Fu audio analysing.
        template <bool Dir>
        void TransformLinear1024();

        // Since quantization is not related to column or row, we assume step is 1 here for performance.
        void Quantize8(const float* const tb);
        void Quantize16(const float* const tb);
        void Quantize32(const float* const tb);
    };

    //==============================
    // Desecrate Cosine Transform 2D
    //==============================
    class SbDCT2 {
    public:

        float     *src;
        ptrdiff_t  step;

        // SbDCT2 can only handle data in a square area.
        SbDCT2(float* beg, ptrdiff_t row_size);
        SbDCT2(const SbDCT2&)              = default;
        SbDCT2(SbDCT2&&)                   = default;
        SbDCT2& operator=(const SbDCT2&)   = default;
        SbDCT2& operator=(SbDCT2&&)        = default;
        ~SbDCT2() = default;

        // According to standard JPEG.
        template <bool Dir> void Transform8x8();
        template <bool Dir> void Transform16x16();
        template <bool Dir> void Transform32x32();
        
        void Quantize8x8(const float* const tb);
        void Quantize16x16(const float* const tb);
        void Quantize32x32(const float* const tb);
    };

}
