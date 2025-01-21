///
/// \file      SIMD.hpp
/// \brief     All simd related functionalities are implemented in this file.
/// \details   SSE/NEON or sth are all here these methods are reusable, not only for this SubAV project.
/// \author    HenryDu
/// \date      10.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include <cstring>
#include <utility>

#define SB_SIMD_X86_SSE1     1
#define SB_SIMD_X86_SSE2     2
#define SB_SIMD_X86_SSE3     3
#define SB_SIMD_X86_SSSE3    4
#define SB_SIMD_X86_SSE4_1   5
#define SB_SIMD_X86_SSE4_2   6
#define SB_SIMD_X86_AVX      7
#define SB_SIMD_X86_AVX512   8

#define SB_SIMD_ARM_NEON   1

// If you are using an x86 platform, set SB_SIMD_ARM to 0.
// If you are using an arm platform, set SB_SIMD_X86 to 0.
#define SB_SIMD_X86 SB_SIMD_X86_SSE2
#define SB_SIMD_ARM 0

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
#include <xmmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE2
#include <emmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE3
#include <pmmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSSE3
#include <tmmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE4_1
#include <smmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_SSE4_2
#include <nmmintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_AVX
#include <immintrin.h>
#endif

#if SB_SIMD_X86 >= SB_SIMD_X86_AVX512
#include <zmmintrin.h>
#endif

namespace SubIT {
    class SbSIMD {
    public:
        static inline void AddA4(float* a, const float* b) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            *reinterpret_cast<__m128*>(a) = _mm_add_ps(*reinterpret_cast<__m128*>(a), *reinterpret_cast<const __m128*>(b));
#else
            a[0] += b[0]; a[1] += b[1]; a[2] += b[2]; a[3] += b[3];
#endif
        }
        static inline void SubA4(float* a, const float* b) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            *reinterpret_cast<__m128*>(a) = _mm_sub_ps(*reinterpret_cast<__m128*>(a), *reinterpret_cast<const __m128*>(b));
#else
            a[0] -= b[0]; a[1] -= b[1]; a[2] -= b[2]; a[3] -= b[3];
#endif
        }
        static inline void MulA4(float* a, const float* b) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            *reinterpret_cast<__m128*>(a) = _mm_mul_ps(*reinterpret_cast<__m128*>(a), *reinterpret_cast<const __m128*>(b));
#else
            a[0] *= b[0]; a[1] *= b[1]; a[2] *= b[2]; a[3] *= b[3];
#endif
        }
        static inline void DivA4(float* a, const float* b) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            *reinterpret_cast<__m128*>(a) = _mm_div_ps(*reinterpret_cast<__m128*>(a), *reinterpret_cast<const __m128*>(b));
#else
            a[0] /= b[0]; a[1] /= b[1]; a[2] /= b[2]; a[3] /= b[3];
#endif
        }
        static inline auto Rotate2D(const float cr, const float sr, const float x0, const float y0) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            __m128 v = _mm_mul_ps(_mm_set_ps(cr, sr, -sr, cr), _mm_set_ps(x0, x0, y0, y0));
            v = _mm_add_ps(v, _mm_shuffle_ps(v, v, 0x4E));
            return std::make_pair(reinterpret_cast<const float*>(&v)[3], reinterpret_cast<const float*>(&v)[2]);
#else
            return std::make_pair(cr * x0 - sr * y0, sr * x0 + cr * y0);
#endif
        }
        // This acceleration requires SSE 4.1
        static inline void F2I4(float* f) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE2
            __m128i i = _mm_cvtps_epi32 (*reinterpret_cast<__m128*>(f)); // With rounding.
            std::memcpy(f, reinterpret_cast<int*>(&i), 16);
#else
            reinterpret_cast<int*>(f)[0] = static_cast<int>(f[0]);
            reinterpret_cast<int*>(f)[1] = static_cast<int>(f[1]);
            reinterpret_cast<int*>(f)[2] = static_cast<int>(f[2]);
            reinterpret_cast<int*>(f)[3] = static_cast<int>(f[3]);
#endif
        }
    };
    
}