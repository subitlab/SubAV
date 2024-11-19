///
/// \file      RFSGenerator.hpp
/// \brief     Use to generate raw yuv file with FFMpeg.
/// \details   ~
/// \author    HenryDu
/// \date      12.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace SubIT {

    class SbStandaloneImage;
    class SbFrameSequence;

    class SbFFMpegCommander {
    public:
        static uint32_t YUVCreateDesc(std::string_view filename, std::string_view out = "temp");
        static uint32_t YUVParseDesc(std::string_view tmpName, size_t* width, size_t* height, uint16_t* num, uint16_t* den);
        static uint32_t YUVCreateStream(std::string_view filename, std::string_view out = "temp");
        static uint32_t StandaloneFillDescAndAllocate(SbStandaloneImage* image, std::string_view tmpName);
    };

}

