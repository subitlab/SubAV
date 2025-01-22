///
/// \file      RGB.hpp
/// \brief     YUV to RGB(A) convertor
/// \author    Steve Wang
/// \date      1.2.2025
/// \copyright © Steve Wang 2025. All right reserved.
///
#include "OwlVision.hpp"

namespace SubIT {
    class SbRGB {
    public:
        SbOwlVisionCoreImage *img;
        void operator ()(uint8_t *dest);
    };
}
