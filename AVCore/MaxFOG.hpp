///
/// \file      MaxFOG.hpp
/// \brief     MaxFOG coding implementation.
/// \details   ~
/// \author    HenryDu, Steve Wang
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <cstdint>
#include <cstddef>
#include <iosfwd>

namespace SubIT {
    class SbBitBuffer;
    //======================
    // MaxFOG Coding
    //======================
    class SbCodecMaxFOG {
    public:
        static uint8_t*  MakeTree    (uint8_t* treeBeg, uint8_t* beg, uint8_t* end);
        static size_t    EncodeBytes (uint8_t* beg, uint8_t* end, std::ostream* stream, uint8_t* bitBuffer);

        static size_t    GetEncodedBits(std::istream* stream);
        static size_t    DecodeBits    (uint8_t* beg, size_t bits, std::istream* stream, uint8_t* buf);
    };
    
}
