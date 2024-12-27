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
#include <future> // for std::async.
#include <cmath>

#include "OwlVision.hpp"
#include "MaxFOG.hpp"
#include "DCT.hpp"

namespace SubIT {

    SbOwlVisionCoreImage::SbOwlVisionCoreImage(size_t w, size_t h) : width(w), height(h), data(nullptr) {}

    SbOwlVisionCoreImage::SbOwlVisionCoreImage(const SbOwlVisionCoreImage& right) : width(right.width), height(right.height), data(nullptr) {
        std::memcpy(data, right.data, TotalSize());
    }
    SbOwlVisionCoreImage::SbOwlVisionCoreImage(SbOwlVisionCoreImage&& right) noexcept
    : width    (std::exchange(right.width, 0)),
      height   (std::exchange(right.height, 0)),
      data     (std::exchange(right.data, nullptr)) {}

    bool SbOwlVisionCoreImage::SatisfyRestriction() const {
        return !(width & 0xF || height & 0xF);
    }

    size_t SbOwlVisionCoreImage::PlaneSize(PlaneType p) const {
        return width * height >> (((p + 1) >> 1) << 1);
    }

    size_t SbOwlVisionCoreImage::PlaneOffset(PlaneType p) const {
        return ((static_cast<size_t>(p) + 1) >> 1) * PlaneSize(Luma) + (p >> 1) * PlaneSize(ChromaBlue);
    }

    size_t SbOwlVisionCoreImage::PlaneDelta(PlaneType p, size_t off) const {
        return  (reinterpret_cast<const size_t*>(this)[off]) >> ((p + 1) >> 1);
    }

    size_t SbOwlVisionCoreImage::TotalSize() const {
        return (width * height * 3) >> 1;
    }

    void SbOwlVisionCoreImage::Allocate(void*(* alloc)(size_t size)) {
        data = static_cast<uint8_t*>(alloc(TotalSize()));
    }

    void SbOwlVisionCoreImage::TransformAndQuantizeRowTask8x8(const bool dir, const size_t pTabId, float* beg,
        const size_t rowSize) {
         SbDCT2 transformer(beg, rowSize);
         switch (dir) {
         case SbDCT::dirForward:
             transformer.Transform8x8(SbDCT::dirForward);
             transformer.Quantize8x8(SbOwlVisionConstants::quantTablesFwd[pTabId]); break;
         case SbDCT::dirInverse:
             transformer.Quantize8x8(SbOwlVisionConstants::quantTablesInv8x8[pTabId]);
             transformer.Transform8x8(SbDCT::dirInverse); break;
         }
    }

    void SbOwlVisionCoreImage::TransformAndQuantizePlane8x8(const bool dir, PlaneType p, float* plane) {
        const size_t   pWidth  = PlaneDelta(p, 0);
        const size_t   pHeight = PlaneDelta(p, 1);
        const size_t   pOffset = PlaneOffset(p);
        const size_t   pSize   = PlaneSize(p);
        const size_t   pTabId  = (p + 1) >> 1;

        for (size_t i = 0; i != pSize; i += 4) {
            switch (dir) {
            case SbDCT::dirForward:
                plane[i + 0] =  static_cast<float>((data + pOffset)[i + 0]) - 128.F;
                plane[i + 1] =  static_cast<float>((data + pOffset)[i + 1]) - 128.F;
                plane[i + 2] =  static_cast<float>((data + pOffset)[i + 2]) - 128.F;
                plane[i + 3] =  static_cast<float>((data + pOffset)[i + 3]) - 128.F; break;
            case SbDCT::dirInverse:
                plane[i + 0] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pOffset)[i + 0]);
                plane[i + 1] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pOffset)[i + 1]);
                plane[i + 2] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pOffset)[i + 2]);
                plane[i + 3] = static_cast<float>(reinterpret_cast<int8_t*>(data +  pOffset)[i + 3]); break;
            }
        }

        for (size_t y = 0; y < pHeight; y += 8) {
            for (size_t x = 0; x < pWidth; x += 8) {
                TransformAndQuantizeRowTask8x8(dir, pTabId, plane + (y * pWidth + x), pWidth);
            }
        }

        for (size_t i = 0; i != pSize; i += 4) {
            switch (dir) {
            case SbDCT::dirForward:
                reinterpret_cast<int8_t*>(data + pOffset)[i + 0] = static_cast<int8_t>(std::roundf(plane[i + 0]));
                reinterpret_cast<int8_t*>(data + pOffset)[i + 1] = static_cast<int8_t>(std::roundf(plane[i + 1]));
                reinterpret_cast<int8_t*>(data + pOffset)[i + 2] = static_cast<int8_t>(std::roundf(plane[i + 2]));
                reinterpret_cast<int8_t*>(data + pOffset)[i + 3] = static_cast<int8_t>(std::roundf(plane[i + 3])); break;
            case SbDCT::dirInverse:
                (data + pOffset)[i + 0] =  static_cast<uint8_t>(std::roundf(plane[i + 0]) + 128.F);
                (data + pOffset)[i + 1] =  static_cast<uint8_t>(std::roundf(plane[i + 1]) + 128.F);
                (data + pOffset)[i + 2] =  static_cast<uint8_t>(std::roundf(plane[i + 2]) + 128.F);
                (data + pOffset)[i + 3] =  static_cast<uint8_t>(std::roundf(plane[i + 3]) + 128.F); break;
            }
        }
        
    }

    float* SbOwlVisionContainer::operator()(std::istream* in, void*(*alloc)(size_t)) {
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
        float* buffer = static_cast<float*>(alloc(sizeof(float) * image->width * image->height));
        
        // Write data to memory.
        SbCodecMaxFOG::DecodeBits(image->data, SbCodecMaxFOG::GetEncodedBits(in), in);

        image->TransformAndQuantizePlane8x8(SbDCT::dirInverse, SbOwlVisionCoreImage::Luma, buffer);
        image->TransformAndQuantizePlane8x8(SbDCT::dirInverse, SbOwlVisionCoreImage::ChromaBlue, buffer);
        image->TransformAndQuantizePlane8x8(SbDCT::dirInverse, SbOwlVisionCoreImage::ChromaRed, buffer);
        return buffer;
    }

    float* SbOwlVisionContainer::operator()(std::ostream* out, void*(*alloc)(size_t)) {
        // Write metadata into file stream.
        out->write("SBAV-OVC", 8);
        out->write(reinterpret_cast<char*>(&image->width), 8);
        out->write(reinterpret_cast<char*>(&image->height), 8);

        float* buffer = static_cast<float*>(alloc(sizeof(float) * image->width * image->height));
        
        image->TransformAndQuantizePlane8x8(SbDCT::dirForward, SbOwlVisionCoreImage::Luma, buffer);
        image->TransformAndQuantizePlane8x8(SbDCT::dirForward, SbOwlVisionCoreImage::ChromaBlue, buffer);
        image->TransformAndQuantizePlane8x8(SbDCT::dirForward, SbOwlVisionCoreImage::ChromaRed, buffer);

        // Next is huffman part (all in one).
        SbCodecMaxFOG::EncodeBytes(image->data, image->data + image->TotalSize(), out);
        return buffer;
    }

}
