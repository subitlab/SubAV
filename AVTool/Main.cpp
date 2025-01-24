/// 
/// \file      Main.cpp
/// \author    Henry Du
/// \date      11.16.2024
/// \copyright © Henry Du @ SubIT 2024. All right reserved.
/// 


#include "../AVCore/common.hpp"

#include "../AVCore/RGBA.hpp"
#include "../AVCore/OwlVision.hpp"
#include "../AVCore/MacaqueMixture.hpp"
#include "../AVCore/DolphinAudition.hpp"

#include "PPM.hpp"
#include "FFmpeg.hpp"

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
Copyright © SubIT 2024. All right reserved.

Part of SubAV SDK, a tool for ovc, dac, mmc generation.
Following commands are available (You should at least have three arguments):

-ovg : Follows a image (JPEG, PNG, etc.)  and generate a ovc file.
-dag : Follows a audio (MP3, OGG etc.) and generate a dac file (WIP).
-mmg : Follows a video (MP4, MOV etc.) and generate a MMC file (WIP).
-ovv : Follows a ovc image -- view it.
-dav : Follows a dac audio -- listen to it (WIP).
-mmv : Follows a mmc video -- feel it (WIP).

========================================== Our Team ==========================================
Leading developer: Henry Du     - Implemented Fast DCT + Quantization and this software.
                                - Also designed all logos.
