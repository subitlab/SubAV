///
/// \file      RGB.hpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang, Henry Du
/// \date      1.22.2025
/// \copyright © Steve Wang 2025
///
#include "OwlVision.hpp"

namespace SubIT {
    class SbRGBA {
    public:
        SbOwlVisionCoreImage *img;
        void operator ()(uint8_t *dest);
    };

    class SbRGB {
    public:
        SbOwlVisionCoreImage* img;
        void operator()(uint8_t* dest);
    };
}
