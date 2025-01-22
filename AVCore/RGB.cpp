///
/// \file      RGB.cpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang
/// \date      1.22.2025
/// \copyright © Steve Wang 2025.
///

#include "RGB.hpp"
#include "SIMD.hpp"
#include <cstdint>
#include <future>

namespace SubIT {
    void yuv_to_rgba(uint8_t *src, uint8_t *dest, size_t width, size_t height) {
        const size_t u_offset = width*height, v_offset = u_offset+(u_offset>>2);
        std::future<void> *futs = new std::future<void>[height];
        for(int i = 0; i<height; ++i) {
            futs[i] = (std::async([&](){
                uint8_t *des = dest+i*width;
                for(int j = 0; j<width; ++j) {
                    SbSIMD::yuv2rgba(src[i*width+j], src[u_offset+(i>>1)*width+(j>>1)], src[v_offset+(i>>1)*width+(j>>1)], des);
                    des += 4;
                }
            }));
        }
        for(int i = 0; i<height; i++) {
            futs[i].wait();
        }
        delete[] futs;
    }
    inline void SbRGB::operator()(uint8_t *dest) {
        yuv_to_rgba(img->data, dest, img->width, img->height);
    }
}
