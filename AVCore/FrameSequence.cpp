///
/// \file      FrameSequence.cpp
/// \brief     A frame sequence according to timeline.
/// \details   ~
/// \author    HenryDu
/// \date      12.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include "FrameSequence.hpp"

namespace SubIT {
    SbFrameSequence::SbFrameSequence(uint16_t num, uint16_t den) {
        SetFrameRate(num, den);
    }

    void SbFrameSequence::SetFrameRate(uint16_t num, uint16_t den) {
        frameRate = static_cast<uint32_t>(num << 16) | static_cast<uint32_t>(den);
    }

    float SbFrameSequence::GetFrequency() const {
        return static_cast<float>((frameRate >> 0) & 0xFFFF) / static_cast<float>((frameRate >> 16) & 0xFFFF);
    }
    
}
