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

        float& At(ptrdiff_t i);
        
        template <bool Dir> void Transform4();
        template <bool Dir> void Transform8();
        template <bool Dir> void Transform16();
        template <bool Dir> void Transform32(); // Wait for a new quantization table.
        

        // Since quantization is not related to column or row, we assume step is 1 here for performance.
        template <bool Dir> void Quantize4(const float* const tb);
        template <bool Dir> void Quantize8(const float* const tb);
        template <bool Dir> void Quantize16(const float* const tb);
        template <bool Dir> void Quantize32(const float* const tb);
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
        template <bool Dir> void Transform4x4();
        template <bool Dir> void Transform8x8();
        template <bool Dir> void Transform16x16();
        template <bool Dir> void Transform32x32();

        template <bool Dir> void Quantize4x4(const float* const tb);
        template <bool Dir> void Quantize8x8(const float* const tb);
        template <bool Dir> void Quantize16x16(const float* const tb);
        template <bool Dir> void Quantize32x32(const float* const tb);
    };

}
