///
/// \file      BitStream.hpp
/// \brief     
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <cstdint>
#include <iosfwd>

namespace SubIT {

    //=========================
    // SbBitBuffer
    //=========================

    class SbBitBuffer {
    public:
        std::uint8_t*                  buf;
        std::size_t                    pos;
        std::size_t                    capacity;

        // The size of this buffer should greater equal than (bitSize + 4) >> 3, we don't sure what would happen if you don't have enough capacity.
        SbBitBuffer(uint8_t* data, std::size_t bitSize);
        SbBitBuffer(const SbBitBuffer&) = default;
        SbBitBuffer(SbBitBuffer&&)      = default;
        SbBitBuffer& operator=(const SbBitBuffer&) = default;
        SbBitBuffer& operator=(SbBitBuffer&&)      = default;
        ~SbBitBuffer() = default;

        void           BitPut(std::size_t pos, std::uint8_t value);
        void           BitGet(std::size_t pos, std::uint8_t& value);
        std::size_t    MaxBit()     const;
        void           Flush();
        bool           IsOverflow() const;
    };

    //=========================
    // SbBitStreams
    //=========================

    class SbOBitStream {
        SbBitBuffer     *mBitBuf;
        std::streambuf  *mStreamBuf;
    public:
        SbOBitStream(SbBitBuffer* bb, std::streambuf* sb);
        SbOBitStream(const SbOBitStream&) = default;
        SbOBitStream(SbOBitStream&&) = default;
        SbOBitStream& operator=(const SbOBitStream&) = default;
        SbOBitStream& operator=(SbOBitStream&&) = default;
        ~SbOBitStream() = default;

        void Push() const;
        void BitPut(std::uint8_t value) const;
    };

    class SbIBitStream {
        SbBitBuffer    *mBitBuf;
        std::streambuf *mStreamBuf;
    public:
        SbIBitStream(SbBitBuffer* bb, std::streambuf* sb);
        SbIBitStream(const SbIBitStream&) = default;
        SbIBitStream(SbIBitStream&&) = default;
        SbIBitStream& operator=(const SbIBitStream&) = default;
        SbIBitStream& operator=(SbIBitStream&&) = default;
        ~SbIBitStream() = default;

        void            Pull() const;
        std::uint8_t    BitGet() const;
    };

}//! namespace SubIT