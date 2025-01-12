///
/// \file      DCT.cpp
/// \brief     Implementation of DCT
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include "DCT.hpp"
#include "SIMD.hpp"
#include "compile_time.hpp"
#include "base2_lee_dct_impl.hpp"

namespace SubIT {
    
    constexpr inline static auto pam(const float a, const float b, const float k = 1.F) {
        return std::make_pair(k * (a + b), k * (a - b));
    }
        
    SbDCT::SbDCT(float* beg, ptrdiff_t s) :src(beg), step(s) {}

    template void SbDCT::Transform8<SbDCT::dirForward>();
    template void SbDCT::Transform8<SbDCT::dirInverse>();
    template void SbDCT::Transform16<SbDCT::dirForward>();
    template void SbDCT::Transform16<SbDCT::dirInverse>();
    template void SbDCT::Transform32<SbDCT::dirForward>();
    template void SbDCT::Transform32<SbDCT::dirInverse>();

    // 8 dimension has its special implementation for commemorative significance
    // This special implementation is also a lot faster than lee dct in most cases.
    template <bool Dir>
    void SbDCT::Transform8() {
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
        
        if constexpr (Dir == dirForward) {
            // Stage one values
            const auto[s0, s1] = pam(src[0 * step], src[7 * step]);
            const auto[s2, s3] = pam(src[1 * step], src[6 * step]);
            const auto[s4, s5] = pam(src[2 * step], src[5 * step]);
            const auto[s6, s7] = pam(src[3 * step], src[4 * step]);
        
            // Two fastest terms.
            src[0 * step] = a * (s0 + s6 + s2 + s4); // 1 mul
            src[4 * step] = a * (s0 + s6 - s2 - s4); // 1 mul
        
            const auto[r0_0, r0_1] = SbSIMD::Rotate2D(b, B, s7, s1); // 1 (SSE) / 4 mul 
            const auto[r1_0, r1_1] = SbSIMD::Rotate2D(c, C, s5, s3); // 1 (SSE) / 4 mul 
            src[1 * step] = r0_1 + r1_1;
            src[7 * step] = r1_0 - r0_0;
        
            const auto[g0, g1] =  SbSIMD::Rotate2D(d, D, s2 - s4, s0 - s6) ; // 1 (SSE) / 4 mul
            src[2 * step] =  g1;
            src[6 * step] = -g0;
        
            const auto[t0_0, t0_1] = SbSIMD::Rotate2D(c, C, s1, s7); // 1 (SSE) / 4 mul
            const auto[t1_0, t1_1] = SbSIMD::Rotate2D(b, B, s3, s5); // 1 (SSE) / 4 mul
            src[3 * step] = t0_0 - t1_1;
            src[5 * step] = t0_1 - t1_0;
        }
        else if constexpr (Dir == dirInverse) {
            // First stage
            const auto [s0, s1] = pam(src[0 * step], src[4 * step], a);
            const auto [s2, s3] = SbSIMD::Rotate2D(D, d, src[2 * step], src[6 * step]);
        
            // Second stage
            const auto [g0, g1] = SbSIMD::Rotate2D(B, b, src[1 * step], src[7 * step]);
            const auto [g2, g3] = SbSIMD::Rotate2D(b, B, src[3 * step], src[5 * step]);
            const auto [g4, g5] = SbSIMD::Rotate2D(c, C, src[1 * step], src[7 * step]);
            const auto [g6, g7] = SbSIMD::Rotate2D(C, c, src[3 * step], src[5 * step]);
        
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
        }
    }

    template <bool Dir>
    void SbDCT::Transform16() {
        if constexpr (Dir == dirForward) {
            base2_lee_dct_impl::forward_transform_with_step<16, float>(src, step);  
        } else {
            base2_lee_dct_impl::inverse_transform_with_step<16, float>(src, step);
        }
    }

    template <bool Dir>
    void SbDCT::Transform32() {
        if constexpr (Dir == dirForward) {
            base2_lee_dct_impl::forward_transform_with_step<32, float>(src, step);
        } else {
            base2_lee_dct_impl::inverse_transform_with_step<32, float>(src, step);
        }
    }

