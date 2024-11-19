/// 
/// \file      Main.cpp
/// \author    Henry Du
/// \date      11.16.2024
/// \copyright © Henry Du @ SubIT 2024. All right reserved.
/// 

#include <dinput.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "../AVCore/StandaloneImage.hpp"
#include "../AVCore/FrameSequence.hpp"

#include "RFSGenerator.hpp"

namespace SubIT {
    class SbAVTool {
        std::pmr::vector<std::string> args;
    public:
        SbAVTool(int argc, char* argv[]) {
            args.resize(argc);
            std::copy_n(argv, argc, args.begin());
        }

        void PrintHelpMessage() const {
            const char* message = R"(
Copyright © Henry Du @ SubIT 2024. All right reserved.
Part of SubAV SDK, a tool for sbsi, sbws, sbav generation.
Following commands are available now (You should at least have three arguments):

-sig : Follows a image (JPEG, PNG, etc.)  and generate a sbsi file.
-fsg : Follows a video (MP4, MOV etc. This ignore audio track inside) and generate a sbfs file.
-wsg : Follows a audio (MP3, OGG etc.) and generate a sbws file.
-siv : Follows a sbsi image -- view it.
-fsv : Follows a sbfs video -- view it.

)";
            std::cout << message;
        }

        static void GenerateStandalone(std::string_view filename, std::string_view tmp) {
            using namespace std::string_literals;
            // We will create both property description and raw stream. 
            SbFFMpegCommander::YUVCreateStream(filename, tmp);
            SbFFMpegCommander::YUVCreateDesc(filename, tmp);
            SbStandaloneImage image;
            // We will hold all temporary names.
            auto yuvTmpName  = std::string(tmp) + ".yuv"s;
            auto descTmpName = std::string(tmp) + ".txt"s;
            
            std::ifstream input(yuvTmpName, std::ios::binary);
            SbFFMpegCommander::StandaloneFillDescAndAllocate(&image, tmp);
            input.read(reinterpret_cast<char*>(image.data), image.TotalSize());
            
            // Create a standalone image.
            std::ofstream output(std::string(tmp) + ".sbsi"s, std::ios::binary);
            SbSIFactory factory{ &image };
            factory(&output);

            // Clear all temporary files.
            output.close();
            input.close();
            std::filesystem::remove(yuvTmpName);
            std::filesystem::remove(descTmpName);
        }
        
        static void GenerateFrameSequence(std::string_view filename, std::string_view tmp) {
            using namespace std::string_literals;
            SbFFMpegCommander::YUVCreateStream(filename, tmp);
            SbFFMpegCommander::YUVCreateDesc(filename, tmp);

            // Write all information into frame sequence.
            SbFrameSequence sequence;
            uint16_t num = 0, den = 0;
            SbFFMpegCommander::YUVParseDesc(tmp, &sequence.image.width,&sequence.image.height, &num, &den);
            sequence.SetFrameRate(num, den);
            sequence.image.Allocate();
            
            auto yuvTmpName  = std::string(tmp) + ".yuv"s;
            auto descTmpName = std::string(tmp) + ".txt"s;
            std::ifstream input(yuvTmpName, std::ios::binary);
            std::ofstream output(std::string(tmp) + ".sbfs"s, std::ios::binary);

            output.write("SB-AV-FS", 8);
            output.write(reinterpret_cast<const char*>(&sequence.image.width), 8);
            output.write(reinterpret_cast<const char*>(&sequence.image.height), 8);
            output.write(reinterpret_cast<const char*>(&sequence.frameRate), 4);
            
            for (; input.peek() != EOF;) {
                input.read(reinterpret_cast<char*>(sequence.image.data), sequence.image.TotalSize());
                output.write(reinterpret_cast<const char*>(sequence.image.data), sequence.image.TotalSize());
            }
            
            // Clear resources.
            output.close();
            input.close();
            std::filesystem::remove(yuvTmpName);
            std::filesystem::remove(descTmpName);
        }

        static void GenerateWaveSequence(std::string_view filename, std::string_view tmp) {
            std::cout << "Sorry, but this is still work in progress!\n";
        }
        
        void ViewStandalone(std::string_view filename, std::string_view tmp) const {
            std::cout << "Sorry, but this is still work in progress!\n";
        }

        void ViewFrameSequence(std::string_view filename, std::string_view tmp) const {
            std::cout << "Sorry, but this is still work in progress!\n";
        }

        // Execute when command case is equal to 3.
        void OperateFile() const {
            std::string command  = args[1];
            std::string filename = args[2];
            std::string tmp      = filename.substr(0,filename.find_last_of('.'));
            
            if (command == "-sig") { GenerateStandalone(filename, tmp); }
            if (command == "-wsg") { GenerateWaveSequence(filename, tmp); }
            if (command == "-fsg") { GenerateFrameSequence(filename, tmp); }
            if (command == "-siv") { ViewStandalone(filename, tmp); }
            if (command == "-fsv") { ViewFrameSequence(filename, tmp); }
        }

        // App will use this to execute the whole program.
        void operator()() const {
            switch (args.size()) {
            default:
            case 1:
            case 2: PrintHelpMessage(); break;
            case 3: OperateFile(); break;
            }
        }
    };
}

int main(int argc, char* argv[]) {
    SubIT::SbAVTool avToolApplication(argc, argv);
    avToolApplication();
    return 0;
}