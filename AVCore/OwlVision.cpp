///
/// \file      OwlVision.cpp
/// \brief     Implementation of SBSI.
/// \details   ~
/// \author    HenryDu
/// \date      10.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include <cstring> // for memcpy.
#include <utility> // for std::exchange
#include <istream>
#include <ostream>
#include <iostream> // for quantization debug output.
#include <future> // for std::async.

#include "OwlVision.hpp"
#include "MaxFOG.hpp"
#include "DCT.hpp"
#include "SIMD.hpp"

namespace SubIT {

    SbOwlVisionCoreImage::SbOwlVisionCoreImage(size_t w, size_t h) : width(w), height(h), data(nullptr), shadow(nullptr) {}

    SbOwlVisionCoreImage::SbOwlVisionCoreImage(const SbOwlVisionCoreImage& right) : width(right.width), height(right.height), data(nullptr), shadow(right.shadow) {
        // We will not copy shadow since it's only for transforming.
        std::memcpy(data, right.data, size());
    }
    SbOwlVisionCoreImage::SbOwlVisionCoreImage(SbOwlVisionCoreImage&& right) noexcept
    : width    (std::exchange(right.width, 0)),
      height   (std::exchange(right.height, 0)),
      data     (std::exchange(right.data, nullptr)),
      shadow   (std::exchange(right.shadow, nullptr)){}

    bool SbOwlVisionCoreImage::SatisfyRestriction() const {
        return !(width & 0xF || height & 0xF);
    }

    size_t SbOwlVisionCoreImage::size() const {
        return (width * height * 3) >> 1;
    }

    void SbOwlVisionCoreImage::Allocate(void*(* alloc)(size_t size)) {
        data   = static_cast<uint8_t*>(alloc(size()));
        shadow = static_cast<float*>(alloc(size() * sizeof(float)));
    }

    void SbOwlVisionCoreImage::Deallocate(void dealloc(void*)) {
        dealloc(data);
        dealloc(shadow);
    }

    void SbOwlVisionCoreImage::InitShadowOperationPipelineInfo(PlaneType p, ShadowOperationPipelineInfo* pi) const {
        const size_t wh = width * height;
        pi->id     = (p + 1) >> 1;
        pi->size   = wh >> (pi->id << 1);
        pi->offset = (pi->id) * (wh) + (p >> 1) * (wh >> 2);
        pi->width  = width  >> pi->id;
        pi->height = height >> pi->id;
    }

    template void SbOwlVisionCoreImage::ShadowTransformAndQuantize<SbDCT::dirForward>(const ShadowOperationPipelineInfo& pp);
    template void SbOwlVisionCoreImage::ShadowTransformAndQuantize<SbDCT::dirInverse>(const ShadowOperationPipelineInfo& pp);

    static constexpr float sShadowNormalBias[4] = {128.F, 128.F, 128.F, 128.F};
    