    template <bool Dir>
    void SbDCT::TransformLinear1024() {
        // This uses no-step versions because audio sequences are linear (1D).
        if constexpr (Dir == dirForward) {
            base2_lee_dct_impl::forward_transform<1024, float>(src);  
        } else {
            base2_lee_dct_impl::inverse_transform<1024, float>(src);
        }
    }

    void SbDCT::Quantize8 (const float* const tb) {
        src[0] *= tb[0];
        src[1] *= tb[1];
        src[2] *= tb[2];
        src[3] *= tb[3];
        src[4] *= tb[4];
        src[5] *= tb[5];
        src[6] *= tb[6];
        src[7] *= tb[7];
    }
    void SbDCT::Quantize16(const float* const tb) {
        src[0] *= tb[0]; src[8] *= tb[8];
        src[1] *= tb[1]; src[9] *= tb[9];
        src[2] *= tb[2]; src[10] *= tb[10];
        src[3] *= tb[3]; src[11] *= tb[11];
        src[4] *= tb[4]; src[12] *= tb[12];
        src[5] *= tb[5]; src[13] *= tb[13];
        src[6] *= tb[6]; src[14] *= tb[14];
        src[7] *= tb[7]; src[15] *= tb[15];
    }

    void SbDCT::Quantize32(const float* const tb) {
        src[0] *= tb[0]; src[8] *= tb[8];   src[16] *= tb[16];  src[24] *= tb[24];
        src[1] *= tb[1]; src[9] *= tb[9];   src[17] *= tb[17];  src[25] *= tb[25];
        src[2] *= tb[2]; src[10] *= tb[10]; src[18] *= tb[18];  src[26] *= tb[26];
        src[3] *= tb[3]; src[11] *= tb[11]; src[19] *= tb[19];  src[27] *= tb[27];
        src[4] *= tb[4]; src[12] *= tb[12]; src[20] *= tb[20];  src[28] *= tb[28];
        src[5] *= tb[5]; src[13] *= tb[13]; src[21] *= tb[21];  src[29] *= tb[29];
        src[6] *= tb[6]; src[14] *= tb[14]; src[22] *= tb[22];  src[30] *= tb[30];
        src[7] *= tb[7]; src[15] *= tb[15]; src[23] *= tb[23];  src[31] *= tb[31];
    }

    SbDCT2::SbDCT2(float* beg, ptrdiff_t row_size) : src(beg), step(row_size) {}

    template void SbDCT2::Transform8x8<SbDCT::dirForward>();
    template void SbDCT2::Transform8x8<SbDCT::dirInverse>();
    template void SbDCT2::Transform16x16<SbDCT::dirForward>();
    template void SbDCT2::Transform16x16<SbDCT::dirInverse>();
    template void SbDCT2::Transform32x32<SbDCT::dirForward>();
    template void SbDCT2::Transform32x32<SbDCT::dirInverse>();

    template <bool Dir>
    void SbDCT2::Transform8x8() {
        SbDCT row0(src + 0 * step, 1); row0.Transform8<Dir>(); 
        SbDCT row1(src + 1 * step, 1); row1.Transform8<Dir>();
        SbDCT row2(src + 2 * step, 1); row2.Transform8<Dir>();
        SbDCT row3(src + 3 * step, 1); row3.Transform8<Dir>();
        SbDCT row4(src + 4 * step, 1); row4.Transform8<Dir>();
        SbDCT row5(src + 5 * step, 1); row5.Transform8<Dir>();
        SbDCT row6(src + 6 * step, 1); row6.Transform8<Dir>();
        SbDCT row7(src + 7 * step, 1); row7.Transform8<Dir>();

        SbDCT col0(src + 0, step); col0.Transform8<Dir>();
        SbDCT col1(src + 1, step); col1.Transform8<Dir>();
        SbDCT col2(src + 2, step); col2.Transform8<Dir>();
        SbDCT col3(src + 3, step); col3.Transform8<Dir>();
        SbDCT col4(src + 4, step); col4.Transform8<Dir>();
        SbDCT col5(src + 5, step); col5.Transform8<Dir>();
        SbDCT col6(src + 6, step); col6.Transform8<Dir>();
        SbDCT col7(src + 7, step); col7.Transform8<Dir>();
    }

