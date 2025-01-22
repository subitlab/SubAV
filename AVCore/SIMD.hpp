///
/// \file      SIMD.hpp
/// \brief     All simd related functionalities are implemented in this file (New year's first feature!).
/// \details   SSE/NEON or sth are all here these methods are reusable, not only for this SubAV project.
/// \author    HenryDu, Steve Wang
/// \date      1.22.2025
/// \copyright © HenryDu 2024, Steve Wang 2025
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
#include <mmintrin.h>
#include <pmmintrin.h>
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
        
        static inline void yuv2rgba(const float y, const float u, const float v, unsigned char *dest) {
#if SB_SIMD_X86 >= SB_SIMD_X86_SSE1
            __m128  t = _mm_set_ps(y, u, v, 255.F);
            __m128  r1 = _mm_mul_ps(t, _mm_set_ps(1.F,       0.F, 1.13983F,   0.F));
            __m128  r2 = _mm_mul_ps(t, _mm_set_ps(1.F, -0.39645F, -0.5806F,   0.F));
                    r1 = _mm_hadd_ps(r1, r2);
            __m128  r3 = _mm_mul_ps(t, _mm_set_ps(1.F,  2.03211F,      0.F,   0.F));
                    r2 = _mm_mul_ps(t, _mm_set_ps(0.F,       0.F,      0.F,   1.F));
                    r3 = _mm_hadd_ps(r3, r2);
                    r1 = _mm_hadd_ps(r1, r3);   //result.
            __m64   rs = _mm_cvtps_pi16(r1);    //result, signed int16.
                                                //it's annoying that SSE has no instruction to convert its result directly into uint8, and we therefore need such a dirty workaround.
            const unsigned long long res = reinterpret_cast<const unsigned long long>(rs);
            *dest = *(unsigned char *)(&res);
            dest[1] = *(unsigned char *)((&res)+2);
            dest[2] = *(unsigned char *)((&res)+4);
            dest[3] = *(unsigned char *)((&res)+6);
#else
            *dest = (unsigned char)(y+v*1.13983F);
            dest[1] = (unsigned char)(y+u*-0.39645F+v*-0.5806F);
            dest[2] = (unsigned char)(y+u*2.03211F);
            dest[3] = 0xff;
#endif
        }
    };
    
}
