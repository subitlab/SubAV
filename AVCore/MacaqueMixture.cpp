///
/// \file      MacaqueMixture.cpp
/// \brief     A frame sequence according to timeline.
/// \details   ~
/// \author    HenryDu
/// \date      12.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include "MacaqueMixture.hpp"

namespace SubIT {
    SbMacaqueMixtureCoreSequence::SbMacaqueMixtureCoreSequence(uint16_t num, uint16_t den) {
        SetFrameRate(num, den);
    }

    void SbMacaqueMixtureCoreSequence::SetFrameRate(uint16_t num, uint16_t den) {
        frameRate = static_cast<uint32_t>(num << 16) | static_cast<uint32_t>(den);
    }

    float SbMacaqueMixtureCoreSequence::GetFrequency() const {
        return static_cast<float>((frameRate >> 0) & 0xFFFF) / static_cast<float>((frameRate >> 16) & 0xFFFF);
    }
}
