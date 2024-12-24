///
/// \file      MaxFOG.cpp
/// \brief     A MaxFOG codec implementation.
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#include <algorithm> // for std::sort
#include <ios>
#include <ostream>
#include <istream>

#include "BitStream.hpp"
#include "MaxFOG.hpp"

#include <iostream>

#include <cstddef>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

namespace ikp {
    ByteDecoder::ByteDecoder(const unsigned char *freqs, const unsigned char totalCount) {
        int moffset = 0;
#ifdef _WIN32
        //__builtin_printf("Win32\n");
        char *nmem = (char *)VirtualAlloc(nullptr, funsiz = (17+0x31+((int)((totalCount - 3)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e)+0x2e), MEM_COMMIT, PAGE_READWRITE);
        const unsigned char win_workaround[17] = {0x57, 0x56,
                                                 0x41, 0x54,
                                                 0x41, 0x55,
                                                 0x41, 0x56,
                                                 0x41, 0x57,
                                                 0x53,
                                                 0x48, 0x89, 0xcf, 0x48, 0x89, 0xd6};
        memcpy(nmem, win_workaround, 17);
        moffset = 17;
        const unsigned long long int returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#else
        char *nmem = (char *)mmap(NULL, funsiz = (0x31+((int)((totalCount - 3)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e)+0x23), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        const unsigned long long int returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#endif
        {
            const unsigned long long int joffset = returnAddr - 0x31;
            const unsigned char header[0x31] = {0x41, 0x54,
                0x41, 0x55,
                0x41, 0x56,
                0x41, 0x57,
                0x55, 0x48, 0x89, 0xe5, 0x49, 0x89, 0xff, 0x4c, 0x8b, 0x27, 0x49, 0x89, 0xf5, 0x4d, 0x8b, 0x75, 0x00, 0x41, 0x0f, 0xb6, 0x14, 0x24, 0x4c, 0x89, 0xf0, 0x21, 0xd0, 0x84, 0xc0, 0x75, 0x0a, 0xb8, 0, 0, 0, 0, 0xe9, (unsigned char)joffset, (unsigned char)(joffset >> 8), (unsigned char)(joffset >> 16), (unsigned char)(joffset >> 24)};
            memcpy(nmem+moffset, header, 0x31);
            moffset += 0x31;
        }
        for(int i = 0; i <= totalCount-3; i+=2) {
#ifdef _WIN32
            const unsigned long long int joffset1 = returnAddr - 0x4e - moffset+17,
                                         joffset2 = returnAddr - 0x58 - moffset+17;
#else
            const unsigned long long int joffset1 = returnAddr - 0x4e - moffset,
                                         joffset2 = returnAddr - 0x58 - moffset;
#endif
            const unsigned char blockdata[0x58] = {
                0x41, 0xd0, 0xee,
                0x4c, 0x89, 0xf0,
                0x84, 0xc0,
                0x75, 0x0b,
                0x49, 0xc7, 0xc6, 0x80, 0, 0, 0,
                0x49, 0x83, 0xc4, 0x01,
                0x49, 0x8b, 0x14, 0x24,
                0x4c, 0x89, 0xf0,
                0x21, 0xd0,
                0x84, 0xc0,
                0x75, 0x36,
                0x41, 0xd0, 0xee,
                0x4c, 0x89, 0xf0,
                0x84, 0xc0,
                0x75, 0x0b,
                0x49, 0xc7, 0xc6, 0x80, 0, 0, 0,
                0x49, 0x83, 0xc4, 0x01,
                0x49, 0x8b, 0x14, 0x24,
                0x4c, 0x89, 0xf0,
                0x21, 0xd0,
                0x84, 0xc0,
                0x75, 0x0a,
                0xb8, freqs[i], 0, 0, 0,
                0xe9, (unsigned char)joffset1, (unsigned char)(joffset1>>8), (unsigned char)(joffset1>>16), (unsigned char)(joffset1>>24),
                0xb8, freqs[i+1], 0, 0, 0,
                0xe9, (unsigned char)joffset2, (unsigned char)(joffset2>>8), (unsigned char)(joffset2>>16), (unsigned char)(joffset2>>24)
            };
            memcpy(nmem+moffset, blockdata, 0x58);
            moffset += 0x58;
        }
        if(totalCount & 1) {
            const unsigned char lastpar[5] = {0xb8, freqs[totalCount-1], 0, 0, 0};
            memcpy(nmem+moffset, lastpar, 5);
            moffset += 5;
        }
        else {
            const unsigned char lastpar[0x2e] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf0, 0x84, 0xc0, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x49, 0x8b, 0x14, 0x24, 0x4c, 0x89, 0xf0, 0x21, 0xd0, 0x84, 0xc0, 0x75, 0x07, 0xb8, freqs[totalCount-2], 0, 0, 0, 0xeb, 0x05, 0xb8, freqs[totalCount-1], 0, 0, 0};
            memcpy(nmem+moffset, lastpar, 0x2e);
            moffset += 0x2e;
        }
#ifdef _WIN32
      //const unsigned char returning[0x2e] = {0x41, 0xd0, 0xe6, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x01, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x4c, 0x89, 0xeb, 0x44, 0x88, 0x33, 0x4c, 0x89, 0xfb, 0x4c, 0x89, 0x23, 0x5d,
        const unsigned char returning[0x23] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x45, 0x88, 0x75, 0, 0x4d, 0x89, 0x27, 0x5d, 0xc3,
        0x5b,
        0x41, 0x5f,
        0x41, 0x5e,
        0x41, 0x5d,
        0x41, 0x5c,
        0x5e, 0x5f, 0xc3};
        memcpy(nmem+moffset, returning, 0x2e);
        funsiz = moffset+0x2e;
#else
        const unsigned char returning[0x26] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x45, 0x88, 0x75, 0, 0x4d, 0x89, 0x27, 0x5d,
            0x41, 0x5f,
            0x41, 0x5e,
            0x41, 0x5d,
            0x41, 0x5c,
            0xc3};
        memcpy(nmem+moffset, returning, 0x26);
        funsiz = moffset+0x26;
#endif
#ifdef _WIN32
        DWORD tmp;
        VirtualProtect(nmem, funsiz, PAGE_EXECUTE_READ, &tmp);
#endif
        decoderFun = (fn)nmem;
        return;
    }
    ByteDecoder::~ByteDecoder() {
#ifdef _WIN32
        VirtualFree((void *)decoderFun, 0, MEM_RELEASE);
#else
        munmap((void *)decoderFun, funsiz);
#endif
    }
}

