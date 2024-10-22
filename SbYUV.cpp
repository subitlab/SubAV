///
/// \file      SbYUV.cpp
/// \brief     Implementation of SbYUV format.
/// \details   ~
/// \author    SubIT
/// \date      19.10.2024
/// \copyright © SubIT 2024. All right reserved.
///


#include "SbYUV.hpp"
#include <cstring>

namespace Sb::Video {

    uint32_t YUV_MakeFrameRate(uint16_t num, uint16_t den) {
        const uint32_t quotient = static_cast<uint32_t>(num << 16) | static_cast<uint32_t>(den);
        return quotient;
    }

    bool     YUV_VerifyMagic(std::istream* input) {
        input->seekg(std::ios::beg);
        char magic[16] = {};
        input->read(magic, 16);
        int  declareResult = std::memcmp(magic + 0, YUV_Descriptor::magicDeclareTag, 9);
        int  versionResult = std::memcmp(magic + 9, "-V-", 3);
        bool isVersion     = std::isdigit(magic[12]) && std::isdigit(magic[13]) && magic[14] == '-' && std::isdigit(magic[15]);
        return (declareResult == 0 && versionResult == 0 && isVersion);
    }

    uint32_t YUV_GetDescriptorAndPrepare(std::istream* input, YUV_Descriptor* desc) {
        // 16 is the total fixed size of two magic strings.
        input->seekg(16, std::ios::beg);

        input->read(reinterpret_cast<char*>(&desc->width), sizeof(uint32_t));
        input->read(reinterpret_cast<char*>(&desc->height), sizeof(uint32_t));
        input->read(reinterpret_cast<char*>(&desc->frameRate), sizeof(uint32_t));
        input->read(reinterpret_cast<char*>(&desc->extraFlag), sizeof(uint32_t));
        return 0;
    }

    uint32_t YUV_PutMagic(std::ostream* output) {
        output->seekp(std::ios::beg);

        output->write(reinterpret_cast<const char*>(YUV_Descriptor::magicDeclareTag), sizeof(YUV_Descriptor::magicDeclareTag));
        output->write(reinterpret_cast<const char*>(YUV_Descriptor::magicVersionTag), sizeof(YUV_Descriptor::magicVersionTag));
        return 0;
    }

    uint32_t YUV_PutDescriptorAndPrepare(std::ostream* output, const YUV_Descriptor* desc) {
        output->seekp(16, std::ios::beg);

        output->write(reinterpret_cast<const char*>(&desc->width), sizeof(uint32_t));
        output->write(reinterpret_cast<const char*>(&desc->height), sizeof(uint32_t));
        output->write(reinterpret_cast<const char*>(&desc->frameRate), sizeof(uint32_t));
        output->write(reinterpret_cast<const char*>(&desc->extraFlag), sizeof(uint32_t));
        return 0;
    }

    size_t YUV_GetFrameBufferSize(const YUV_Descriptor* desc) {
        // A classic yuv420p size.
        return 3 * desc->width * desc->height >> 1;
    }

    float YUV_GetFrequency(const YUV_Descriptor* desc) {
        return static_cast<float>((desc->frameRate >> 0) & 0xFFFF) / static_cast<float>((desc->frameRate >> 16) & 0xFFFF);
    }

    //===================================================
    // SbYUV stream IO functionalities
    //===================================================

    size_t YUV_FromDescriptorAndRawStream(std::ostream* output, const YUV_Descriptor* inputDesc, std::istream* input) {

        // A yuv420p frame buffer.
        const size_t frameBufferSize = YUV_GetFrameBufferSize(inputDesc);
        auto*        frameBuffer     = static_cast<std::byte*>(::operator new(frameBufferSize));

        YUV_PutMagic(output);
        YUV_PutDescriptorAndPrepare(output, inputDesc);

        size_t frameCount = 0;
        // Write yuv datas.
        for (; input->peek() != -1; ++frameCount) {
            input ->read (reinterpret_cast<char*>(frameBuffer), static_cast<std::streamsize>(frameBufferSize));
            output->write(reinterpret_cast<char*>(frameBuffer), static_cast<std::streamsize>(frameBufferSize));
        }
        ::operator delete(frameBuffer);

        return frameCount;
    }

    size_t YUV_ToDescriptorAndRawStream(std::istream* input, YUV_Descriptor* outputDesc, std::ostream* output) {

        if (!YUV_VerifyMagic(input)) {
            return 1;
        }
        YUV_GetDescriptorAndPrepare(input, outputDesc);

        const size_t frameBufferSize = YUV_GetFrameBufferSize(outputDesc);
        auto*        frameBuffer     = static_cast<uint8_t*>(::operator new(frameBufferSize));

        size_t frameCount = 0;
        for (; input->peek() != -1 ;++frameCount) {
            input -> read(reinterpret_cast<char*>(frameBuffer), static_cast<std::streamsize>(frameBufferSize));
            output->write(reinterpret_cast<char*>(frameBuffer), static_cast<std::streamsize>(frameBufferSize));
        }
        ::operator delete(frameBuffer);

        return frameCount;
    }

}
