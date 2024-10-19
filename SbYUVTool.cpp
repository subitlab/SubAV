///
/// \file      SbYUVTool.cpp
/// \brief     Create an SbYUV or convert an existed SbYUV to other MPEG format.
/// \details   ~
/// \author    SubIT
/// \date      19.10.2024
/// \copyright © SubIT 2024. All right reserved.
///

#include <charconv>
#include <filesystem>
#include <format>
#include <iostream>
#include <fstream>
#include <string>

#include "SbYUV.hpp"


namespace Sb::Video {

    void     YUVTool_PrintHelpMessage() {
        const char* message = R"(
Copyright(c) HenryDu @ SubIT 2024. All right reserved.
A tool for quickly generate SbYUV video which is a raw and fast video format for Sub-Engine.
To use this tool make sure you have installed FFMpeg and already put it in you system path.

This tool has only two commands listed below:
-from <video you want to convert to SbYUV> --> Generate .sby file.
-to   <SbYUV you want to convert back>     --> Generate .mp4 file.
)";
        std::cout << message;
    }
    uint32_t YUVTool_CreateDescTemp(std::string_view filename) {
        std::string fmt = std::format(
            "ffprobe -v quiet -select_streams v:0 -show_entries stream=width,height,r_frame_rate -of default=noprint_wrappers=1 {0:s} > temporary.txt",
        filename);
        return std::system(fmt.c_str());
    }

    uint32_t YUVTool_CreateDescriptor(YUV_Descriptor* desc) {

        std::ifstream descFile("temporary.txt");

        std::pmr::string line;

        std::getline(descFile, line);
        std::from_chars(line.data() + 6, line.data() + line.size(), desc->width);

        std::getline(descFile, line);
        std::from_chars(line.data() + 7, line.data() + line.size(), desc->height);

        std::uint16_t numerator = 0, denominator = 0;
        std::getline(descFile, line);
        const std::size_t division = line.find('/');
        std::from_chars(line.data() + 13, line.data() + division, numerator);
        std::from_chars(line.data() + division + 1, line.data() + line.size(), denominator);

        desc->frameRate = YUV_MakeFrameRate(numerator, denominator);
        desc->extraFlag = 0;
        descFile.close();

        return 0;
    }

    uint32_t YUVTool_CreateRawStream(std::string_view filename) {
        std::string fmt = std::format("ffmpeg -v quiet -i {0:s} -vf format=yuv420p -vf vflip temporary.yuv", filename);
        return std::system(fmt.c_str());
    }

    uint32_t YUVTool_CreateMPEG(const YUV_Descriptor* desc, std::string_view filename) {
        std::string fmt = std::format("ffmpeg -v quiet -f rawvideo -vcodec rawvideo "
            "-s {0:d}x{1:d} -r {2:d}/{3:d} -pix_fmt yuv420p -i temporary.yuv -vf vflip -c:v libx264 -preset slow -qp 0 {4:s}.mp4",
            desc->width, desc->height, desc->frameRate >> 16, desc->frameRate & 0xFFFF, filename.substr(0, filename.find('.')));
        return std::system(fmt.c_str());
    }

}

int main(int argc, char* argv[]) {

    switch (argc) {
    default: break;
    case 1: { Sb::Video::YUVTool_PrintHelpMessage(); break; }
    case 2: { std::cout << "Too less arguments!\nDon't pass in any parameters to check help message!"; break; }
    case 3: {
        std::string command = argv[1];
        std::string filename = argv[2];
        if (command == "-to") {
            std::ifstream sbyStream(filename, std::ios::binary);
            std::ofstream rawStream("temporary.yuv", std::ios::binary);

            Sb::Video::YUV_Descriptor desc;
            Sb::Video::YUV_ToDescriptorAndRawStream(&sbyStream, &desc, &rawStream);

            sbyStream.close();
            rawStream.close();

            Sb::Video::YUVTool_CreateMPEG(&desc, filename);
            // Remove temporary files.
            std::filesystem::remove("temporary.yuv");
            break;
        }
        if (command == "-from") {
            Sb::Video::YUVTool_CreateDescTemp(filename);
            Sb::Video::YUV_Descriptor desc;
            Sb::Video::YUVTool_CreateDescriptor(&desc);
            Sb::Video::YUVTool_CreateRawStream(filename);

            std::ifstream rawStream("temporary.yuv", std::ios::binary);
            std::ofstream sbyStream(filename.substr(0, filename.find('.')) + ".sby", std::ios::binary);
            std::size_t frameCount = Sb::Video::YUV_FromDescriptorAndRawStream(&sbyStream, &desc, &rawStream);

            std::cout << std::format("Conversion complete!\nTotal frame count: {:d}\n", frameCount);

            rawStream.close();
            sbyStream.close();
            // Remove temporary files.
            std::filesystem::remove("temporary.txt");
            std::filesystem::remove("temporary.yuv");
            break;
        }
        std::cout << "No such command!\nDon't pass in any parameters to check help message.\n";
        break;
    }
    }

    return 0;
}