    template <bool Dir>
    void SbDCT2::Transform16x16() {
        SbDCT row0 (src + 0  * step, 1); row0 .Transform16<Dir>(); 
        SbDCT row1 (src + 1  * step, 1); row1 .Transform16<Dir>();
        SbDCT row2 (src + 2  * step, 1); row2 .Transform16<Dir>();
        SbDCT row3 (src + 3  * step, 1); row3 .Transform16<Dir>();
        SbDCT row4 (src + 4  * step, 1); row4 .Transform16<Dir>();
        SbDCT row5 (src + 5  * step, 1); row5 .Transform16<Dir>();
        SbDCT row6 (src + 6  * step, 1); row6 .Transform16<Dir>();
        SbDCT row7 (src + 7  * step, 1); row7 .Transform16<Dir>();
        SbDCT row8 (src + 8  * step, 1); row8 .Transform16<Dir>(); 
        SbDCT row9 (src + 9  * step, 1); row9 .Transform16<Dir>();
        SbDCT row10(src + 10 * step, 1); row10.Transform16<Dir>();
        SbDCT row11(src + 11 * step, 1); row11.Transform16<Dir>();
        SbDCT row12(src + 12 * step, 1); row12.Transform16<Dir>();
        SbDCT row13(src + 13 * step, 1); row13.Transform16<Dir>();
        SbDCT row14(src + 14 * step, 1); row14.Transform16<Dir>();
        SbDCT row15(src + 15 * step, 1); row15.Transform16<Dir>();

        SbDCT col0(src  + 0,  step); col0 .Transform16<Dir>();
        SbDCT col1(src  + 1,  step); col1 .Transform16<Dir>();
        SbDCT col2(src  + 2,  step); col2 .Transform16<Dir>();
        SbDCT col3(src  + 3,  step); col3 .Transform16<Dir>();
        SbDCT col4(src  + 4,  step); col4 .Transform16<Dir>();
        SbDCT col5(src  + 5,  step); col5 .Transform16<Dir>();
        SbDCT col6(src  + 6,  step); col6 .Transform16<Dir>();
        SbDCT col7(src  + 7,  step); col7 .Transform16<Dir>();
        SbDCT col8 (src + 8 , step); col8 .Transform16<Dir>();
        SbDCT col9 (src + 9 , step); col9 .Transform16<Dir>();
        SbDCT col10(src + 10, step); col10.Transform16<Dir>();
        SbDCT col11(src + 11, step); col11.Transform16<Dir>();
        SbDCT col12(src + 12, step); col12.Transform16<Dir>();
        SbDCT col13(src + 13, step); col13.Transform16<Dir>();
        SbDCT col14(src + 14, step); col14.Transform16<Dir>();
        SbDCT col15(src + 15, step); col15.Transform16<Dir>();
    }

