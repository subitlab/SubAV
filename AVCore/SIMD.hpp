///
/// \file      SIMD.hpp
/// \brief     All simd related functionalities are implemented in this file.
/// \details   SSE/NEON or sth are all here these methods are reusable, not only for this SubAV project.
/// \author    HenryDu
/// \date      10.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include <utility>
#include <cstring>

#define SB_SIMD_X86_SSE1   1
#define SB_SIMD_X86_SSE2   2
#define SB_SIMD_X86_SSE3   3
#define SB_SIMD_X86_SSE4   4
#define SB_SIMD_X86_SSE4_1 5

#define SB_SIMD_ARM_NEON   1

// If you are using an x86 platform, set SB_SIMD_ARM to 0.
// If you are using an arm platform, set SB_SIMD_X86 to 0.
#define SB_SIMD_X86 5
#define SB_SIMD_ARM 0

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
#include <xmmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE2
#include <emmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE4_1
#include <smmintrin.h>
#endif

namespace SubIT {
    class SbSIMD {
    public:
        static inline auto Rotate2D(const float cr, const float sr, const float x0, const float y0) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            __m128 v = _mm_mul_ps(_mm_set_ps(cr, sr, -sr, cr), _mm_set_ps(x0, x0, y0, y0));
            v = _mm_add_ps(v, _mm_shuffle_ps(v, v, 0x4E));
            // Avoid _mm_store_ps usage (That costs addition temporary storage and can be slow)!
            const float* h = reinterpret_cast<const float*>(&v);
            return std::make_pair(h[3], h[2]);
#else
            return std::make_pair(cr * x0 - sr * y0, sr * x0 + cr * y0);
#endif
        }
        // This acceleration requires SSE 4.1
        static inline void FloatToInt4(float* f) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE4_1
            __m128i i = _mm_cvtps_epi32 (_mm_round_ps(*reinterpret_cast<__m128*>(f), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
            std::memcpy(f, reinterpret_cast<int*>(&i), 16);
#else
            reinterpret_cast<int*>(f)[0] = static_cast<int>(std::roundf(f[0]));
            reinterpret_cast<int*>(f)[1] = static_cast<int>(std::roundf(f[1]));
            reinterpret_cast<int*>(f)[2] = static_cast<int>(std::roundf(f[2]));
            reinterpret_cast<int*>(f)[3] = static_cast<int>(std::roundf(f[3]));
#endif
        }
        
        
    };
    
}