Core developer   : Steve Wang   - Implemented MaxFOG and IKP byte decoder.
Contributors     : Jincheng Xu  - Managed out 4x4, 16x16 and 32x32 dct quantize matrices with AI.
Add your name here as long as you have made contributions.
Addition thanks to Xincheng Hao who inspired us to do this project and created all logos.
==============================================================================================

)";
            std::cout << message;
        }

        static void MakeOVC(std::string_view filename, std::string_view tmp) {
            using namespace std::string_literals;
            // We will create both property description and raw stream. 
            SbFFMpegCommander::YUVCreateStream(filename, tmp);
            SbFFMpegCommander::YUVCreateDesc(filename, tmp);
            SbOwlVisionCoreImage image;
            // We will hold all temporary names.
            auto yuvTmpName  = std::string(tmp) + ".yuv"s;
            auto descTmpName = std::string(tmp) + ".txt"s;
            
            std::ifstream input(yuvTmpName, std::ios::binary);
            SbFFMpegCommander::OwlVisionFillDesc(&image, tmp);
            image.Allocate(::operator new);

            if (!image.SatisfyRestriction()) {
                std::cout << "Error, your data's width and height must all divisible by 16!" << std::endl;
                return;
            }
            input.read(reinterpret_cast<char*>(image.entity), image.size());
            
            // Create a standalone image.
            std::ofstream output(std::string(tmp) + ".ovc"s, std::ios::binary);

            auto start = std::chrono::high_resolution_clock::now();
            SbOwlVisionContainer factory{ &image };
            factory(&output, ::operator new);
            auto stop = std::chrono::high_resolution_clock::now();

            std::cout << "Totoal compression time used: ";
            std::cout << std::format("{}s\n", std::chrono::duration<float>(stop - start).count());

            image.Deallocate(::operator delete);
            // Clear all temporary files.
            output.close();
            input.close();
            std::filesystem::remove(yuvTmpName);
            std::filesystem::remove(descTmpName);
        }
        
        static void MakeMMC(std::string_view filename, std::string_view tmp) {
            using namespace std::string_literals;
            SbFFMpegCommander::YUVCreateStream(filename, tmp);
            SbFFMpegCommander::YUVCreateDesc(filename, tmp);

            // Write all information into frame sequence.
            SbMacaqueMixtureCoreSequence sequence;
            uint16_t num = 0, den = 0;
            SbFFMpegCommander::YUVParseDesc(tmp, &sequence.image.width,&sequence.image.height, &num, &den);
            sequence.SetFrameRate(num, den);
            sequence.image.Allocate(::operator new);
            
            auto yuvTmpName  = std::string(tmp) + ".yuv"s;
            auto descTmpName = std::string(tmp) + ".txt"s;
            std::ifstream input(yuvTmpName, std::ios::binary);
            std::ofstream output(std::string(tmp) + ".mmc"s, std::ios::binary);

            output.write("SBAV-MMC", 8);
            output.write(reinterpret_cast<const char*>(&sequence.image.width), 8);
            output.write(reinterpret_cast<const char*>(&sequence.image.height), 8);
            output.write(reinterpret_cast<const char*>(&sequence.frameRate), 4);
            
            for (; input.peek() != EOF;) {
                input.read(reinterpret_cast<char*>(sequence.image.entity), sequence.image.size());
                output.write(reinterpret_cast<const char*>(sequence.image.entity), sequence.image.size());
            }
            ::operator delete(sequence.image.entity);
            
            // Clear resources.
            output.close();
            input.close();
            std::filesystem::remove(yuvTmpName);
            std::filesystem::remove(descTmpName);
        }

        static void MakeDAC(std::string_view filename, std::string_view tmp) {
            std::cout << "Sorry, but this is still working in progress!\n";
        }

        static void ViewOVC(std::string_view filename, std::string_view tmp) {
            SbOwlVisionCoreImage image;
            SbOwlVisionContainer factory{ &image };
    
            std::ifstream inovc(filename.data(), std::ios::binary);
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            factory(&inovc, ::operator new);
            auto stop = std::chrono::high_resolution_clock::now();
    
            std::cout << "Totoal uncompression time: ";
            std::cout << std::format("{}s\n", std::chrono::duration<float>(stop - start).count());
            std::cout << std::flush;
            
            auto tmpName = std::format("{:s}.yuv", tmp);
            std::ofstream ofs(tmpName, std::ios::binary);
            ofs.write(reinterpret_cast<const char*>(image.entity), image.size());
            ofs.close();
            SbFFMpegCommander::OwlVisionDisplay(&image, tmpName);

            image.Deallocate(::operator delete);
            std::filesystem::remove(tmpName);
        }

        static void ViewDAC(std::string_view filename, std::string_view tmp) {
            std::cout << "Sorry, but this is still working in progress!\n";
        }

        static void ViewMMC(std::string_view filename, std::string_view tmp) {
            std::cout << "Sorry, but this is still working in progress!\n";
        }

        static void MakePPM(std::string_view filename, std::string_view tmp) {
            SbOwlVisionCoreImage image;
            SbOwlVisionContainer factory{ &image };

            std::ifstream inovc(filename.data(), std::ios::binary);
            std::ofstream oppm(std::format("{:s}.ppm", tmp));

            factory(&inovc, ::operator new);

            SbRGB rgb{ &image };
            rgb(reinterpret_cast<uint8_t*>(image.shadow));
            SbPPM ppm{ reinterpret_cast<uint8_t*>(image.shadow), image.width, image.height };
            ppm(&oppm);

            std::cout << "Conversion complete!" << std::endl;

            inovc.close();
            oppm .close();
            image.Deallocate(::operator delete);
        }

        // Execute when command case is equal to 3.
        void OperateFile() const {
            std::string command  = args[1];
            std::string filename = args[2];
            std::string tmp      = filename.substr(0,filename.find_last_of('.'));

            if (command == "-ovg")    { MakeOVC(filename, tmp); return; }
            if (command == "-dag")    { MakeDAC(filename, tmp); return; }
            if (command == "-mmg")    { MakeMMC(filename, tmp); return; }
            if (command == "-ovppm")  { MakePPM(filename, tmp); return; }
            if (command == "-ovv")    { ViewOVC(filename, tmp); return; }
            if (command == "-dav")    { ViewDAC(filename, tmp); return; }
            if (command == "-mmv")    { ViewMMC(filename, tmp); return; }

            std::cout << "Error, invalid arguments, please check help messages." << std::endl;
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
    SubIT::SbAVTool avTool(argc, argv);
    avTool();
    return 0;
}
