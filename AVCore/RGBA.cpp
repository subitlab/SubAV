///
/// \file      RGB.cpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang
/// \date      1.22.2025
/// \copyright © Steve Wang 2025.
///

#include "RGBA.hpp"
#include "SIMD.hpp"
#include <cstdint>

namespace SubIT {
    void yuv_to_rgba(uint8_t *src, uint8_t *dest, size_t width, size_t height) {
        const size_t u_offset = width*height, v_offset = u_offset+(u_offset>>2);
        for(int i = 0; i<height; ++i) {
            uint8_t *des = dest+i*width;
            for(int j = 0; j<width; ++j) {
                SbSIMD::yuv2rgba(src[i*width+j], src[u_offset+(i>>1)*width+(j>>1)], src[v_offset+(i>>1)*width+(j>>1)], des);
                des += 4;
            }
        }
    }
    inline void SbRGBA::operator()(uint8_t *dest) {
        yuv_to_rgba(img->data, dest, img->width, img->height);
    }
}
