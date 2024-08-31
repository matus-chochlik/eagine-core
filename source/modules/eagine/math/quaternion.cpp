/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:quaternion;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.units;
import eagine.core.simd;
import :functions;
import :vector;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Template for quaternion.
/// @ingroup math
export template <typename T, bool V = true>
class quaternion {
public:
    /// @brief Default constructor. Constructs a zero quaternion.
    /// @post this->is_zero()
    constexpr quaternion() noexcept = default;

    /// @brief Constructs a quaternion from the real and the imaginary path.
    constexpr quaternion(T real, const vector<T, 3, V>& imag) noexcept
      : _params{imag, real} {}

    /// @brief Constructs a quaternion from a unit vector and an angle.
    /// @pre axis.is_unit()
    /// @post this->is_unit()
    constexpr quaternion(vector<T, 3, V> axis, radians_t<T> angle)
      : quaternion{cos(angle / T(2)), axis * sin(angle / T(2))} {
        assert(axis.is_unit());
    }

    /// @brief Returns the i-imaginary part of the quaternion.
    constexpr auto i() const noexcept -> T {
        return _params[0];
    }

    /// @brief Returns the j-imaginary part of the quaternion.
    constexpr auto j() const noexcept -> T {
        return _params[1];
    }

    /// @brief Returns the k-imaginary part of the quaternion.
    constexpr auto k() const noexcept -> T {
        return _params[2];
    }

    /// @brief Returns the real part of the quaternion.
    constexpr auto r() const noexcept -> T {
        return _params[3];
    }

    /// @brief Returns the real part of the quaternion.
    constexpr auto real() const noexcept -> T {
        return _params[3];
    }

    /// @brief Returns the imaginary part of the quaternion.
    constexpr auto imag() const noexcept -> vector<T, 3, V> {
        return vector<T, 3, V>{_params};
    }

    /// @brief Returns the dot product of this quaternion with other quaternion.
    constexpr auto dot(const quaternion& that) const noexcept -> T {
        return _params.dot(that._params);
    }

    /// @brief Returns the magnitude of this quaternion.
    constexpr auto magnitude() const noexcept -> T {
        using std::sqrt;
        return sqrt(dot(*this));
    }

    /// @brief Indicates if this quaternion has unit magnitude.
    /// @see is_zero
    constexpr auto is_unit(
      const T eps = std::numeric_limits<T>::epsilon()) const noexcept -> bool {
        return _params.is_unit(eps);
    }

    /// @brief Indicates if this quaternion has zero magnitude.
    /// @see is_unit
    constexpr auto is_zero(
      const T eps = std::numeric_limits<T>::epsilon()) const noexcept -> bool {
        return _params.is_zero(eps);
    }

    /// @brief Returns this quaternion normalized
    /// @pre not is_zero()
    /// @post is_unit()
    constexpr auto normalized(const T eps = std::numeric_limits<T>::epsilon())
      const noexcept -> quaternion {
        return {_params.normalized()};
    }

    /// @brief Indicates if this quaternion is close to another quaternion.
    [[nodiscard]] auto close_to(
      const quaternion& v,
      const T eps = std::numeric_limits<T>::epsilon()) const noexcept -> bool {
        return _params.close_to(v._params, eps);
    }

    /// @brief Returns the inverse of this quaternion.
    /// @pre not is_zero()
    constexpr auto inverse(const T eps = std::numeric_limits<T>::epsilon())
      const noexcept -> quaternion {
        const auto dp{dot(*this)};
        assert(dp > eps);
        return {real() / dp, -imag() / dp};
    }

    /// @brief Returns the conjugate of this quaternion.
    constexpr auto conjugate() const noexcept -> quaternion {
        return {real(), -imag()};
    }

    /// @brief Addition operator.
    constexpr auto operator+(const quaternion& that) const noexcept
      -> quaternion {
        return {_params + that._params};
    }

    /// @brief Multiplication by constant operator.
    constexpr auto operator*(const T c) const noexcept -> quaternion {
        return {_params * c};
    }

    /// @brief Multiplication operator.
    constexpr auto operator*(const quaternion& q) const noexcept -> quaternion {
        return {vector<T, 4, V>{
          _sp<+1, +1, -1, +1>().dot(q._params.template shuffled<3, 2, 1, 0>()),
          _sp<-1, +1, +1, +1>().dot(q._params.template shuffled<2, 3, 0, 1>()),
          _sp<+1, -1, +1, +1>().dot(q._params.template shuffled<1, 0, 3, 2>()),
          _sp<-1, -1, -1, +1>().dot(q._params.template shuffled<0, 1, 2, 3>())}};
    }

    /// @brief Rotates a vector by a quaternion.
    constexpr auto rotate(const vector<T, 3, V>& v) const noexcept
      -> vector<T, 3, V> {
        return (*this * quaternion(T(0), v) * conjugate()).imag();
    }

private:
    constexpr quaternion(const vector<T, 4, V> params) noexcept
      : _params{params} {}

    template <int... S>
    constexpr auto _sp() const noexcept -> vector<T, 4, V> {
        return vector<T, 4, V>{T(S)...} * _params;
    }

    vector<T, 4, V> _params{};
};
//------------------------------------------------------------------------------
} // namespace eagine::math
