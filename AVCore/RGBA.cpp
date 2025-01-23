///
/// \file      RGB.cpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang, Henry Du
/// \date      1.22.2025
/// \copyright © Steve Wang 2025.
///

#include "RGBA.hpp"
#include "SIMD.hpp"
#include <cstdint>
#include <algorithm>

namespace SubIT {
    //static void yuv_to_rgba(uint8_t *src, uint8_t *dest, size_t width, size_t height) {
    //    const size_t u_offset = width*height, v_offset = u_offset+(u_offset>>2);
    //    for(int i = 0; i<height; ++i) {
    //        uint8_t *des = dest+i*width;
    //        for(int j = 0; j<width; ++j) {
    //            SbSIMD::yuv2rgba(src[i*width+j], src[u_offset+(i>>1)*width+(j>>1)], src[v_offset+(i>>1)*width+(j>>1)], des);
    //            des += 4;
    //        }
    //    }
    //}

    void SbRGBA::operator()(uint8_t *dest) {
        const size_t u_offset = img->width * img->height;
        const size_t v_offset = u_offset + (u_offset >> 2);

        for (int i = 0; i < img->height; ++i) {
            for (int j = 0; j < img->width; ++j) {
                SbSIMD::yuv2rgba(img->entity[i * img->width + j], img->entity[u_offset + (i >> 1) * img->width + (j >> 1)], img->entity[v_offset + (i >> 1) * img->width + (j >> 1)], dest);
                dest += 4;
            }
        }
    }

    void SbRGB::operator()(uint8_t* dest) {
        const size_t u_offset = img->width * img->height;
        const size_t v_offset = u_offset + (u_offset >> 2);
        uint8_t rgba[4];
        for (int i = 0; i < img->height; ++i) {
            for (int j = 0; j < img->width; ++j) {
                SbSIMD::yuv2rgba(img->entity[i * img->width + j], img->entity[u_offset + (i >> 1) * img->width + (j >> 1)], img->entity[v_offset + (i >> 1) * img->width + (j >> 1)], rgba);
                std::memcpy(dest, rgba, 3);
                dest += 3;
            }
        }

    }
}
