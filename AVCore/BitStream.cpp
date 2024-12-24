///
/// \file      BitStream.cpp
/// \brief     
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#include <bit>
#include <streambuf>
#include "BitStream.hpp"
#include <cstring>

namespace SubIT {
    SbBitBuffer::SbBitBuffer(std::size_t bitSize) : bitBuffer(bitSize >> 3, 0), bitPosition(0) {}

    void SbBitBuffer::BitPut(std::size_t pos, std::uint8_t value) {
        const std::size_t bytePos = pos >> 3;
        const std::size_t bitPos = std::endian::native == std::endian::big ? (pos & 0x07) : (0x07 - (pos & 0x07));
        const bool bitData = value;
        bitBuffer[bytePos] |= std::endian::native == std::endian::big ? (bitData >> bitPos) : (bitData << bitPos);
    }

    void SbBitBuffer::BitGet(std::size_t pos, std::uint8_t& value) {
        const std::size_t bytePos = pos >> 3;
        const std::size_t bitPos = std::endian::native == std::endian::big ? (pos & 0x07) : (0x07 - (pos & 0x07));
        // This part is necessary since bool will normalize all numbers differs from 0 to 1.
        const bool bitData = bitBuffer[bytePos] & (std::endian::native == std::endian::big ? (1 >> bitPos) : (1 << bitPos));
        value = bitData;
    }

    std::size_t SbBitBuffer::MaxBit() const {
        return bitBuffer.size() << 3;
    }

    void SbBitBuffer::Flush() {
        bitPosition = 0;
        memset(bitBuffer.data(), 0, MaxBit() >> 3);
    }

    bool SbBitBuffer::IsOverflow() const {
        return bitPosition > MaxBit() - 1;
    }

    SbOBitStream::SbOBitStream(SbBitBuffer* bb, std::streambuf* sb) : mBitBuf(bb), mStreamBuf(sb) {}

    void SbOBitStream::Push() const {
        const std::size_t bytes = mBitBuf->bitPosition >> 3;
        const std::size_t remain = static_cast<bool>(mBitBuf->bitPosition & 0x07);
        mStreamBuf->sputn(reinterpret_cast<const char*>(mBitBuf->bitBuffer.data()), static_cast<std::streamsize>(bytes + remain));
    }

    void SbOBitStream::BitPut(std::uint8_t value) const {
        if (mBitBuf->IsOverflow()) {
            Push();
            mBitBuf->Flush();
        }
        mBitBuf->BitPut(mBitBuf->bitPosition, value);
        ++mBitBuf->bitPosition;
    }

    SbIBitStream::SbIBitStream(SbBitBuffer* bb, std::streambuf* sb) : mBitBuf(bb), mStreamBuf(sb) {}

    void SbIBitStream::Pull() const {
        const std::streampos cur = mStreamBuf->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
        const std::streampos end = mStreamBuf->pubseekoff(0, std::ios_base::end, std::ios_base::in);
        mStreamBuf->pubseekpos(cur, std::ios_base::in);
        const std::size_t bytes = (end - cur);
        const std::size_t count = bytes >= (mBitBuf->bitBuffer.size()) ? (mBitBuf->bitBuffer.size()) : bytes % (mBitBuf->bitBuffer.size());
        mStreamBuf->sgetn(reinterpret_cast<char*>(mBitBuf->bitBuffer.data()), static_cast<std::streamsize>(count));
    }

    std::uint8_t SbIBitStream::BitGet() const {
        if (mBitBuf->IsOverflow()) {
            mBitBuf->Flush();
            Pull();
        }
        std::uint8_t value;
        mBitBuf->BitGet(mBitBuf->bitPosition, value);
        ++mBitBuf->bitPosition;
        return value;
    }
}