namespace SubIT {

    uint8_t* SbCodecMaxFOG::MakeTree(uint8_t* treeBeg, uint8_t* beg, uint8_t* end) {
        size_t  countMap[256] = {};
        uint8_t *treeEnd = treeBeg;
        for (; beg != end; ++beg) {
            if(*beg == 0) continue;
            if (countMap[*beg] == 0) {
                *treeEnd++ = *beg;
            }
            ++countMap[*beg];
        }
        // Sort tree according to count.
        std::sort(treeBeg, treeEnd, [&countMap](auto a, auto b) {
            return countMap[a] > countMap[b];
        });
        return treeEnd;
    }

    size_t SbCodecMaxFOG::EncodeBytes(uint8_t* beg, uint8_t* end, std::ostream* stream) {
        auto start = std::chrono::high_resolution_clock::now();
        // For future relocate.
        const std::streampos streambeg = stream->tellp();

        // Leave this part empty to fill how many bits encoded after encode.
        stream->seekp(sizeof(size_t), std::ios_base::cur);

        // Create tree.
        uint8_t  treeBeg[256];
        uint8_t* treeEnd = MakeTree(treeBeg, beg, end);

        // Write tree to stream for further decode.
        const size_t nodeCount = static_cast<size_t>(treeEnd - treeBeg);
        stream->rdbuf()->sputc(static_cast<char>(nodeCount & 0xFF));
        for (uint8_t i = 0; i != nodeCount; ++i) {
            stream->rdbuf()->sputc(static_cast<char>(treeBeg[i]));
        }

        // Create acMap and fill it to accelerate the encode process.
        uint8_t  acMap[256] = {};
        for (uint8_t i = 0; i != nodeCount; ++i) {
            acMap[treeBeg[i]] = i;
        }

        // Create output bitstream
        SbBitBuffer  bitBuffer(65536); // This constant can be changed to whatever you like but this is for good performance.
        SbOBitStream bitStream(&bitBuffer, stream->rdbuf());
        // Write all bytes into bit stream according to the map.
        size_t     bitsEncoded = 0;
        for(; beg != end; ++beg) {
            if(*beg == '\0') { // 0 is treated differently because of the nature of DCT'ed data, especially after quantization.
                bitStream.BitPut(0);
                ++bitsEncoded;
                continue;
            }
            bitStream.BitPut(1); // Golomb coding
            ++bitsEncoded;
            auto treePos = treeBeg;
            for(; treeEnd - treePos > 2; treePos += 2) {
                if(*treePos == *beg) { // No.0 in current chunk
                    bitStream.BitPut(0);
                    bitStream.BitPut(0);
                    bitsEncoded += 2;
                    break;
                }
                if(*(treePos+1) == *beg) { // No.1 in current chunk
                    bitStream.BitPut(0);
                    bitStream.BitPut(1);
                    bitsEncoded += 2;
                    break;
                }
                bitStream.BitPut(1); // Otherwise, go to the next chunk
                ++bitsEncoded;
            }
            if(treePos == treeEnd - 2) { // The last mark is only needed if there are two values in the last chunk
                bitStream.BitPut(!(*treePos == *beg));
                ++bitsEncoded;
            }
        }
        // Push remaining data.
        bitStream.Push();
        // Back to start position to write how many bits encoded.
        stream->seekp(streambeg);
        stream->write(reinterpret_cast<const char*>(&bitsEncoded), sizeof(size_t));
        auto time_end = std::chrono::high_resolution_clock::now();
        std::cerr << "File writing and MaxFOG coding time: " << std::chrono::duration<float>(time_end - start).count() << '\n';
        return bitsEncoded;
    }

