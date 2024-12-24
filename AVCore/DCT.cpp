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
#include <functional>

namespace SubIT {
    SbDCT::SbDCT(float* beg, ptrdiff_t n, ptrdiff_t s) :begin(beg), delta(n), step(s) {}

    void SbDCT::Forward(ptrdiff_t* id, ptrdiff_t f, ptrdiff_t F) {
        id[0] = f;
        id[1] = F;
    }

    void SbDCT::Inverse(ptrdiff_t* id, ptrdiff_t f, ptrdiff_t F) {
        id[0] = F;
        id[1] = f;
    }

    void SbDCT::operator()(DirectionCB dir, float* dst, CoefficientCB inside, CoefficientCB outside) {
        // First store all data into the cache.

        // Do transform and store result in cache.
        // TODO: Fast Fourier Transform (Butterfly Algorithm) optimization.
        const ptrdiff_t de = delta << 1;
        ptrdiff_t oi_i[2] = {};
        for (ptrdiff_t j = 0; j != delta; ++j) {
            dst[j] = 0.F;
            for (ptrdiff_t i = 0; i != delta; ++i) {
                std::invoke(dir, oi_i, i, j);
                const ptrdiff_t nm   = (oi_i[0] << 1) + 1;
                const float     time = std::numbers::pi_v<float> * static_cast<float>(nm) / static_cast<float>(de);
                const float     rad  = time * static_cast<float>(oi_i[1]);
                dst[j] += begin[i * step] * std::cosf(rad) * inside(i);
            }
            dst[j] *= outside(j);
        }
    }

    void SbDCT::operator()(DirectionCB dir, CoefficientCB inside, CoefficientCB outside) {
        float* cache = static_cast<float*>((alloca(sizeof(float) * static_cast<size_t>(delta))));
        operator()(dir, cache, inside, outside);

        // Copy data into pointer destination.
        for (ptrdiff_t i = 0; i != delta; ++i) {
            begin[i] = cache[i];
        }
    }

    SbDCT2::SbDCT2(float* beg, ptrdiff_t length, ptrdiff_t row_size) : begin(beg), delta(length), step(row_size) {}

    void SbDCT2::operator()(DirectionCB dir, float* dst, CoefficientCB inside, CoefficientCB outside) {
        // Square block cache.
        float* cache = static_cast<float*>((alloca(sizeof(float) * static_cast<size_t>(delta * delta))));

        // Transform each column.
        for (ptrdiff_t i = 0; i != delta; ++i) {
            SbDCT trans(begin + i, delta, step);
            trans(dir, cache + (i * delta), inside, outside);
        }

        // Transform each row.
        for(ptrdiff_t i = 0; i != delta; ++i) {
            SbDCT trans(cache + i, delta, delta);
            trans(dir, dst + (i * delta), inside, outside);
        }
    }

    void SbDCT2::operator()(DirectionCB dir, CoefficientCB inside, CoefficientCB outside) {
        float* cache = static_cast<float*>((alloca(sizeof(float) * static_cast<size_t>(delta * delta))));
        operator()(dir, cache, inside, outside);

        // Copy data into pointer destination.
        for (ptrdiff_t i = 0; i != delta; ++i) {
            for (ptrdiff_t j = 0; j != delta; ++j) {
                begin[i * step + j] = cache[i * delta + j];
            }
        }
    }

}
