///
/// \file      IKP.hpp
/// \brief     IKP byte decoder implementation.
/// \author    Steve Wang (Formatted by Henry Du)
/// \date      12.27.2024
/// \copyright © Steve Wang 2024. All right reserved.
///
#pragma once

#include <cstdint>

namespace SubIT {

    // IKP accelerated byte decoder by Steve Wang.
    class SbIKPByteDecoder {
    public:
        using fn = uint8_t(*)(uint8_t**, uint8_t*);
        
        fn        decoderFun;
        int64_t   funsiz;
        
        SbIKPByteDecoder(const uint8_t *freqs, const uint8_t totalCount);
        ~SbIKPByteDecoder();
        
        inline uint8_t operator()(uint8_t **data, uint8_t *bitPos) const { return decoderFun(data, bitPos); }
    };
    
}