    template <bool dir>
    void SbOwlVisionCoreImage::ShadowReceieveData(const ShadowOperationPipelineInfo& pi) {
        for (size_t i = 0; i != pi.size; i += 4) {
            if constexpr (dir == SbDCT::dirForward) {
                (shadow + pi.offset)[i + 0] =  static_cast<float>((data + pi.offset)[i + 0]);
                (shadow + pi.offset)[i + 1] =  static_cast<float>((data + pi.offset)[i + 1]);
                (shadow + pi.offset)[i + 2] =  static_cast<float>((data + pi.offset)[i + 2]);
                (shadow + pi.offset)[i + 3] =  static_cast<float>((data + pi.offset)[i + 3]);
                SbSIMD::SubA4(shadow + pi.offset + i, sShadowNormalBias);
            }
            else if constexpr (dir == SbDCT::dirInverse) {
                (shadow + pi.offset)[i + 0] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pi.offset)[i + 0]);
                (shadow + pi.offset)[i + 1] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pi.offset)[i + 1]);
                (shadow + pi.offset)[i + 2] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pi.offset)[i + 2]);
                (shadow + pi.offset)[i + 3] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pi.offset)[i + 3]);
            }
        } // for
    }

    template <bool dir>
    void SbOwlVisionCoreImage::ShadowTransformAndQuantize(const ShadowOperationPipelineInfo& pi) {
        for (size_t y = 0; y != pi.height; y += 32) {
            for (size_t x = 0; x != pi.width; x += 32) {
                SbDCT2 transformer(shadow + pi.offset + (y * pi.width + x), pi.width);
                if constexpr (dir == SbDCT::dirForward) {
                    transformer.Transform32x32<SbDCT::dirForward>();
                    transformer.Quantize32x32<SbDCT::dirForward>(SbOwlVisionConstants::QM32x32[pi.id]);
                    // Debug usage
                    // In future development, if there are any problems in image, use this to find out. 
                    // for (int i = 0; i != 32; ++i) {
                    //     for (int j = 0; j != 32; ++j) {
                    //         if (auto v = (shadow + pi.offset + (y * pi.width + x))[i * 32 + j]; (v < -128 || v > 127) && pi.id == PlaneType::Luma) {
                    //             std::cout << "(" << j << ", " << i << ", " << v << ")" << std::endl;
                    //         }
                    //     }
                    // }
                }
                else if constexpr (dir == SbDCT::dirInverse) {
                    transformer.Quantize32x32<SbDCT::dirInverse>(SbOwlVisionConstants::QM32x32[pi.id]);
                    transformer.Transform32x32<SbDCT::dirInverse>();
                }
            }
        }
    }

    template <bool dir>
    void SbOwlVisionCoreImage::ShadowReturnData(const ShadowOperationPipelineInfo& pi) {
        for (size_t i = 0; i != pi.size; i += 4) {
            if constexpr (dir == SbDCT::dirForward) {
                SbSIMD::F2I4(shadow + pi.offset + i);
                reinterpret_cast<int8_t*>(data + pi.offset)[i + 0] = static_cast<int8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 0)));
                reinterpret_cast<int8_t*>(data + pi.offset)[i + 1] = static_cast<int8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 1)));
                reinterpret_cast<int8_t*>(data + pi.offset)[i + 2] = static_cast<int8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 2)));
                reinterpret_cast<int8_t*>(data + pi.offset)[i + 3] = static_cast<int8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 3)));
            }
            else if constexpr (dir == SbDCT::dirInverse) {
                SbSIMD::AddA4(shadow + pi.offset + i, sShadowNormalBias);
                SbSIMD::F2I4 (shadow + pi.offset + i);
                (data + pi.offset)[i + 0] =  static_cast<uint8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 0)));
                (data + pi.offset)[i + 1] =  static_cast<uint8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 1)));
                (data + pi.offset)[i + 2] =  static_cast<uint8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 2)));
                (data + pi.offset)[i + 3] =  static_cast<uint8_t>(*reinterpret_cast<int*>(shadow + pi.offset + (i + 3)));
            }
        } // for
    }

    template <bool dir>
    static inline auto StartAndExecuteFixedPipeline(SbOwlVisionCoreImage* image, SbOwlVisionCoreImage::PlaneType plane) {
        SbOwlVisionCoreImage::ShadowOperationPipelineInfo pi;
        std::invoke(&SbOwlVisionCoreImage::InitShadowOperationPipelineInfo, image, plane, &pi);
        // Standard pipeline stages.
        std::invoke(&SbOwlVisionCoreImage::ShadowReceieveData<dir>, image, pi);
        std::invoke(&SbOwlVisionCoreImage::ShadowTransformAndQuantize<dir>, image, pi);
        std::invoke(&SbOwlVisionCoreImage::ShadowReturnData<dir>, image, pi);
    }
    
    void SbOwlVisionContainer::operator()(std::istream* in, void*(*alloc)(size_t)) {
        // Verify header.
        char header[8] = {};
        in->read(header, 8);
        if (std::memcmp(header, "SBAV-OVC", 8) != 0) {
            throw std::runtime_error("Error: invalid ovc file.");
        }
        in->read(reinterpret_cast<char*>(&image->width), 8);
        in->read(reinterpret_cast<char*>(&image->height), 8);

        // Allocate it now.
        image->Allocate(alloc);

        // Write data to memory.
        SbCodecMaxFOG::DecodeBits(image->data, SbCodecMaxFOG::GetEncodedBits(in), in);

        // Multi thread optimization.
        auto f0 = std::async(std::launch::async, StartAndExecuteFixedPipeline<SbDCT::dirInverse>, image, SbOwlVisionCoreImage::Luma);
        auto f1 = std::async(std::launch::async, StartAndExecuteFixedPipeline<SbDCT::dirInverse>, image, SbOwlVisionCoreImage::ChromaBlue);
        auto f2 = std::async(std::launch::async, StartAndExecuteFixedPipeline<SbDCT::dirInverse>, image, SbOwlVisionCoreImage::ChromaRed);
    }

    void SbOwlVisionContainer::operator()(std::ostream* out, void*(*alloc)(size_t)) {
        // Write metadata into file stream.
        out->write("SBAV-OVC", 8);
        out->write(reinterpret_cast<char*>(&image->width), 8);
        out->write(reinterpret_cast<char*>(&image->height), 8);
        
        // I don't know why async doesn't work for this part, it should work I mean.
        std::invoke(StartAndExecuteFixedPipeline<SbDCT::dirForward>, image, SbOwlVisionCoreImage::Luma);
        std::invoke(StartAndExecuteFixedPipeline<SbDCT::dirForward>, image, SbOwlVisionCoreImage::ChromaBlue);
        std::invoke(StartAndExecuteFixedPipeline<SbDCT::dirForward>, image, SbOwlVisionCoreImage::ChromaRed);

        // Next is huffman part (all in one).
        SbCodecMaxFOG::EncodeBytes(image->data, image->data + image->size(), out);
    }

}
