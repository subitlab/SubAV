///
/// \file      RGB.cpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang
/// \date      1.2.2025
/// \copyright © Steve Wang 2025. All right reserved.
///

#include "RGB.hpp"

extern void yuv_to_rgba(uint8_t *src, uint8_t *dest, size_t width, size_t height);

namespace SubIT {
    static const float yuva2rgba_matrix[16] = {
        1.F,       0.F, 1.13983F,   0.F,
        1.F, -0.39645F, -0.5806F,   0.F,
        1.F,  2.03211F,      0.F,   0.F,
        0.F,       0.F,      0.F,   1.F
    };
    inline void SbRGB::operator()(uint8_t *dest) {
        yuv_to_rgba(img->data, dest, img->width, img->height);
    }
}
