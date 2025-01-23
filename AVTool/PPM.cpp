///
/// \file      PPM.cpp
/// \brief     Implementation of PPM.hpp
/// \author    HenryDu
/// \date      23.01.2025
/// \copyright © HenryDu 2025. All right reserved.
///

#include <istream>
#include <ostream>
#include <string>
#include <charconv>
#include <format>

#include "PPM.hpp"

namespace SubIT {

    void SbPPM::operator()(std::ostream* os) {
        std::string header = std::format("P3 {:d} {:d} {:d} ", width, height, max);
        os->write(header.c_str(), static_cast<size_t>(header.size()));
        char buffer[12] = "";
        for (size_t i = 0; i != (width * height * 3); i += 3) {
            auto end = std::format_to(buffer, "{:d} {:d} {:d} ", data[i], data[i + 1], data[i + 2]);
            os->write(buffer, static_cast<size_t>(end - buffer));
        }
    }

    void SbPPM::operator()(std::istream* is) {
        // Skip P3
        is->seekg(3, std::ios::cur);
        // Start to read.
        char  buffer[6] = "";
        char* e = buffer;
        for (size_t i = 0; i != (width * height + 1) * 3; ++e) {
            *e = static_cast<char>(is->get());
            // Time to read value from buffer.
            if (std::isspace(*e)) {
                uint16_t c = 0;
                std::from_chars(buffer, e, c);
                // Mode select -- whether it's size description or pixel data.
                switch (i) {
                default: data[i - 3] = static_cast<uint8_t>(c); break;
                case 0: width = c;                       break;
                case 1: height = c;                       break;
                case 2: max = c;                       break;
                }
                // This is safe absolutely because we will add it up right after this if statement. 
                e = buffer - 1;
                ++i;
            }
        } // for.
    }
    
}