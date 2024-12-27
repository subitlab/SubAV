///
/// \file      DCT.cpp
/// \brief     Implementation of DCT
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include "DCT.hpp"

#include <cmath>
#include <numbers>
#include <algorithm>
#include <iostream>
#include <iterator>

#define __SSE__

#ifdef __SSE__
#include <xmmintrin.h>
#endif

namespace SubIT {
    // This is the soul of our DCT implementation
    // If you want SSE acceleration just simply define __SSE__ macro on top of this file.
    template <float cr, float sr>
    static auto rot(const  float x0, const  float y0) {
#ifdef __SSE__
        __m128 v = _mm_mul_ps(_mm_set_ps(cr, sr, -sr, cr), _mm_set_ps(x0, x0, y0, y0));
        v = _mm_add_ps(v, _mm_shuffle_ps(v, v, 0x4E));
        return std::make_pair(v.m128_f32[3], v.m128_f32[2]);
#else
        y1_x1[1] = cr * x0 - sr * y0;
        y1_x1[0] = sr * x0 + cr * y0;
#endif
    }
    constexpr inline static auto pam(const float a, const float b, const float k = 1.F) {
        return std::make_pair(k * (a + b), k * (a - b));
    }
        
    SbDCT::SbDCT(float* beg, ptrdiff_t s) :src(beg), step(s) {}

    void SbDCT::Transform8(const bool dir) {
        // Rotor SIMD DCT.
        // This algorithm contains only 7 multiplication when enable simd, this is the best case
        // and 22 mul without simd enabled, this is the worst case.
        // Compared to original O(n^2) algorithm this saved (64 - 22) == 42 multiplications at least,
        // and saves (64 - 7) == 57 multiplications.
        // Total time complexity is O(24 == 8 * log_2{8}), however with too much optimization
        // Its time cost can even reach to O(1).
        
        // All times sqrt(2 / 8) which is 0.5
        constexpr float a = 0.3535533905F;
        constexpr float b = 0.4903926402F;
        constexpr float c = 0.4157348061F;
        constexpr float d = 0.4619397662F;
        constexpr float B = 0.0975451610F;
        constexpr float C = 0.2777851165F;
        constexpr float D = 0.1913417161F;

        switch (dir) {
        case dirForward: {
            // Stage one values
            const auto[s0, s1] = pam(src[0 * step], src[7 * step]);
            const auto[s2, s3] = pam(src[1 * step], src[6 * step]);
            const auto[s4, s5] = pam(src[2 * step], src[5 * step]);
            const auto[s6, s7] = pam(src[3 * step], src[4 * step]);

            // Two fastest terms.
            src[0 * step] = a * (s0 + s6 + s2 + s4); // 1 mul
            src[4 * step] = a * (s0 + s6 - s2 - s4); // 1 mul

            const auto[r0_0, r0_1] = rot<b, B>(s7, s1); // 1 (SSE) / 4 mul 
            const auto[r1_0, r1_1] = rot<c, C>(s5, s3); // 1 (SSE) / 4 mul 
            src[1 * step] = r0_1 + r1_1;
            src[7 * step] = r1_0 - r0_0;
        
            const auto[g0, g1] =  rot<d, D>(s2 - s4, s0 - s6); // 1 (SSE) / 4 mul
            src[2 * step] =  g1;
            src[6 * step] = -g0;

            const auto[t0_0, t0_1] = rot<c, C>(s1, s7); // 1 (SSE) / 4 mul
            const auto[t1_0, t1_1] = rot<b, B>(s3, s5); // 1 (SSE) / 4 mul
            src[3 * step] = t0_0 - t1_1;
            src[5 * step] = t0_1 - t1_0;
            return;
        }
        case dirInverse: {
            // First stage
            const auto [s0, s1] = pam(src[0 * step], src[4 * step], a);
            const auto [s2, s3] = rot<D, d>(src[2 * step], src[6 * step]);

            // Second stage
            const auto [g0, g1] = rot<B, b>(src[1 * step], src[7 * step]);
            const auto [g2, g3] = rot<b, B>(src[3 * step], src[5 * step]);
            const auto [g4, g5] = rot<c, C>(src[1 * step], src[7 * step]);
            const auto [g6, g7] = rot<C, c>(src[3 * step], src[5 * step]);

            // Third stage 0
            const float t0 = g1 + g7;
            const float t1 = g3 - g4;
            const float t2 = g2 - g5;
            const float t3 = g0 - g6;

            // Third stage 1.
            const auto [k0, k1] = pam(s0, s3);
            const auto [k2, k3] = pam(s1, s2);

            // Final stage FFT emplace.
            src[0 * step] = k0 + t0;
            src[7 * step] = k0 - t0;
            src[1 * step] = k2 - t1;
            src[6 * step] = k2 + t1;
            src[2 * step] = k3 - t2;
            src[5 * step] = k3 + t2;
            src[3 * step] = k1 + t3;
            src[4 * step] = k1 - t3;
            return;
        }
        }
    }

    void SbDCT::Quantize8(const float* const tb) {
        src[0] *= tb[0];
        src[1] *= tb[1];
        src[2] *= tb[2];
        src[3] *= tb[3];
        src[4] *= tb[4];
        src[5] *= tb[5];
        src[6] *= tb[6];
        src[7] *= tb[7];
    }

    SbDCT2::SbDCT2(float* beg, ptrdiff_t row_size) : src(beg), step(row_size) {}

    void SbDCT2::Transform8x8(const bool dir) {
        SbDCT row0(src + 0 * step, 1); row0.Transform8(dir); 
        SbDCT row1(src + 1 * step, 1); row1.Transform8(dir);
        SbDCT row2(src + 2 * step, 1); row2.Transform8(dir);
        SbDCT row3(src + 3 * step, 1); row3.Transform8(dir);
        SbDCT row4(src + 4 * step, 1); row4.Transform8(dir);
        SbDCT row5(src + 5 * step, 1); row5.Transform8(dir);
        SbDCT row6(src + 6 * step, 1); row6.Transform8(dir);
        SbDCT row7(src + 7 * step, 1); row7.Transform8(dir);

        SbDCT col0(src + 0, step); col0.Transform8(dir);
        SbDCT col1(src + 1, step); col1.Transform8(dir);
        SbDCT col2(src + 2, step); col2.Transform8(dir);
        SbDCT col3(src + 3, step); col3.Transform8(dir);
        SbDCT col4(src + 4, step); col4.Transform8(dir);
        SbDCT col5(src + 5, step); col5.Transform8(dir);
        SbDCT col6(src + 6, step); col6.Transform8(dir);
        SbDCT col7(src + 7, step); col7.Transform8(dir);
    }

    void SbDCT2::Quantize8x8(const float* const tb) {
        SbDCT row0(src + 0 * step); row0.Quantize8(tb + (0 << 3));
        SbDCT row1(src + 1 * step); row1.Quantize8(tb + (1 << 3));
        SbDCT row2(src + 2 * step); row2.Quantize8(tb + (2 << 3));
        SbDCT row3(src + 3 * step); row3.Quantize8(tb + (3 << 3));
        SbDCT row4(src + 4 * step); row4.Quantize8(tb + (4 << 3));
        SbDCT row5(src + 5 * step); row5.Quantize8(tb + (5 << 3));
        SbDCT row6(src + 6 * step); row6.Quantize8(tb + (6 << 3));
        SbDCT row7(src + 7 * step); row7.Quantize8(tb + (7 << 3));
    }
}
