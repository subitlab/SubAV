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
#include <cstring>
#include "BitStream.hpp"

namespace SubIT {
    SbBitBuffer::SbBitBuffer(uint8_t* data, std::size_t bitSize) : buf(data), pos(0), capacity(bitSize >> 3) {}

    void SbBitBuffer::BitPut(std::size_t pos, std::uint8_t value) {
        const std::size_t bytePos = pos >> 3;
        const std::size_t bitPos = std::endian::native == std::endian::big ? (pos & 0x07) : (0x07 - (pos & 0x07));
        const bool bitData = value;
        buf[bytePos] |= std::endian::native == std::endian::big ? (bitData >> bitPos) : (bitData << bitPos);
    }

    void SbBitBuffer::BitGet(std::size_t pos, std::uint8_t& value) {
        const std::size_t bytePos = pos >> 3;
        const std::size_t bitPos = std::endian::native == std::endian::big ? (pos & 0x07) : (0x07 - (pos & 0x07));
        // This part is necessary since bool will normalize all numbers differs from 0 to 1.
        const bool bitData = buf[bytePos] & (std::endian::native == std::endian::big ? (1 >> bitPos) : (1 << bitPos));
        value = bitData;
    }

    std::size_t SbBitBuffer::MaxBit() const {
        return capacity << 3;
    }

    void SbBitBuffer::Flush() {
        pos = 0;
        std::memset(buf, 0, MaxBit() >> 3);
    }

    bool SbBitBuffer::IsOverflow() const {
        return pos > MaxBit() - 1;
    }

    SbOBitStream::SbOBitStream(SbBitBuffer* bb, std::streambuf* sb) : mBitBuf(bb), mStreamBuf(sb) {}

    void SbOBitStream::Push() const {
        const std::size_t bytes = mBitBuf->pos >> 3;
        const std::size_t remain = static_cast<bool>(mBitBuf->pos & 0x07);
        mStreamBuf->sputn(reinterpret_cast<const char*>(mBitBuf->buf), static_cast<std::streamsize>(bytes + remain));
    }

    void SbOBitStream::BitPut(std::uint8_t value) const {
        if (mBitBuf->IsOverflow()) {
            Push();
            mBitBuf->Flush();
        }
        mBitBuf->BitPut(mBitBuf->pos, value);
        ++mBitBuf->pos;
    }

    SbIBitStream::SbIBitStream(SbBitBuffer* bb, std::streambuf* sb) : mBitBuf(bb), mStreamBuf(sb) {}

    void SbIBitStream::Pull() const {
        const std::streampos cur = mStreamBuf->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
        const std::streampos end = mStreamBuf->pubseekoff(0, std::ios_base::end, std::ios_base::in);
        mStreamBuf->pubseekpos(cur, std::ios_base::in);
        const std::size_t bytes = (end - cur);
        const std::size_t count = bytes >= (mBitBuf->capacity) ? (mBitBuf->capacity) : bytes % (mBitBuf->capacity);
        mStreamBuf->sgetn(reinterpret_cast<char*>(mBitBuf->buf), static_cast<std::streamsize>(count));
    }

    std::uint8_t SbIBitStream::BitGet() const {
        if (mBitBuf->IsOverflow()) {
            mBitBuf->Flush();
            Pull();
        }
        std::uint8_t value;
        mBitBuf->BitGet(mBitBuf->pos, value);
        ++mBitBuf->pos;
        return value;
    }
}
