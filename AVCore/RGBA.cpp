///
/// \file      RGB.cpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang, Henry Du
/// \date      1.22.2025
/// \copyright © Steve Wang 2025.
///

#include "RGBA.hpp"
#include "SIMD.hpp"
#include "common.hpp"

namespace SubIT {

    void SbRGBA::operator()(uint8_t *dest) {
        const size_t u_offset = img->width * img->height;
        const size_t v_offset = u_offset + (u_offset >> 2);
        const size_t uv_width = img->width>>1;
        for(int i = 0; i < img->height>>1; ++i) {
            for(int j = 0; j < uv_width; ++j) {
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], dest);
                dest += 4;
            }
            dest += img->width*4;
        }
        /*for (int i = 0; i < img->height; ++i) {
            for (int j = 0; j < img->width; ++j) {
                SbSIMD::yuv2rgba(img->entity[i * img->width + j], img->entity[u_offset + (i >> 1) * (img->width>>1) + (j >> 1)], img->entity[v_offset + (i >> 1) * (img->width>>1) + (j >> 1)], dest);
                dest += 4;
            }
        }*/
    }

    void SbRGB::operator()(uint8_t* dest) {
        const size_t u_offset = img->width * img->height;
        const size_t v_offset = u_offset + (u_offset >> 2);
        const size_t uv_width = img->width>>1;
        uint8_t rgba[4];
        for(int i = 0; i < img->height>>1; ++i) {
            for(int j = 0; j < uv_width; ++j) {
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                ++j;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*img->width+(j<<1)], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
                SbSIMD::yuv2rgba(img->entity[(i<<1)*img->width+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest, rgba, 3);
                SbSIMD::yuv2rgba(img->entity[((i<<1)+1)*(img->width)+(j<<1)+1], img->entity[u_offset+i*uv_width+j], img->entity[v_offset+i*uv_width+j], rgba);
                std::memcpy(dest+img->width*3, rgba, 3);
                dest += 3;
            }
            dest += img->width*3;
        }
        /*
        for (int i = 0; i < img->height; ++i) {
            for (int j = 0; j < img->width; ++j) {
                SbSIMD::yuv2rgba(img->entity[i * img->width + j], img->entity[u_offset + (i >> 1) * (img->width>>1) + (j >> 1)], img->entity[v_offset + (i >> 1) * (img->width>>1) + (j >> 1)], rgba);
                std::memcpy(dest, rgba, 3);
                dest += 3;
            }
        }*/

    }
}
