///
/// \file      FrameSequence.hpp
/// \brief     Contains SbFrameSequence class which is YUV sequence.
/// \details   ~
/// \author    HenryDu
/// \date      12.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include "OwlVision.hpp"

namespace SubIT {


    //===============================================
    // SbFrameSequence (pure texture video)
    //===============================================
    //    Bytes     |  Description  |     Value     |
    //==============|===============|================
    //    [0,7)     |    Header     |   SB-AV-FS   |
    //==============|===============|================
    //    [7,15)    |    Width      |  64 bit uint  |
    //==============|===============|================
    //    [15,23)   |    Height     |  64 bit uint  |
    //==============|===============|================
    //    [23,27)   |   FPS Mask    |  32 bit uint  |
    //==============|===============|================
    //   [27,EOF)   |     DATA      |  Byte Stream  |
    //==============|===============|================ 
    //        Class implemented all above.
    //===============================================
    class SbMacaqueMixtureCoreSequence { 
    public:
        // This is a special block -- high 16 bit is the numerator and
        // low 16 bit is the denominator. Do division and we get FPS.
        // Inverse the quotient we get "time duration between two frames".
        uint32_t           frameRate;
        SbOwlVisionCoreImage  image; // This is a temporary buffer.

        SbMacaqueMixtureCoreSequence() = default;
        SbMacaqueMixtureCoreSequence(uint16_t num, uint16_t den);
        SbMacaqueMixtureCoreSequence(const SbMacaqueMixtureCoreSequence& other) = default;
        SbMacaqueMixtureCoreSequence(SbMacaqueMixtureCoreSequence&& other) = default;
        ~SbMacaqueMixtureCoreSequence() = default;
        
        void     SetFrameRate(uint16_t num, uint16_t den);
        float    GetFrequency() const;
    };
    
}
