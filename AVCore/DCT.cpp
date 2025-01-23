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

namespace SubIT {
    
    SbDCT::SbDCT(float* beg, ptrdiff_t s) :src(beg), step(s) {}
    
    template void SbDCT::Transform4<SbDCT::dirForward>();
    template void SbDCT::Transform4<SbDCT::dirInverse>();
    template void SbDCT::Transform8<SbDCT::dirForward>();
    template void SbDCT::Transform8<SbDCT::dirInverse>();
    template void SbDCT::Transform16<SbDCT::dirForward>();
    template void SbDCT::Transform16<SbDCT::dirInverse>();
    template void SbDCT::Transform32<SbDCT::dirForward>();
    template void SbDCT::Transform32<SbDCT::dirInverse>();
    template void SbDCT::Quantize4<SbDCT::dirForward>(const float* const);
    template void SbDCT::Quantize4<SbDCT::dirInverse>(const float* const);
    template void SbDCT::Quantize8<SbDCT::dirForward>(const float* const);
    template void SbDCT::Quantize8<SbDCT::dirInverse>(const float* const);
    template void SbDCT::Quantize16<SbDCT::dirForward>(const float* const);
    template void SbDCT::Quantize16<SbDCT::dirInverse>(const float* const);
    template void SbDCT::Quantize32<SbDCT::dirForward>(const float* const);
    template void SbDCT::Quantize32<SbDCT::dirInverse>(const float* const);

    float& SbDCT::At(ptrdiff_t i) {
        return src[i * step];
    }
    
    template <bool Dir>
    void SbDCT::Transform4() {
        constexpr float a = 0.5F;
        constexpr float b = 0.270598F;
        constexpr float c = 0.653281F;
        if constexpr (Dir == dirForward) {
            const auto [k0, k1] = SbSIMD::Rotate2D(a, a, At(0) + At(3), At(1) + At(2));
            const auto [k2, k3] = SbSIMD::Rotate2D(b, c, At(0) - At(3), At(1) - At(2));
            At(0) = k1;
            At(1) = k3;
            At(2) = k0;
            At(3) = k2;
        }
        else if constexpr (Dir == dirInverse) {
            const auto [t0, t1] = SbSIMD::Rotate2D(a, a, At(0), At(2));
            const auto [t2, t3] = SbSIMD::Rotate2D(b, c, At(1), At(3));
            At(0) = t1 + t3;
            At(1) = t0 + t2;
            At(2) = t0 - t2;
            At(3) = t1 - t3;
        }
    }
    
    template <bool Dir>
    void SbDCT::Transform8() {
        constexpr float a = 0.3535533905F;
        constexpr float b = 0.4903926402F;
        constexpr float c = 0.4157348061F;
        constexpr float d = 0.4619397662F;
        constexpr float e = 0.0975451610F;
        constexpr float f = 0.2777851165F;
        constexpr float g = 0.1913417161F;
        
        if constexpr (Dir == dirForward) {
            const float s0 = At(0) + At(7), s1 = At(0) - At(7);
            const float s2 = At(1) + At(6), s3 = At(1) - At(6);
            const float s4 = At(2) + At(5), s5 = At(2) - At(5);
            const float s6 = At(3) + At(4), s7 = At(3) - At(4);
            const auto [g0, g1] = SbSIMD::Rotate2D(a, a, s0 + s6, s2 + s4);
            const auto [g2, g3] = SbSIMD::Rotate2D(d, g, s2 - s4, s0 - s6);
            const auto [r0, r1] = SbSIMD::Rotate2D(b, e, s7, s1); 
            const auto [r2, r3] = SbSIMD::Rotate2D(c, f, s5, s3);
            const auto [t0, t1] = SbSIMD::Rotate2D(c, f, s1, s7);
            const auto [t2, t3] = SbSIMD::Rotate2D(b, e, s3, s5);
            At(0) =  g1;
            At(2) =  g3;
            At(4) =  g0;
            At(6) = -g2;
            At(7) = r2 - r0;
            At(5) = t1 - t2;
            At(3) = t0 - t3;
            At(1) = r1 + r3;
        }
        else if constexpr (Dir == dirInverse) {
            const auto [s1, s0] = SbSIMD::Rotate2D(a, a, At(0), At(4));
            const auto [s2, s3] = SbSIMD::Rotate2D(g, d, At(2), At(6));
            const auto [g0, g1] = SbSIMD::Rotate2D(e, b, At(1), At(7));
            const auto [g2, g3] = SbSIMD::Rotate2D(b, e, At(3), At(5));
            const auto [g4, g5] = SbSIMD::Rotate2D(c, f, At(1), At(7));
            const auto [g6, g7] = SbSIMD::Rotate2D(f, c, At(3), At(5));
            const float t0 = g1 + g7;
            const float t1 = g3 - g4;
            const float t2 = g2 - g5;
            const float t3 = g0 - g6;
            const float k0 = s0 + s3, k1 = s0 - s3;
            const float k2 = s1 + s2, k3 = s1 - s2;
            At(0) = k0 + t0;
            At(2) = k3 - t2;
            At(4) = k1 - t3;
            At(6) = k2 + t1;
            At(7) = k0 - t0;
            At(5) = k3 + t2;
            At(3) = k1 + t3;
            At(1) = k2 - t1;
        }
    }
    
