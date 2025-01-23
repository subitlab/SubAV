///
/// \file      PPM.hpp
/// \brief     Portable Pixel Map reader and generator (For RGB(A) conversion)
/// \author    HenryDu
/// \date      23.01.2025
/// \copyright © HenryDu 2025. All right reserved.
///
#pragma once
#include <cstdint>
#include <iosfwd>

namespace SubIT {

    class SbPPM {
    public:
        uint8_t  *data = nullptr; // Bind your data to this pointer.
        uint16_t  width, height;
        uint16_t  max = 255;

        void operator()(std::ostream* os);
        void operator()(std::istream* is);
    };
}