/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:scalar;

import eagine.core.vectorization;
import <type_traits>;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Basic scalar implementation template.
/// @ingroup math
/// @see tvec
/// @note This class is used in vectorizer vector-scalar operations.
export template <typename T, int N, bool V>
struct scalar {
    using type = scalar;

    /// @brief The scalar element type.
    using value_type = T;

    /// @brief Indicates if the implementation uses SIMD extensions.
    using is_vectorized = vect::has_simd_data<T, N, V>;

    using data_type =
      std::conditional_t<is_vectorized::value, vect::data_t<T, N, V>, T>;

    data_type _v{};

    using scalar_param = const scalar&;

    static constexpr auto _from(const data_type v) noexcept {
        return scalar{v};
    }

    static constexpr auto _make(const T v, const std::true_type) noexcept {
        return scalar{vect::fill<T, N, V>::apply(v)};
    }

    static constexpr auto _make(const T v, const std::false_type) noexcept {
        return scalar{v};
    }

    /// @brief Creates a scalar with the specified value.
    [[nodiscard]] static constexpr auto make(const T v) noexcept {
        return _make(v, is_vectorized());
    }

    constexpr auto _get(const std::true_type) const noexcept -> T {
        return _v[0];
    }

    constexpr auto _get(const std::false_type) const noexcept -> T {
        return _v;
    }

    /// @brief Implicit cast to the value type.
    [[nodiscard]] constexpr operator T() const noexcept {
        return _get(is_vectorized());
    }

    /// @brief Assignment from the value type.
    auto operator=(const T v) noexcept -> scalar& {
        return *this = make(v);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::math
