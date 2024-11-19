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
        using CoefficientCB = float(*)(ptrdiff_t);
        using DirectionCB   = void (*)(ptrdiff_t*, ptrdiff_t, ptrdiff_t);

        // Iterate range.
        float     *begin;
        ptrdiff_t  delta;

        // For extension purpose.
        ptrdiff_t  step;

        SbDCT(float* beg, ptrdiff_t n, ptrdiff_t s = 1);
        SbDCT(const SbDCT&)            = default;
        SbDCT(SbDCT&&)                 = default;
        SbDCT& operator=(const SbDCT&) = default;
        SbDCT& operator=(SbDCT&&)      = default;

        static void Forward(ptrdiff_t* id, ptrdiff_t f, ptrdiff_t F);
        static void Inverse(ptrdiff_t* id, ptrdiff_t f, ptrdiff_t F);

        // Composed transformer copy mode.
        void operator()(DirectionCB dir, float* dst,
            CoefficientCB inside = [](ptrdiff_t) { return 1.F; }, CoefficientCB outside = [](ptrdiff_t) { return 1.F; });
        // Composed transformer in-place mode.
        void operator()(DirectionCB dir,
            CoefficientCB inside = [](ptrdiff_t) { return 1.F; }, CoefficientCB outside = [](ptrdiff_t) { return 1.F; });
    };

    //==============================
    // Desecrate Cosine Transform 2D
    //==============================

    class SbDCT2 {
    public:
        // Contents from SbDCT, they are pretty much the same.
        using DirectionCB   = SbDCT::DirectionCB;
        using CoefficientCB = SbDCT::CoefficientCB;

        float     *begin;
        ptrdiff_t  delta;
        ptrdiff_t  step;

        // SbDCT2 can only handle data in a square area.
        SbDCT2(float* beg, ptrdiff_t length, ptrdiff_t row_size);
        SbDCT2(const SbDCT2&)              = default;
        SbDCT2(SbDCT2&&)                   = default;
        SbDCT2& operator=(const SbDCT2&)   = default;
        SbDCT2& operator=(SbDCT2&&)        = default;
        ~SbDCT2() = default;

        void operator()(DirectionCB dir, float* dst,
            CoefficientCB inside = [](ptrdiff_t) { return 1.F; }, CoefficientCB outside = [](ptrdiff_t) { return 1.F; });
        void operator()(DirectionCB dir,
            CoefficientCB inside = [](ptrdiff_t) { return 1.F; }, CoefficientCB outside = [](ptrdiff_t) { return 1.F; });
    };
}