    template <bool Dir>
    void SbDCT2::Transform32x32() {
        SbDCT row0 (src + 0  * step, 1); row0 .Transform32<Dir>(); 
        SbDCT row1 (src + 1  * step, 1); row1 .Transform32<Dir>();
        SbDCT row2 (src + 2  * step, 1); row2 .Transform32<Dir>();
        SbDCT row3 (src + 3  * step, 1); row3 .Transform32<Dir>();
        SbDCT row4 (src + 4  * step, 1); row4 .Transform32<Dir>();
        SbDCT row5 (src + 5  * step, 1); row5 .Transform32<Dir>();
        SbDCT row6 (src + 6  * step, 1); row6 .Transform32<Dir>();
        SbDCT row7 (src + 7  * step, 1); row7 .Transform32<Dir>();
        SbDCT row8 (src + 8  * step, 1); row8 .Transform32<Dir>(); 
        SbDCT row9 (src + 9  * step, 1); row9 .Transform32<Dir>();
        SbDCT row10(src + 10 * step, 1); row10.Transform32<Dir>();
        SbDCT row11(src + 11 * step, 1); row11.Transform32<Dir>();
        SbDCT row12(src + 12 * step, 1); row12.Transform32<Dir>();
        SbDCT row13(src + 13 * step, 1); row13.Transform32<Dir>();
        SbDCT row14(src + 14 * step, 1); row14.Transform32<Dir>();
        SbDCT row15(src + 15 * step, 1); row15.Transform32<Dir>();
        SbDCT row16(src + 16 * step, 1); row16.Transform32<Dir>(); 
        SbDCT row17(src + 17 * step, 1); row17.Transform32<Dir>();
        SbDCT row18(src + 18 * step, 1); row18.Transform32<Dir>();
        SbDCT row19(src + 19 * step, 1); row19.Transform32<Dir>();
        SbDCT row20(src + 20 * step, 1); row20.Transform32<Dir>();
        SbDCT row21(src + 21 * step, 1); row21.Transform32<Dir>();
        SbDCT row22(src + 22 * step, 1); row22.Transform32<Dir>();
        SbDCT row23(src + 23 * step, 1); row23.Transform32<Dir>();
        SbDCT row24(src + 24 * step, 1); row24.Transform32<Dir>(); 
        SbDCT row25(src + 25 * step, 1); row25.Transform32<Dir>();
        SbDCT row26(src + 26 * step, 1); row26.Transform32<Dir>();
        SbDCT row27(src + 27 * step, 1); row27.Transform32<Dir>();
        SbDCT row28(src + 28 * step, 1); row28.Transform32<Dir>();
        SbDCT row29(src + 29 * step, 1); row29.Transform32<Dir>();
        SbDCT row30(src + 30 * step, 1); row30.Transform32<Dir>();
        SbDCT row31(src + 31 * step, 1); row31.Transform32<Dir>();

        SbDCT col0(src  + 0,  step); col0 .Transform32<Dir>();
        SbDCT col1(src  + 1,  step); col1 .Transform32<Dir>();
        SbDCT col2(src  + 2,  step); col2 .Transform32<Dir>();
        SbDCT col3(src  + 3,  step); col3 .Transform32<Dir>();
        SbDCT col4(src  + 4,  step); col4 .Transform32<Dir>();
        SbDCT col5(src  + 5,  step); col5 .Transform32<Dir>();
        SbDCT col6(src  + 6,  step); col6 .Transform32<Dir>();
        SbDCT col7(src  + 7,  step); col7 .Transform32<Dir>();
        SbDCT col8 (src + 8 , step); col8 .Transform32<Dir>();
        SbDCT col9 (src + 9 , step); col9 .Transform32<Dir>();
        SbDCT col10(src + 10, step); col10.Transform32<Dir>();
        SbDCT col11(src + 11, step); col11.Transform32<Dir>();
        SbDCT col12(src + 12, step); col12.Transform32<Dir>();
        SbDCT col13(src + 13, step); col13.Transform32<Dir>();
        SbDCT col14(src + 14, step); col14.Transform32<Dir>();
        SbDCT col15(src + 15, step); col15.Transform32<Dir>();
        SbDCT col16(src + 16, step); col16.Transform32<Dir>();
        SbDCT col17(src + 17, step); col17.Transform32<Dir>();
        SbDCT col18(src + 18, step); col18.Transform32<Dir>();
        SbDCT col19(src + 19, step); col19.Transform32<Dir>();
        SbDCT col20(src + 20, step); col20.Transform32<Dir>();
        SbDCT col21(src + 21, step); col21.Transform32<Dir>();
        SbDCT col22(src + 22, step); col22.Transform32<Dir>();
        SbDCT col23(src + 23, step); col23.Transform32<Dir>();
        SbDCT col24(src + 24, step); col24.Transform32<Dir>();
        SbDCT col25(src + 25, step); col25.Transform32<Dir>();
        SbDCT col26(src + 26, step); col26.Transform32<Dir>();
        SbDCT col27(src + 27, step); col27.Transform32<Dir>();
        SbDCT col28(src + 28, step); col28.Transform32<Dir>();
        SbDCT col29(src + 29, step); col29.Transform32<Dir>();
        SbDCT col30(src + 30, step); col30.Transform32<Dir>();
        SbDCT col31(src + 31, step); col31.Transform32<Dir>();
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

    void SbDCT2::Quantize16x16(const float* const tb) {
        SbDCT row0 (src + 0  * step); row0 .Quantize16(tb + (0 << 4));
        SbDCT row1 (src + 1  * step); row1 .Quantize16(tb + (1 << 4));
        SbDCT row2 (src + 2  * step); row2 .Quantize16(tb + (2 << 4));
        SbDCT row3 (src + 3  * step); row3 .Quantize16(tb + (3 << 4));
        SbDCT row4 (src + 4  * step); row4 .Quantize16(tb + (4 << 4));
        SbDCT row5 (src + 5  * step); row5 .Quantize16(tb + (5 << 4));
        SbDCT row6 (src + 6  * step); row6 .Quantize16(tb + (6 << 4));
        SbDCT row7 (src + 7  * step); row7 .Quantize16(tb + (7 << 4));
        SbDCT row8 (src + 8  * step); row8 .Quantize16(tb + (8  << 4));
        SbDCT row9 (src + 9  * step); row9 .Quantize16(tb + (9  << 4));
        SbDCT row10(src + 10 * step); row10.Quantize16(tb + (10 << 4));
        SbDCT row11(src + 11 * step); row11.Quantize16(tb + (11 << 4));
        SbDCT row12(src + 12 * step); row12.Quantize16(tb + (12 << 4));
        SbDCT row13(src + 13 * step); row13.Quantize16(tb + (13 << 4));
        SbDCT row14(src + 14 * step); row14.Quantize16(tb + (14 << 4));
        SbDCT row15(src + 15 * step); row15.Quantize16(tb + (15 << 4));
    }

    void SbDCT2::Quantize32x32(const float* const tb) {
        SbDCT row0 (src + 0  * step); row0 .Quantize32(tb + (0  << 5));
        SbDCT row1 (src + 1  * step); row1 .Quantize32(tb + (1  << 5));
        SbDCT row2 (src + 2  * step); row2 .Quantize32(tb + (2  << 5));
        SbDCT row3 (src + 3  * step); row3 .Quantize32(tb + (3  << 5));
        SbDCT row4 (src + 4  * step); row4 .Quantize32(tb + (4  << 5));
        SbDCT row5 (src + 5  * step); row5 .Quantize32(tb + (5  << 5));
        SbDCT row6 (src + 6  * step); row6 .Quantize32(tb + (6  << 5));
        SbDCT row7 (src + 7  * step); row7 .Quantize32(tb + (7  << 5));
        SbDCT row8 (src + 8  * step); row8 .Quantize32(tb + (8  << 5));
        SbDCT row9 (src + 9  * step); row9 .Quantize32(tb + (9  << 5));
        SbDCT row10(src + 10 * step); row10.Quantize32(tb + (10 << 5));
        SbDCT row11(src + 11 * step); row11.Quantize32(tb + (11 << 5));
        SbDCT row12(src + 12 * step); row12.Quantize32(tb + (12 << 5));
        SbDCT row13(src + 13 * step); row13.Quantize32(tb + (13 << 5));
        SbDCT row14(src + 14 * step); row14.Quantize32(tb + (14 << 5));
        SbDCT row15(src + 15 * step); row15.Quantize32(tb + (15 << 5));
        SbDCT row16(src + 16 * step); row16.Quantize32(tb + (16 << 5));
        SbDCT row17(src + 17 * step); row17.Quantize32(tb + (17 << 5));
        SbDCT row18(src + 18 * step); row18.Quantize32(tb + (18 << 5));
        SbDCT row19(src + 19 * step); row19.Quantize32(tb + (19 << 5));
        SbDCT row20(src + 20 * step); row20.Quantize32(tb + (20 << 5));
        SbDCT row21(src + 21 * step); row21.Quantize32(tb + (21 << 5));
        SbDCT row22(src + 22 * step); row22.Quantize32(tb + (22 << 5));
        SbDCT row23(src + 23 * step); row23.Quantize32(tb + (23 << 5));
        SbDCT row24(src + 24 * step); row24.Quantize32(tb + (24 << 5));
        SbDCT row25(src + 25 * step); row25.Quantize32(tb + (25 << 5));
        SbDCT row26(src + 26 * step); row26.Quantize32(tb + (26 << 5));
        SbDCT row27(src + 27 * step); row27.Quantize32(tb + (27 << 5));
        SbDCT row28(src + 28 * step); row28.Quantize32(tb + (28 << 5));
        SbDCT row29(src + 29 * step); row29.Quantize32(tb + (29 << 5));
        SbDCT row30(src + 30 * step); row30.Quantize32(tb + (30 << 5));
        SbDCT row31(src + 31 * step); row31.Quantize32(tb + (31 << 5));
    }
    
    
}
