/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:scalar;

import std;
import eagine.core.simd;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Basic scalar implementation template.
/// @ingroup math
/// @see vector
/// @note This class is used in SIMD vector-scalar operations.
export template <typename T, int N, bool V>
class scalar {
public:
    using type = scalar;

    /// @brief The scalar element type.
    using value_type = T;

    /// @brief Indicates if the implementation uses SIMD extensions.
    using is_vectorized = simd::has_simd_data<T, N, V>;

    using data_type =
      std::conditional_t<is_vectorized::value, simd::data_t<T, N, V>, T>;

    data_type _v{};

    using scalar_param = const scalar&;

    /// @brief Creates a scalar with the specified value.
    [[nodiscard]] static constexpr auto make(const T v) noexcept {
        if constexpr(is_vectorized()) {
            return scalar{simd::fill<T, N, V>::apply(v)};
        } else {
            return scalar{v};
        }
    }

    /// @brief Implicit cast to the value type.
    [[nodiscard]] constexpr operator T() const noexcept {
        if constexpr(is_vectorized()) {
            return _v[0];
        } else {
            return _v;
        }
    }

    /// @brief Assignment from the value type.
    auto operator=(const T v) noexcept -> scalar& {
        if constexpr(is_vectorized()) {
            _v = simd::fill<T, N, V>::apply(v);
        } else {
            _v = v;
        }
        return *this;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::math
