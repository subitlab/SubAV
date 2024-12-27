///
/// \file      IKP.cpp
/// \brief     IKP byte decoder implementation.
/// \author    Steve Wang (Formatted by Henry Du)
/// \date      12.27.2024
/// \copyright © Steve Wang 2024. All right reserved.
///

#include <cstdlib>
#include "IKP.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

namespace SubIT {
    SbIKPByteDecoder::SbIKPByteDecoder(const uint8_t *freqs, const uint8_t totalCount) {
            int moffset = 0;
#ifdef _WIN32
            char *nmem = (char *)VirtualAlloc(nullptr, funsiz = (17+0x31+((int)((totalCount - 3)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e)+0x2e), MEM_COMMIT, PAGE_READWRITE);
            const uint8_t win_workaround[17] = {0x57, 0x56,
                                                     0x41, 0x54,
                                                     0x41, 0x55,
                                                     0x41, 0x56,
                                                     0x41, 0x57,
                                                     0x53,
                                                     0x48, 0x89, 0xcf, 0x48, 0x89, 0xd6};
            memcpy(nmem, win_workaround, 17);
            moffset = 17;
            const uint64_t returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#else
            char *nmem = (char *)mmap(NULL, funsiz = (0x31+((int)((totalCount - 3)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e)+0x23), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            const uint64_t returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#endif
            {
                const uint64_t joffset = returnAddr - 0x31;
                const uint8_t header[0x31] = {0x41, 0x54,
                    0x41, 0x55,
                    0x41, 0x56,
                    0x41, 0x57,
                    0x55, 0x48, 0x89, 0xe5, 0x49, 0x89, 0xff, 0x4c, 0x8b, 0x27, 0x49, 0x89, 0xf5, 0x4d, 0x8b, 0x75, 0x00, 0x41, 0x0f, 0xb6, 0x14, 0x24, 0x4c, 0x89, 0xf0, 0x21, 0xd0, 0x84, 0xc0, 0x75, 0x0a, 0xb8, 0, 0, 0, 0, 0xe9, (uint8_t)joffset, (uint8_t)(joffset >> 8), (uint8_t)(joffset >> 16), (uint8_t)(joffset >> 24)};
                memcpy(nmem+moffset, header, 0x31);
                moffset += 0x31;
            }
            for(int i = 0; i <= totalCount-3; i+=2) {
#ifdef _WIN32
                const uint64_t joffset1 = returnAddr - 0x4e - moffset+17,
                                             joffset2 = returnAddr - 0x58 - moffset+17;
#else
                const uint64_t joffset1 = returnAddr - 0x4e - moffset,
                                             joffset2 = returnAddr - 0x58 - moffset;
#endif
                const uint8_t blockdata[0x58] = {
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
                    0xe9, (uint8_t)joffset1, (uint8_t)(joffset1>>8), (uint8_t)(joffset1>>16), (uint8_t)(joffset1>>24),
                    0xb8, freqs[i+1], 0, 0, 0,
                    0xe9, (uint8_t)joffset2, (uint8_t)(joffset2>>8), (uint8_t)(joffset2>>16), (uint8_t)(joffset2>>24)
                };
                memcpy(nmem+moffset, blockdata, 0x58);
                moffset += 0x58;
            }
            if(totalCount & 1) {
                const uint8_t lastpar[5] = {0xb8, freqs[totalCount-1], 0, 0, 0};
                memcpy(nmem+moffset, lastpar, 5);
                moffset += 5;
            }
            else {
                const uint8_t lastpar[0x2e] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf0, 0x84, 0xc0, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x49, 0x8b, 0x14, 0x24, 0x4c, 0x89, 0xf0, 0x21, 0xd0, 0x84, 0xc0, 0x75, 0x07, 0xb8, freqs[totalCount-2], 0, 0, 0, 0xeb, 0x05, 0xb8, freqs[totalCount-1], 0, 0, 0};
                memcpy(nmem+moffset, lastpar, 0x2e);
                moffset += 0x2e;
            }
#ifdef _WIN32
          //const uint8_t returning[0x2e] = {0x41, 0xd0, 0xe6, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x01, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x4c, 0x89, 0xeb, 0x44, 0x88, 0x33, 0x4c, 0x89, 0xfb, 0x4c, 0x89, 0x23, 0x5d,
            const uint8_t returning[0x31] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x45, 0x88, 0x75, 0, 0x4d, 0x89, 0x27, 0x5d,
            0x41, 0x5f,
            0x41, 0x5e,
            0x41, 0x5d,
            0x41, 0x5c,
            0x5b,
            0x41, 0x5f,
            0x41, 0x5e,
            0x41, 0x5d,
            0x41, 0x5c,
            0x5e, 0x5f,
            0xc3};
            memcpy(nmem+moffset, returning, 0x31);
            funsiz = moffset+0x31;
#else
            const uint8_t returning[0x26] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x45, 0x88, 0x75, 0, 0x4d, 0x89, 0x27, 0x5d,
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
        SbIKPByteDecoder::~SbIKPByteDecoder() {
#ifdef _WIN32
            VirtualFree((void*)decoderFun, 0, MEM_RELEASE);
#else
            munmap((void *)decoderFun, funsiz);
#endif
        }
}