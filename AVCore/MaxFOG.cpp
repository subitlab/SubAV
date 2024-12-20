///
/// \file      MaxFOG.cpp
/// \brief     A MaxFOG codec implementation.
/// \details   ~
/// \author    HenryDu
/// \date      9.11.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#include <algorithm> // for std::sort
#include <ostream>
#include <istream>

#include "BitStream.hpp"
#include "MaxFOG.hpp"

#include <iostream>

namespace SubIT {

    uint8_t* SbCodecMaxFOG::MakeTree(uint8_t* treeBeg, uint8_t* beg, uint8_t* end) {
        size_t  countMap[256] = {};
        uint8_t *treeEnd = treeBeg;
        for (; beg != end; ++beg) {
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

        // Start iterate through tree and encode.
        for (; beg != end; ++beg) {
            // Tree depth is a function of nodeCount;
            const size_t treeDepth = (nodeCount + 1) >> 1;
            const uint8_t branchSide = acMap[*beg] & 1;

            // Normal case huffman tree
            if (treeDepth > 1) {
                const size_t depthLimit = treeDepth - 2;
                const size_t depthAt = acMap[*beg] >> 1;
                const size_t determinant = nodeCount - acMap[*beg]; // Compare determinant < 3.

                // 1 count.
                size_t depth = 0;
                for (; depth != depthAt && depth != depthLimit; ++depth) {
                    bitStream.BitPut(1);
                }

                // Not satisfy determinant, which means it must read left branch.
                if (depth == depthAt) {
                    bitStream.BitPut(0);
                }
                else {
                    bitStream.BitPut(determinant < 3);
                }
                ++depth;
                bitStream.BitPut(branchSide);
                bitsEncoded += (depth + 1);
            }
            // Least case huffman tree.
            else {
                bitStream.BitPut(branchSide);
                bitsEncoded++;
            }
        }

        // Push remain data.
        bitStream.Push();
        // Back to start position to write how many bits encoded.
        stream->seekp(streambeg);
        stream->write(reinterpret_cast<const char*>(&bitsEncoded), sizeof(size_t));
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
        stream->read(reinterpret_cast<char*>(&nodeCount), sizeof(uint8_t));
        stream->read(reinterpret_cast<char*>(treeBeg), static_cast<std::streamsize>(nodeCount));

        // Create bit-buffer and stream.
        SbBitBuffer  bitBuffer(65536);
        SbIBitStream bitStream(&bitBuffer, stream->rdbuf());

        // We can't get any data unless we pull it first.
        bitStream.Pull();

        // Read all bytes from the bit stream and recover it into bit stream.
        const size_t depthMax = (nodeCount + 1) >> 1;
        size_t       bytesDecoded = 0;

        for (size_t i = 0; i != bits; ++beg, ++bytesDecoded) {
            // Normal case huffman tree.
            if (depthMax > 1) {
                const size_t depthLim = depthMax - 2;
                size_t  depth = 0;
                uint8_t  currentBit = bitStream.BitGet();
                for (; depth != depthLim && currentBit != 0; ++depth) {
                    currentBit = bitStream.BitGet();
                }
                const size_t index = depth << 1;
                if (depth == depthLim) {
                    uint8_t remainIndex = currentBit << 1;
                    remainIndex |= bitStream.BitGet();
                    *beg = treeBeg[index + remainIndex];
                }
                else {
                    const uint8_t remainIndex = bitStream.BitGet();
                    *beg = treeBeg[index + remainIndex];
                }
                i += (depth + 2);
            }
            // Least case huffman tree.
            else {
                const uint8_t index = bitStream.BitGet();
                beg[index] = treeBeg[index];
                ++i;
            }
        }
        return bytesDecoded;
    }
}
