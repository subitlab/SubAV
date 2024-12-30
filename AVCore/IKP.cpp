///
/// \file      IKP.cpp
/// \brief     IKP byte decoder implementation.
/// \author    Steve Wang (Formatted by Henry Du)
/// \date      12.27.2024
/// \copyright © Steve Wang 2024. All right reserved.
///

#include <cstdlib>
#include <cstring>
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
                                                 0x48, 0x89, 0xcf, 0x48, 0x89, 0xd6}; // Convert Microsoft calling convention to SysV calling convention
        memcpy(nmem, win_workaround, 17);
        moffset = 17;
        const uint64_t returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#else
        char *nmem = (char *)mmap(NULL, funsiz = (0x31+((int)((totalCount - 3)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e)+0x23), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        const uint64_t returnAddr = 0x31+((int)((totalCount - 1)>>1)*0x58)+((totalCount&1) ? 5 : 0x2e);
#endif
        {
            const uint64_t joffset = returnAddr - 0x31;
            const uint8_t header[0x31] = {
                0x41, 0x54,             //push %r12
                0x41, 0x55,             //push %r13
                0x41, 0x56,             //push %r14
                0x41, 0x57,             //push %r15
                                        //;We'll use these callee-saved registers, so we push them onto the stack first.
                0x55,                   //push %rbp
                0x48, 0x89, 0xe5,       //mov %rsp, %rbp
                                        //;Standard x64 function header
                0x49, 0x89, 0xff,       //mov %rdi, %r15        ;the first argument,pointer to pointer to the current byte
                0x4c, 0x8b, 0x27,       //mov (%rdi), %r12      ;pointer to the current byte
                0x49, 0x89, 0xf5,       //mov %rsi, %r13        ;the second argument, pointer to the char marking the current byte
                0x4d, 0x8b, 0x75, 0x00, //mov (%r13), %r14      ;the char itself
                0x41, 0x0f, 0xb6,
                      0x14, 0x24,       //movzbl (%r12), %edx   ;get current byte
                0x4c, 0x89, 0xf0,       //mov %r14, %rax        ;get current (the first) bit
                0x21, 0xd0,             //and %edx, %eax        ;get the current (the first) bit
                0x84, 0xc0,             //test %al, %al         ;test if the first bit is zero
                0x75, 0x0a,             //jne <Golomb blocks>   ;if not, start Golomb decoding
                0xb8, 0, 0, 0, 0,       //mov %0x0, %eax        ;otherwise, return '\0'
                0xe9, (uint8_t)joffset, (uint8_t)(joffset >> 8), (uint8_t)(joffset >> 16), (uint8_t)(joffset >> 24)}; //jmp <Returning block>
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
                0x41, 0xd0, 0xee,       //shr $1, %r14b     ;next bit
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;load the current bit marker into accumulator
                0x84, 0xc0,             //test %al, %al     ;if it is zero, then we should read the next byte
                0x75, 0x0b,             //jne NO_OVERFLOW
                0x49, 0xc7, 0xc6,
                      0x80, 0, 0, 0,    //mov $0x80, %r14   ;bin10000000, the highest bit
                0x49, 0x83, 0xc4, 0x01, //add $0x1, %r12    ;move to the next byte
                                    //NO_OVERFLOW:
                0x49, 0x8b, 0x14, 0x24, //mov (%r12), %rdx  ;get the current byte
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;load the current bit marker into the accumulator
                0x21, 0xd0,             //and %edx, %eax    ;get current bit
                0x84, 0xc0,             //test %al, %al     ;if current bit is not 0
                0x75, 0x36,             //jne <next block>  ; then go to the next block
                0x41, 0xd0, 0xee,       //shr $1, %r14b     ;next bit
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;loat the current bit marker into the accumulator
                0x84, 0xc0,             //test %al, %al     ;if it is zero, then we should read the next byte
                0x75, 0x0b,             //jne NO_OVERFLOW1
                0x49, 0xc7, 0xc6,
                      0x80, 0, 0, 0,    //mov $0x80, %r14   ;bin10000000, the highest bit
                0x49, 0x83, 0xc4, 0x01, //add $0x1, %r12    ;move to the next byte
                                    //NO_OVERFLOW1:
                0x49, 0x8b, 0x14, 0x24, //mov (%r12), %rdx  ;get the current byte
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;load the current bit marker into the accumulator
                0x21, 0xd0,             //and %edx, %eax    ;get current bit
                0x84, 0xc0,             //test %al, %al     ;if currentbit == 0
                0x75, 0x0a,             //jne CURRENT_1     ;then
                0xb8, freqs[i], 0, 0, 0,//mov freqs[i], %eax    ;return freqs[i]
                0xe9, (uint8_t)joffset1, (uint8_t)(joffset1>>8), (uint8_t)(joffset1>>16), (uint8_t)(joffset1>>24),
                                        //jmp <Returning block>
                                    //CURRENT_1:            ;else
                0xb8, freqs[i+1],
                                0, 0, 0,//mov freqs[i+1], %eax  ;return freqs[i+1]
                0xe9, (uint8_t)joffset2, (uint8_t)(joffset2>>8), (uint8_t)(joffset2>>16), (uint8_t)(joffset2>>24)
                                        //jmp <Returning block>
            };
            memcpy(nmem+moffset, blockdata, 0x58);
            moffset += 0x58;
        }
        if(totalCount & 1) { //Total value count is odd, there is only one value in the last chunk, therefore no code is read.
            const uint8_t lastpar[5] = {0xb8, freqs[totalCount-1], 0, 0, 0}; //mov freqs[totalCount-1], %eax ;return the last value. No JMP is needed because it will directly drop into the returning block.
            memcpy(nmem+moffset, lastpar, 5);
            moffset += 5;
        }
        else { //Total value count count is even, there are two values in the last chunk.
            const uint8_t lastpar[0x2e] = {
                0x41, 0xd0, 0xee,       //shr $1, %r14b     ;next bit
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;load into accumulator
                0x84, 0xc0,             //test %al, %al     ;if it is zero, go to the next byte
                0x75, 0x0b,             //jne NO_OVERFLOW_LASTCHUNK
                0x49, 0xc7, 0xc6,
                    0x80, 0, 0, 0,      //mov $0x80, %r14   ;bin10000000
                0x49, 0x83, 0xc4, 0x01, //add $0x1, %r12    ;next byte
                                    //NO_OVERFLOW_LASTCHUNK:
                0x49, 0x8b, 0x14, 0x24, //mov (%r12), %rdx  ;current byte
                0x4c, 0x89, 0xf0,       //mov %r14, %rax    ;load bit marker into accumulator
                0x21, 0xd0,             //and %edx, %eax    ;get current bit
                0x84, 0xc0,             //test %al, %al     ;if currentbit == 0
                0x75, 0x07,             //jne LAST_1        ;then
                0xb8, freqs[totalCount-2], 0, 0, 0, //mov freqs[totalCount-2], %eax ;return freqs[totalCount-2]
                0xeb, 0x05,             //jmp <Returning block> ;using short relatave jump because it is so close.
                                    //LAST_1:               ;else
                0xb8, freqs[totalCount-1], 0, 0, 0 //mov freqs[totalCount-1], %eax ;return freqs[totalCount-1]
                                        //                  ;No JMP is needed because it will directly drop into the returning block.
            };
            memcpy(nmem+moffset, lastpar, 0x2e);
            moffset += 0x2e;
        }
#ifdef _WIN32
        const uint8_t returning[0x31] = {0x41, 0xd0, 0xee, 0x4c, 0x89, 0xf2, 0x84, 0xd2, 0x75, 0x0b, 0x49, 0xc7, 0xc6, 0x80, 0, 0, 0, 0x49, 0x83, 0xc4, 0x01, 0x45, 0x88, 0x75, 0, 0x4d, 0x89, 0x27, 0x5d, 0x41, 0x5f, 0x41, 0x5e, 0x41, 0x5d, 0x41, 0x5c, //see the #else block
        0x5b,
        0x41, 0x5f,
        0x41, 0x5e,
        0x41, 0x5d,
        0x41, 0x5c,
        0x5e, 0x5f,
        0xc3};      //calling convention convertion
        memcpy(nmem+moffset, returning, 0x31);
        funsiz = moffset+0x31;
#else
        const uint8_t returning[0x26] = {
            0x41, 0xd0, 0xee,       //shr $1, %r14b     ;next bit
            0x4c, 0x89, 0xf2,       //mov %r14, %rdx    ;load into RDX
            0x84, 0xd2,             //test %dl, %dl     ;if it is zero, go to the next byte
            0x75, 0x0b,             //jne NO_OVERFLOW_RET
            0x49, 0xc7, 0xc6,
                0x80, 0, 0, 0,      //mov $0x80, %r14   ;#%10000000
            0x49, 0x83, 0xc4, 0x01, //add $0x1, %r12    ;next byte
                                //NO_OVERFLOW_RET:
            0x45, 0x88, 0x75, 0,    //mov %r14b, (%r13) ;store the marker
            0x4d, 0x89, 0x27,       //mov %r12 (%r15)   ;store the pointer
            0x5d,                   //pop %rbp          ;standard x64 function end
            0x41, 0x5f,             //pop %r15
            0x41, 0x5e,             //pop %r14
            0x41, 0x5d,             //pop %r13
            0x41, 0x5c,             //pop %r12
                                    //;write back the previously pushed callee-saved registers
            0xc3                    //ret               ;return from subroutine
        };
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
