///
/// \file      BitStream.hpp
/// \brief     
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <memory_resource>
#include <vector>

namespace SubIT {

    //=========================
    // SbBitBuffer
    //=========================

    class SbBitBuffer {
    public:
        std::pmr::vector<std::uint8_t> bitBuffer;
        std::size_t                    bitPosition;

        SbBitBuffer(std::size_t bitSize);
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