    size_t SbCodecMaxFOG::GetEncodedBits(std::istream* stream) {
        size_t bits = 0;
        stream->read(reinterpret_cast<char*>(&bits), sizeof(size_t));
        return bits;
    }

    size_t SbCodecMaxFOG::DecodeBits(uint8_t* beg, size_t bits, std::istream* stream) {

        // Get tree and its range.
        uint8_t  treeBeg[256] = {};
        uint8_t  nodeCount = 0;
        auto start = std::chrono::high_resolution_clock::now();
        stream->read(reinterpret_cast<char*>(&nodeCount), sizeof(uint8_t));
        stream->read(reinterpret_cast<char*>(treeBeg), static_cast<std::streamsize>(nodeCount));
        ikp::ByteDecoder bytDec(treeBeg, nodeCount);
        char *buf = new char[(bits>>3)+1];
        unsigned char *curByte = (unsigned char *)buf;
        const size_t  totalBytes = (bits>>3)+((bits&0x7)?1:0);
        stream->read(buf, totalBytes);
        size_t bytesDecoded = 0;
        for(unsigned char bitPos = 0x80;(curByte - (unsigned char *)buf)<totalBytes || (0x80>>(bits&0x7))>=bitPos; ++beg, ++bytesDecoded) {
//            __builtin_printf("%x\n", *curByte);
//            std::cerr << (int)bitPos << '\n';
            *beg = bytDec(&curByte, &bitPos);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cerr << "File reading and MaxFOG decoding time: " << std::chrono::duration<float>(end - start).count() << '\n';
        delete[] buf;
        return bytesDecoded;
    }
}