    template <bool Dir>
    void SbDCT::Transform16() {
        // TODO: Implement it.
    }
    
    template <bool Dir>
    void SbDCT::Transform32() {
        // TODO: Implement it.
    }

    template <bool Dir>
    void SbDCT::Quantize4(const float* const tb) {
        if constexpr (Dir == dirForward) { SbSIMD::DivA4(src, tb); return; }
        if constexpr (Dir == dirInverse) { SbSIMD::MulA4(src, tb); return; }
    }
    
    template <bool Dir>
    void SbDCT::Quantize8 (const float* const tb) {
        if constexpr (Dir == dirForward) { SbSIMD::DivA4(src + 0, tb + 0);
                                           SbSIMD::DivA4(src + 4, tb + 4); return; }
        if constexpr (Dir == dirInverse) { SbSIMD::MulA4(src + 0, tb + 0);
                                           SbSIMD::MulA4(src + 4, tb + 4); return; }
    }

    template <bool Dir>
    void SbDCT::Quantize16(const float* const tb) {
        if constexpr (Dir == dirForward) {  SbSIMD::DivA4(src + 0,  tb + 0);
                                            SbSIMD::DivA4(src + 4,  tb + 4);
                                            SbSIMD::DivA4(src + 8,  tb + 8);
                                            SbSIMD::DivA4(src + 12, tb + 12); return;}
        if constexpr (Dir == dirInverse) {  SbSIMD::MulA4(src + 0,  tb + 0);
                                            SbSIMD::MulA4(src + 4,  tb + 4);
                                            SbSIMD::MulA4(src + 8,  tb + 8);
                                            SbSIMD::MulA4(src + 12, tb + 12); return;}
    }

    template <bool Dir>
    void SbDCT::Quantize32(const float* const tb) {
        if constexpr (Dir == dirForward) {  SbSIMD::DivA4(src + 0,   tb + 0);
                                            SbSIMD::DivA4(src + 4,   tb + 4);
                                            SbSIMD::DivA4(src + 8,   tb + 8);
                                            SbSIMD::DivA4(src + 12,  tb + 12);
                                            SbSIMD::DivA4(src + 16,  tb + 16);
                                            SbSIMD::DivA4(src + 20,  tb + 20);
                                            SbSIMD::DivA4(src + 24,  tb + 24);
                                            SbSIMD::DivA4(src + 28,  tb + 28); return;}
        if constexpr (Dir == dirInverse) {  SbSIMD::MulA4(src + 0,   tb + 0); 
                                            SbSIMD::MulA4(src + 4,   tb + 4); 
                                            SbSIMD::MulA4(src + 8,   tb + 8); 
                                            SbSIMD::MulA4(src + 12,  tb + 12);
                                            SbSIMD::MulA4(src + 16,  tb + 16);
                                            SbSIMD::MulA4(src + 20,  tb + 20);
                                            SbSIMD::MulA4(src + 24,  tb + 24);
                                            SbSIMD::MulA4(src + 28,  tb + 28); return; }
    }

    SbDCT2::SbDCT2(float* beg, ptrdiff_t row_size) : src(beg), step(row_size) {}
    
