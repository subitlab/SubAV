/// 
/// \file      RFSGenerator.cpp
/// \author    Henry Du
/// \date      11.16.2024
/// \copyright © Henry Du @ SubIT 2024. All right reserved.
/// 

#include <format>
#include <fstream>
#include <string>
#include <cstdlib>

#include "../AVCore/StandaloneImage.hpp"

#include "RFSGenerator.hpp"

#include <iostream>

namespace SubIT {
    uint32_t SbFFMpegCommander::YUVCreateDesc(std::string_view filename, std::string_view out) {
        std::string fmt = std::format(
            "ffprobe -v quiet -select_streams v:0 -show_entries stream=width,height,r_frame_rate -of default=noprint_wrappers=1 {0:s} > {1:s}.txt",
            filename, out);
        return std::system(fmt.c_str());
    }

    uint32_t SbFFMpegCommander::YUVParseDesc(std::string_view tmpName, size_t* width, size_t* height, uint16_t* num,
                                             uint16_t* den) {
        std::ifstream descFile(std::format("{0:s}.txt", tmpName));

        std::pmr::string line;

        std::getline(descFile, line);
        std::from_chars(line.data() + 6, line.data() + line.size(), *width);

        std::getline(descFile, line);
        std::from_chars(line.data() + 7, line.data() + line.size(), *height);

        std::getline(descFile, line);
        const std::size_t division = line.find('/');
        std::from_chars(line.data() + 13, line.data() + division, *num);
        std::from_chars(line.data() + division + 1, line.data() + line.size(), *den);
        return 0;
    }

    uint32_t SbFFMpegCommander::YUVCreateStream(std::string_view filename, std::string_view out) {
        std::string fmt = std::format("ffmpeg -v quiet -i {0:s} -vf format=yuv420p -vf vflip {1:s}.yuv", filename, out);
        return std::system(fmt.c_str());
    }

    uint32_t SbFFMpegCommander::StandaloneFillDescAndAllocate(SbStandaloneImage* image, std::string_view tmpName) {
        uint16_t tmp1, tmp2;
        YUVParseDesc(tmpName, &image->width, &image->height, &tmp1, &tmp2);
        image->Allocate();
        return 0;
    }
}
