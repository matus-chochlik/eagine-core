/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:colors;

import std;
import eagine.core.types;
import :constants;
import :functions;
import :vector;

namespace eagine::math {
//------------------------------------------------------------------------------
export template <std::floating_point T, bool V>
[[nodiscard]] constexpr auto rgb_to_hsv(vector<T, 3, V> rgb) noexcept
  -> vector<T, 3, V> {
    const auto mn{minimum(rgb.x(), rgb.y(), rgb.z())};
    const auto mx{maximum(rgb.x(), rgb.y(), rgb.z())};
    const auto zero{T(0)};
    const auto half{T(1) / T(2)};
    const auto one{T(1)};
    const auto h{[=]() -> T {
        using std::sqrt;
        const T div{sqrt(
          dot(rgb, rgb) -
          (rgb.x() * rgb.y() + rgb.y() * rgb.z() + rgb.z() * rgb.x()))};
        if(div > zero or div < zero) {
            using std::acos;
            const T acs{acos((rgb.x() - half * (rgb.y() + rgb.z())) / div)};
            if(rgb.y() >= rgb.z()) {
                return T(acs / tau);
            } else {
                return one - T(acs / tau);
            }
        } else {
            return one;
        }
    }()};
    const auto s{[=]() -> T {
        if(mx > zero) {
            return one - mn / mx;
        } else {
            return zero;
        }
    }()};
    const auto v{mx};

    return {h, s, v};
}
//------------------------------------------------------------------------------
export template <std::floating_point T, bool V>
[[nodiscard]] constexpr auto hsv_to_rgb(vector<T, 3, V> hsv) noexcept
  -> vector<T, 3, V> {
    using std::abs;
    using std::fmod;

    const auto hp{hsv.x() * T(6)};
    const auto c{hsv.y() * hsv.z()};
    const auto x{c * (T(1) - abs(fmod(hp, T(2)) - T(1)))};
    const auto m{hsv.z() - c};
    const auto z{T(0)};
    if(hp < T(1)) {
        return {c + m, x + m, z + m};
    }
    if(hp < T(2)) {
        return {x + m, c + m, z + m};
    }
    if(hp < T(3)) {
        return {z + m, c + m, x + m};
    }
    if(hp < T(4)) {
        return {z + m, x + m, c + m};
    }
    if(hp < T(5)) {
        return {x + m, z + m, c + m};
    }
    return {c + m, z + m, x + m};
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto bytes_to_rgb(byte r, byte g, byte b) noexcept
  -> vector<T, 3, V> {
    return {
      T(r) / T(std::numeric_limits<byte>::max()),
      T(g) / T(std::numeric_limits<byte>::max()),
      T(b) / T(std::numeric_limits<byte>::max())};
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto bytes_to_rgb(byte r, byte g, byte b, byte a) noexcept
  -> vector<T, 4, V> {
    return {
      T(r) / T(std::numeric_limits<byte>::max()),
      T(g) / T(std::numeric_limits<byte>::max()),
      T(b) / T(std::numeric_limits<byte>::max()),
      T(a) / T(std::numeric_limits<byte>::max())};
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto bytes_to_rgb(std::uint32_t c) noexcept
  -> vector<T, 3, V> {
    return bytes_to_rgb<T, V>(
      (c >> 16U) & std::numeric_limits<byte>::max(),
      (c >> 8U) & std::numeric_limits<byte>::max(),
      (c >> 0U) & std::numeric_limits<byte>::max());
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto bytes_to_rgb(std::uint32_t c) noexcept
  -> vector<T, 4, V> {
    return bytes_to_rgb<T, V>(
      (c >> 24U) & std::numeric_limits<byte>::max(),
      (c >> 16U) & std::numeric_limits<byte>::max(),
      (c >> 8U) & std::numeric_limits<byte>::max(),
      (c >> 0U) & std::numeric_limits<byte>::max());
}
//------------------------------------------------------------------------------
static constexpr auto chars2byte(const char h, const char l) noexcept {
    const auto char2byte{[](const char c) {
        if(('0' <= c) and (c <= '9')) {
            return byte(c - '0');
        } else if(('a' <= c) and (c <= 'f')) {
            return byte(10U + (c - 'a'));
        } else if(('A' <= c) and (c <= 'F')) {
            return byte(10U + (c - 'A'));
        } else {
            return byte{0U};
        }
    }};
    return byte((char2byte(h) << 4U) | char2byte(l));
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto hex_to_rgb(const char (&hex)[8]) noexcept
  -> vector<T, 3, V> {
    return bytes_to_rgb<T, V>(
      chars2byte(hex[1], hex[2]),
      chars2byte(hex[3], hex[4]),
      chars2byte(hex[5], hex[6]));
}
//------------------------------------------------------------------------------
export template <std::floating_point T = float, bool V = true>
[[nodiscard]] constexpr auto hex_to_rgba(const char (&hex)[10]) noexcept
  -> vector<T, 3, V> {
    return bytes_to_rgb<T, V>(
      chars2byte(hex[1], hex[2]),
      chars2byte(hex[3], hex[4]),
      chars2byte(hex[5], hex[6]),
      chars2byte(hex[7], hex[8]));
}
//------------------------------------------------------------------------------
} // namespace eagine::math
