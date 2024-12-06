///
/// \file      StandaloneImage.cpp
/// \brief     Implementation of SBSI.
/// \details   ~
/// \author    HenryDu
/// \date      10.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///

#include <cstring> // for memcpy.
#include <utility> // for std::exchange
#include <cmath>

#include "StandaloneImage.hpp"

#include <iostream>

#include "MaxFOG.hpp"
#include "JPEG.hpp"
#include "DCT.hpp"

namespace SubIT {
    SbStandaloneImage::SbStandaloneImage(std::pmr::polymorphic_allocator<uint8_t> allo) :width(0), height(0), data(nullptr), allocator(allo){}

    SbStandaloneImage::SbStandaloneImage(size_t w, size_t h, std::pmr::polymorphic_allocator<uint8_t> allo) : width(w), height(h), data(nullptr), allocator(allo) {
        Allocate();
    }

    SbStandaloneImage::SbStandaloneImage(const SbStandaloneImage& right) : width(right.width), height(right.height), data(nullptr), allocator(right.allocator) {
        data = allocator.allocate(TotalSize());
        std::memcpy(data, right.data, TotalSize());
    }
    SbStandaloneImage::SbStandaloneImage(SbStandaloneImage&& right) noexcept
    : width    (std::exchange(right.width, 0)),
      height   (std::exchange(right.height, 0)),
      data     (std::exchange(right.data, nullptr)),
      allocator(right.allocator){}

    SbStandaloneImage::~SbStandaloneImage() {
        allocator.deallocate(data, TotalSize());
    }

    size_t SbStandaloneImage::PaddedValue(size_t n, PlaneType p) const {
        return n + ((p + 1) >> 1) * (n & 1);
    }

    size_t SbStandaloneImage::PlaneSize(PlaneType p) const {
        return PaddedValue(width, p) * PaddedValue(height, p) >> (((p + 1) >> 1) << 1);
    }

    uint8_t* SbStandaloneImage::PlaneAt(PlaneType p) const {
        const size_t pp1 = static_cast<size_t>(p) + 1;
        return data + ((pp1 >> 1) * PlaneSize(Luma) + (p >> 1) * PlaneSize(ChromaBlue));
    }

    size_t SbStandaloneImage::PlaneDelta(PlaneType p, size_t off) const {
        return PaddedValue(reinterpret_cast<const size_t*>(this)[off], p) >> ((p + 1) >> 1);
    }

    size_t SbStandaloneImage::TotalSize() const {
        return width * height + ((width + (width & 1)) * (height + (height & 1)) >> 1);
    }

    void SbStandaloneImage::Allocate() {
        data = allocator.allocate(TotalSize());
        std::memset(data, 0, TotalSize());
    }

    void SbStandaloneImage::LossyShrinkBlock8x8(float* beg, size_t rowSize, PlaneType p) {
        SbDCT2 dct(beg, 8, static_cast<ptrdiff_t>(rowSize));
        dct(SbDCT::Forward, SbJPEG::DCTFactorNormal, SbJPEG::DCTFactorOrthogonal);

        const size_t tableIndex = (p + 1) >> 1;
        // All divided by quantize table.
        for (size_t i = 0; i != 8; ++i) {
            beg[i * rowSize + 0] /= SbJPEG::quantizeTables[tableIndex][0 + (i << 3)];
            beg[i * rowSize + 1] /= SbJPEG::quantizeTables[tableIndex][1 + (i << 3)];
            beg[i * rowSize + 2] /= SbJPEG::quantizeTables[tableIndex][2 + (i << 3)];
            beg[i * rowSize + 3] /= SbJPEG::quantizeTables[tableIndex][3 + (i << 3)];
            beg[i * rowSize + 4] /= SbJPEG::quantizeTables[tableIndex][4 + (i << 3)];
            beg[i * rowSize + 5] /= SbJPEG::quantizeTables[tableIndex][5 + (i << 3)];
            beg[i * rowSize + 6] /= SbJPEG::quantizeTables[tableIndex][6 + (i << 3)];
            beg[i * rowSize + 7] /= SbJPEG::quantizeTables[tableIndex][7 + (i << 3)];
        }
    }

    void SbStandaloneImage::LossyShrinkPlane(PlaneType p) {
        const size_t   pWidth  = PlaneDelta(p, 0);
        const size_t   pHeight = PlaneDelta(p, 1);

        // Extenuate the plane, so that it is dividable by 8.
        const size_t  pfWidth  = SbJPEG::floor_to_octuple(pWidth);
        const size_t  pfSize   = 3 * pfWidth * SbJPEG::floor_to_octuple(pHeight) >> 1;
        
        // Temporary float storage.
        float*   plane = allocator.allocate_object<float>(pfSize);
        std::memset(plane, 0, pfSize);
        
        // Initialize it by normalized plane value.
        SbJPEG::copy_block(PlaneAt(p), pWidth, pHeight, pWidth, plane, pfWidth,[](auto v) { return v - 128.F; });

        // Iterate through image and use 8x8 block to do DCT and Quantization.
        for (size_t y = 0; y < pHeight; y += 8) {
            for (size_t x = 0; x < pWidth; x += 8) {
                LossyShrinkBlock8x8(plane + (y * pfWidth + x), pfWidth, p);
            }
        }
        // Copy coefficients back to byte buffer
        SbJPEG::copy_block(plane, pWidth, pHeight, pfWidth, reinterpret_cast<int8_t*>(PlaneAt(p)), pWidth,[](auto v) { return std::roundf(v); });

        allocator.deallocate_object(plane, pfSize);
    }