    template void SbDCT2::Transform4x4<SbDCT::dirForward>();
    template void SbDCT2::Transform4x4<SbDCT::dirInverse>();
    template void SbDCT2::Transform8x8<SbDCT::dirForward>();
    template void SbDCT2::Transform8x8<SbDCT::dirInverse>();
    template void SbDCT2::Transform16x16<SbDCT::dirForward>();
    template void SbDCT2::Transform16x16<SbDCT::dirInverse>();
    template void SbDCT2::Transform32x32<SbDCT::dirForward>();
    template void SbDCT2::Transform32x32<SbDCT::dirInverse>();
    template void SbDCT2::Quantize4x4<SbDCT::dirForward>(const float* const);
    template void SbDCT2::Quantize4x4<SbDCT::dirInverse>(const float* const);
    template void SbDCT2::Quantize8x8<SbDCT::dirForward>(const float* const);
    template void SbDCT2::Quantize8x8<SbDCT::dirInverse>(const float* const);
    template void SbDCT2::Quantize16x16<SbDCT::dirForward>(const float* const);
    template void SbDCT2::Quantize16x16<SbDCT::dirInverse>(const float* const);
    template void SbDCT2::Quantize32x32<SbDCT::dirForward>(const float* const);
    template void SbDCT2::Quantize32x32<SbDCT::dirInverse>(const float* const);

    template <bool Dir>
    void SbDCT2::Transform4x4() {
        SbDCT row0(src + 0 * step, 1); row0.Transform4<Dir>();
        SbDCT row1(src + 1 * step, 1); row1.Transform4<Dir>();
        SbDCT row2(src + 2 * step, 1); row2.Transform4<Dir>();
        SbDCT row3(src + 3 * step, 1); row3.Transform4<Dir>();

        SbDCT col0(src + 0, step); col0.Transform4<Dir>();
        SbDCT col1(src + 1, step); col1.Transform4<Dir>();
        SbDCT col2(src + 2, step); col2.Transform4<Dir>();
        SbDCT col3(src + 3, step); col3.Transform4<Dir>();
    }
    
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
    
    template <bool Dir> 
    void SbDCT2::Quantize4x4(const float* const tb) {
        SbDCT row0(src + 0 * step); row0.Quantize4<Dir>(tb + (0 << 2));
        SbDCT row1(src + 1 * step); row1.Quantize4<Dir>(tb + (1 << 2));
        SbDCT row2(src + 2 * step); row2.Quantize4<Dir>(tb + (2 << 2));
        SbDCT row3(src + 3 * step); row3.Quantize4<Dir>(tb + (3 << 2));
    }

    template <bool Dir> 
    void SbDCT2::Quantize8x8(const float* const tb) {
        SbDCT row0(src + 0 * step); row0.Quantize8<Dir>(tb + (0 << 3));
        SbDCT row1(src + 1 * step); row1.Quantize8<Dir>(tb + (1 << 3));
        SbDCT row2(src + 2 * step); row2.Quantize8<Dir>(tb + (2 << 3));
        SbDCT row3(src + 3 * step); row3.Quantize8<Dir>(tb + (3 << 3));
        SbDCT row4(src + 4 * step); row4.Quantize8<Dir>(tb + (4 << 3));
        SbDCT row5(src + 5 * step); row5.Quantize8<Dir>(tb + (5 << 3));
        SbDCT row6(src + 6 * step); row6.Quantize8<Dir>(tb + (6 << 3));
        SbDCT row7(src + 7 * step); row7.Quantize8<Dir>(tb + (7 << 3));
    }

    template <bool Dir>
    void SbDCT2::Quantize16x16(const float* const tb) {
        SbDCT row0 (src + 0  * step); row0 .Quantize16<Dir>(tb + (0 << 4));
        SbDCT row1 (src + 1  * step); row1 .Quantize16<Dir>(tb + (1 << 4));
        SbDCT row2 (src + 2  * step); row2 .Quantize16<Dir>(tb + (2 << 4));
        SbDCT row3 (src + 3  * step); row3 .Quantize16<Dir>(tb + (3 << 4));
        SbDCT row4 (src + 4  * step); row4 .Quantize16<Dir>(tb + (4 << 4));
        SbDCT row5 (src + 5  * step); row5 .Quantize16<Dir>(tb + (5 << 4));
        SbDCT row6 (src + 6  * step); row6 .Quantize16<Dir>(tb + (6 << 4));
        SbDCT row7 (src + 7  * step); row7 .Quantize16<Dir>(tb + (7 << 4));
        SbDCT row8 (src + 8  * step); row8 .Quantize16<Dir>(tb + (8  << 4));
        SbDCT row9 (src + 9  * step); row9 .Quantize16<Dir>(tb + (9  << 4));
        SbDCT row10(src + 10 * step); row10.Quantize16<Dir>(tb + (10 << 4));
        SbDCT row11(src + 11 * step); row11.Quantize16<Dir>(tb + (11 << 4));
        SbDCT row12(src + 12 * step); row12.Quantize16<Dir>(tb + (12 << 4));
        SbDCT row13(src + 13 * step); row13.Quantize16<Dir>(tb + (13 << 4));
        SbDCT row14(src + 14 * step); row14.Quantize16<Dir>(tb + (14 << 4));
        SbDCT row15(src + 15 * step); row15.Quantize16<Dir>(tb + (15 << 4));
    }

