///
/// \file      base2_lee_dct_impl.hpp
/// \brief     Implement a template based compile time fast base2 dct.
/// \details   ~
/// \author    HenryDu
/// \date      1.11.2025
/// \copyright © HenryDu 2024. All right reserved.
///

#pragma once
#include <numbers>
#include <numeric>
#include <iostream>

#include "compile_time.hpp"

namespace SubIT::base2_lee_dct_impl {
    template <size_t N, typename Ty>
    struct butterfly_split_forward {
        template <size_t i>
        static constexpr void for_operator(const Ty* const vec, Ty* const tmp) {
            constexpr Ty r = std::numbers::pi_v<Ty> * static_cast<Ty>((i << 1) + 1) / static_cast<Ty>(N << 1);
            const Ty x = vec[i];
            const Ty y = vec[N - 1 - i];
            tmp[i]            = (x + y);
            tmp[i + (N >> 1)] = (x - y) * 0.5F / compile_time::cos_v<float, r>;
        }
        constexpr void operator()(const Ty* const vec, Ty* const tmp) {
            compile_time::__for<(N >> 1), butterfly_split_forward>(vec, tmp);
        }
    };
    template <size_t N, typename Ty>
    struct butterfly_merge_forward {
        template <size_t i>
        static constexpr void for_operator(Ty* const vec, const Ty* const tmp) {
            vec[(i << 1) + 0] = tmp[i];
            vec[(i << 1) + 1] = tmp[i + (N >> 1)] + tmp[i + (N >> 1) + 1];
        }
        constexpr void operator()(Ty* const vec, const Ty* const tmp) {
            compile_time::__for<(N >> 1) - 1, butterfly_merge_forward>(vec, tmp);
        }
    };
    template <size_t N, typename Ty>
    struct make_sequence_orthogonal {
        template <size_t i>
        static constexpr void for_operator(Ty* const vec) {
            const Ty f = i == 0 ? static_cast<Ty>(1) : std::numbers::sqrt2_v<Ty>;
            vec[i] *= (f / compile_time::sqrt_v<Ty, static_cast<Ty>(N)>);
        }
        constexpr void operator()(Ty* const vec) {
            compile_time::__for<N, make_sequence_orthogonal>(vec);
        }
    };
    template <size_t N, typename Ty>
    constexpr void forward_transform_recursive_part(Ty* const x, Ty* const t) {
        if constexpr (N != 1) {
            butterfly_split_forward<N, Ty>{}(x, t);
            forward_transform_recursive_part<(N >> 1), Ty>(t, x);
            forward_transform_recursive_part<(N >> 1), Ty>(t + (N >> 1), x);
            butterfly_merge_forward<N, Ty>{}(x, t);
            x[N - 2] = t[(N >> 1) - 1];
            x[N - 1] = t[N - 1];
        }
    }
    template <size_t N, std::floating_point Ty>
    constexpr void forward_transform(Ty* const x) {
        Ty tmp[N];
        forward_transform_recursive_part<N, Ty>(x, tmp);
        make_sequence_orthogonal<N, Ty>{}(x);
    }
    template <size_t N, typename Ty>
    struct butterfly_split_inverse {
        template <size_t i>
        static constexpr void for_operator(Ty* const vec, Ty* const tmp) {
            tmp[i] = vec[i << 1];
            tmp[i + (N >> 1)] = vec[(i << 1) - 1] + vec[(i << 1) + 1];
        }
        constexpr void operator()(Ty* const vec,  Ty* const tmp) {
            compile_time::__for<1, (N >> 1) - 1, butterfly_split_inverse>(vec, tmp);
        }
    };
    template <size_t N, typename Ty>
    struct butterfly_merge_inverse {
        template <size_t i>
        static constexpr void for_operator(Ty* const vec, const Ty* const tmp) {
            constexpr Ty r = std::numbers::pi_v<Ty> * static_cast<Ty>((i << 1) + 1) / static_cast<Ty>(N << 1);
            const Ty x = tmp[i];
            const Ty y = tmp[i + (N >> 1)] * static_cast<Ty>(0.5) / compile_time::cos_v<Ty, r>;
            vec[i] = x + y;
            vec[N - 1 - i] = x - y;
        }
        constexpr void operator()(Ty* const vec, const Ty* const tmp) {
            compile_time::__for<(N >> 1), butterfly_merge_inverse>(vec, tmp);
        }
    };
    template <size_t N, typename Ty>
    constexpr void inverse_transform_recursive_part(Ty* const x, Ty* const t) {
        if constexpr (N != 1) {
            butterfly_split_inverse<N, Ty>{}(x, t);
            t[0] = x[0];
            t[(N >> 1)] = x[1];
            inverse_transform_recursive_part<(N >> 1), Ty>(t, x);
            inverse_transform_recursive_part<(N >> 1), Ty>(t + (N >> 1), x);
            butterfly_merge_inverse<N, Ty>{}(x, t);
        }
    }
    template <size_t N, std::floating_point Ty>
    constexpr void inverse_transform(Ty* const x) {
        // Damn it inverse transform.
        // We are still wondering why we need to add one extra element, without it the code will throw an exception.
        // But this exception would only throw the last time we do the transform, so it's still OK to accept this error.
        Ty tmp[N];  
        make_sequence_orthogonal<N, Ty>{}(x);
        inverse_transform_recursive_part<N, Ty>(x, tmp);
    }
    template <size_t N, bool isTo, typename Ty>
    struct step_copy {
        template <size_t i>
        static constexpr void for_operator(Ty* const x, Ty* const t, size_t step) {
            if constexpr (isTo) {
                t[i] = x[i * step];
            } else {
                x[i * step] = t[i];
            }
        }
        constexpr void operator()(Ty* const x, Ty* const t, size_t step) {
            compile_time::__for<N, step_copy>(x, t, step);
        }
    };
    template <size_t N, std::floating_point Ty>
    constexpr void forward_transform_with_step(Ty* const x, size_t step) {
        Ty xp[N];
        step_copy<N, true, Ty>{}(x, xp, step);
        forward_transform<N, Ty>(xp);
        step_copy<N, false, Ty>{}(x, xp, step);
    }
    
    template <size_t N, std::floating_point Ty>
    constexpr void inverse_transform_with_step(Ty* const x, size_t step) {
        Ty xp[N];
        step_copy<N, true, Ty>{}(x, xp, step);
        inverse_transform<N, Ty>(xp);
        step_copy<N, false, Ty>{}(x, xp, step);
    }
}