    void SbStandaloneImage::LossyExpandBlock8x8(float* beg, size_t rowSize, PlaneType p) {
        const size_t tableIndex = (p + 1) >> 1;
        // All divided by quantize table.
        for (size_t i = 0; i != 8; ++i) {
            beg[i * rowSize + 0] *= SbJPEG::quantizeTables[tableIndex][0 + (i << 3)];
            beg[i * rowSize + 1] *= SbJPEG::quantizeTables[tableIndex][1 + (i << 3)];
            beg[i * rowSize + 2] *= SbJPEG::quantizeTables[tableIndex][2 + (i << 3)];
            beg[i * rowSize + 3] *= SbJPEG::quantizeTables[tableIndex][3 + (i << 3)];
            beg[i * rowSize + 4] *= SbJPEG::quantizeTables[tableIndex][4 + (i << 3)];
            beg[i * rowSize + 5] *= SbJPEG::quantizeTables[tableIndex][5 + (i << 3)];
            beg[i * rowSize + 6] *= SbJPEG::quantizeTables[tableIndex][6 + (i << 3)];
            beg[i * rowSize + 7] *= SbJPEG::quantizeTables[tableIndex][7 + (i << 3)];
        }
        SbDCT2 idct(beg, 8, static_cast<ptrdiff_t>(rowSize));
        idct(SbDCT::Inverse, SbJPEG::DCTFactorOrthogonal, SbJPEG::DCTFactorNormal);
    }

    void SbStandaloneImage::LossyExpandPlane(PlaneType p) {
        const size_t   pWidth  = PlaneDelta(p, 0);
        const size_t   pHeight = PlaneDelta(p, 1);

        // Extenuate the plane, so that it is dividable by 8.
        const size_t  pfWidth  = SbJPEG::floor_to_octuple(pWidth);
        const size_t  pfSize   = 3 * pfWidth * SbJPEG::floor_to_octuple(pHeight) >> 1;
        
        // Temporary float storage.
        float* plane = allocator.allocate_object<float>(pfSize);
        std::memset(plane, 0, pfSize);
        
        // Initialize it by normalized plane value.
        SbJPEG::copy_block(reinterpret_cast<int8_t*>(PlaneAt(p)), pWidth, pHeight, pWidth, plane, pfWidth, [](auto v) { return v; });

        // Iterate through image and use 8x8 block to do DCT and Quantization.
        for (size_t y = 0; y < pHeight; y += 8) {
            for (size_t x = 0; x < pWidth; x += 8) {
                LossyExpandBlock8x8(plane + (y * pfWidth + x), pfWidth, p);
            }
        }

        // Copy coefficients back to byte buffer
        SbJPEG::copy_block(plane, pWidth, pHeight, pfWidth, PlaneAt(p), pWidth,[](auto v) { return std::roundf(v) + 128.F; });

        allocator.deallocate_object(plane, pfSize);
    }

    //=====================================
    // Sub Standalone Image Factory.
    //=====================================

    void SbSIFactory::operator()(std::istream* in) {
        // Verify header.
        char header[8] = {};
        in->read(header, 8);
        if (std::memcmp(header, "SB-AV-SI", 8) != 0) {
            throw std::runtime_error("Error, not a valid SBSI format.");
        }
        in->read(reinterpret_cast<char*>(&image->width), 8);
        in->read(reinterpret_cast<char*>(&image->height), 8);

        // Allocate it now.
        image->Allocate();

        // Write data to memory.
        SbCodecMaxFOG::DecodeBits(image->data, SbCodecMaxFOG::GetEncodedBits(in), in);

        image->LossyExpandPlane(SbStandaloneImage::Luma);
        image->LossyExpandPlane(SbStandaloneImage::ChromaBlue);
        image->LossyExpandPlane(SbStandaloneImage::ChromaRed);
    }

    void SbSIFactory::operator()(std::ostream* out) {
        // Write metadata into file stream.
        out->write("SB-AV-SI", 8);
        out->write(reinterpret_cast<char*>(&image->width), 8);
        out->write(reinterpret_cast<char*>(&image->height), 8);

        image->LossyShrinkPlane(SbStandaloneImage::Luma);
        image->LossyShrinkPlane(SbStandaloneImage::ChromaBlue);
        image->LossyShrinkPlane(SbStandaloneImage::ChromaRed);

        // Next is huffman part (all in one).
        SbCodecMaxFOG::EncodeBytes(image->data, image->data + image->TotalSize(), out);
    }

}