    template <bool Dir>
    void SbDCT2::Quantize32x32(const float* const tb) {
        SbDCT row0 (src + 0  * step); row0 .Quantize32<Dir>(tb + (0  << 5));
        SbDCT row1 (src + 1  * step); row1 .Quantize32<Dir>(tb + (1  << 5));
        SbDCT row2 (src + 2  * step); row2 .Quantize32<Dir>(tb + (2  << 5));
        SbDCT row3 (src + 3  * step); row3 .Quantize32<Dir>(tb + (3  << 5));
        SbDCT row4 (src + 4  * step); row4 .Quantize32<Dir>(tb + (4  << 5));
        SbDCT row5 (src + 5  * step); row5 .Quantize32<Dir>(tb + (5  << 5));
        SbDCT row6 (src + 6  * step); row6 .Quantize32<Dir>(tb + (6  << 5));
        SbDCT row7 (src + 7  * step); row7 .Quantize32<Dir>(tb + (7  << 5));
        SbDCT row8 (src + 8  * step); row8 .Quantize32<Dir>(tb + (8  << 5));
        SbDCT row9 (src + 9  * step); row9 .Quantize32<Dir>(tb + (9  << 5));
        SbDCT row10(src + 10 * step); row10.Quantize32<Dir>(tb + (10 << 5));
        SbDCT row11(src + 11 * step); row11.Quantize32<Dir>(tb + (11 << 5));
        SbDCT row12(src + 12 * step); row12.Quantize32<Dir>(tb + (12 << 5));
        SbDCT row13(src + 13 * step); row13.Quantize32<Dir>(tb + (13 << 5));
        SbDCT row14(src + 14 * step); row14.Quantize32<Dir>(tb + (14 << 5));
        SbDCT row15(src + 15 * step); row15.Quantize32<Dir>(tb + (15 << 5));
        SbDCT row16(src + 16 * step); row16.Quantize32<Dir>(tb + (16 << 5));
        SbDCT row17(src + 17 * step); row17.Quantize32<Dir>(tb + (17 << 5));
        SbDCT row18(src + 18 * step); row18.Quantize32<Dir>(tb + (18 << 5));
        SbDCT row19(src + 19 * step); row19.Quantize32<Dir>(tb + (19 << 5));
        SbDCT row20(src + 20 * step); row20.Quantize32<Dir>(tb + (20 << 5));
        SbDCT row21(src + 21 * step); row21.Quantize32<Dir>(tb + (21 << 5));
        SbDCT row22(src + 22 * step); row22.Quantize32<Dir>(tb + (22 << 5));
        SbDCT row23(src + 23 * step); row23.Quantize32<Dir>(tb + (23 << 5));
        SbDCT row24(src + 24 * step); row24.Quantize32<Dir>(tb + (24 << 5));
        SbDCT row25(src + 25 * step); row25.Quantize32<Dir>(tb + (25 << 5));
        SbDCT row26(src + 26 * step); row26.Quantize32<Dir>(tb + (26 << 5));
        SbDCT row27(src + 27 * step); row27.Quantize32<Dir>(tb + (27 << 5));
        SbDCT row28(src + 28 * step); row28.Quantize32<Dir>(tb + (28 << 5));
        SbDCT row29(src + 29 * step); row29.Quantize32<Dir>(tb + (29 << 5));
        SbDCT row30(src + 30 * step); row30.Quantize32<Dir>(tb + (30 << 5));
        SbDCT row31(src + 31 * step); row31.Quantize32<Dir>(tb + (31 << 5));
    }
}
