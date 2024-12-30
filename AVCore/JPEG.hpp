///
/// \file      JPEG.hpp
/// \author    Henry Du
/// \date      11.16.2024
/// \copyright © Henry Du @ SubIT 2024. All right reserved.
///
#pragma once

#include <array>

namespace SubIT {
    // Contains information we would use from JPEG standard.
    class SbJPEG {
    public:
        static inline constexpr std::array<std::array<float, 64>, 2> quantizeTables = {
            // Luminance table.
            std::array<float, 64>{
                16.F, 11.F, 10.F, 16.F, 24.F, 40.F, 51.F, 61.F,
                12.F, 12.F, 14.F, 19.F, 26.F, 58.F, 60.F, 55.F,
                14.F, 13.F, 16.F, 24.F, 40.F, 57.F, 69.F, 56.F,
                14.F, 17.F, 22.F, 29.F, 51.F, 87.F, 80.F, 62.F,
                18.F, 22.F, 37.F, 56.F, 68.F, 109.F, 103.F, 77.F,
                24.F, 35.F, 55.F, 64.F, 81.F, 104.F, 113.F, 92.F,
                49.F, 64.F, 78.F, 87.F, 103.F, 121.F, 120.F, 101.F,
                72.F, 92.F, 95.F, 98.F, 112.F, 100.F, 103.F, 99.F
            },
            // Chrominance table.
            std::array<float, 64>{
                17.F, 18.F, 24.F, 47.F, 99.F, 99.F, 99.F, 99.F,
                18.F, 21.F, 26.F, 66.F, 99.F, 99.F, 99.F, 99.F,
                24.F, 26.F, 56.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                47.F, 66.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F,
                99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F, 99.F
            }
        };

        static constexpr inline float DCTFactorOrthogonal(std::ptrdiff_t i) { return i == 0 ? 0.70710678F : 1.F; }
        static constexpr inline float DCTFactorNormal(std::ptrdiff_t i)     { return 0.5F; }
        
        template <std::integral Ty>
        static constexpr inline Ty floor_to_octuple(Ty a) {
            const Ty q = a >> 3;
            const Ty r = a & 7;
            const Ty p = static_cast<Ty>(r != 0) << 3;
            return (q << 3) + p;
        }

        // Copy one block to another block, their width and height can be different.
        // OP defines a strategy when copying stuff from src to dst.
        template <typename T1, typename T2, typename OP> requires std::is_convertible_v<T1, T2>
        static constexpr inline void copy_block(const T1* src, size_t sw, size_t sh, size_t sl, T2* dst, size_t dl, OP fn) {
            for (size_t i = 0; i != sh; ++i) {
                for (size_t j = 0; j != sw; ++j) {
                    dst[i * dl + j] = static_cast<T2>(fn(src[i * sl + j]));
                }
            }
        }
        
    };
}
