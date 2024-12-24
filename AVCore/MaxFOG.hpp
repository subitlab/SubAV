///
/// \file      MaxFOG.hpp
/// \brief     MaxFOG coding implementation.
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <cstdint>
#include <cstddef>
#include <iosfwd>

namespace ikp {
    class byteDecoder {
        typedef unsigned char(*fn)(unsigned char**, unsigned char*);
        fn decoderFun;
        long long int funsiz;
    public:
        inline unsigned char operator()(unsigned char **data, unsigned char *bitPos) const {
            return decoderFun(data, bitPos);
        }
        byteDecoder(const unsigned char *freqs, const unsigned char totalCount);
        ~byteDecoder();
    };
}

namespace SubIT {
    //======================
    // MaxFOG Coding
    //======================
    class SbCodecMaxFOG {
    public:
        static uint8_t*  MakeTree    (uint8_t* treeBeg, uint8_t* beg, uint8_t* end);
        static size_t    EncodeBytes(uint8_t* beg,     uint8_t* end, std::ostream* stream);

        static size_t    GetEncodedBits(std::istream* stream);
        static size_t    DecodeBits    (uint8_t* beg, size_t bits, std::istream* stream);
    };
    
}
