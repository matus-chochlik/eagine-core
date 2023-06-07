/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:bit_density;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
// type at least twice the size of byte
using double_byte = uint_fast16_t;
//------------------------------------------------------------------------------
export constexpr auto byte_bits() noexcept {
    return span_size_t(8);
}
//------------------------------------------------------------------------------
export constexpr auto dissolved_bits_length(
  const span_size_t original_length,
  const span_size_t bits) noexcept -> span_size_t {
    assert((bits >= 1) and (bits <= byte_bits()));
    return (
      ((original_length * byte_bits()) / bits) +
      (((original_length * byte_bits()) % bits) ? 1 : 0));
}
//------------------------------------------------------------------------------
export constexpr auto concentrated_bits_length(
  const span_size_t original_length,
  const span_size_t bits) noexcept -> span_size_t {
    assert((bits >= 1) and (bits <= byte_bits()));
    return (
      ((original_length * bits) / byte_bits()) +
      (((original_length * bits) % byte_bits()) ? 1 : 0));
}
//------------------------------------------------------------------------------
export template <typename Getter, typename Putter>
auto do_dissolve_bits(Getter get, Putter put, const span_size_t bits) noexcept
  -> bool {

    assert(bits >= 1);
    assert(bits <= byte_bits());

    span_size_t r = 0;
    double_byte w = 0U;
    const auto m = byte((1U << bits) - 1U); // NOLINT(hicpp-signed-bitwise)

    auto push = [&]() -> bool {
        while(r >= bits) {
            if(not put(byte(w >> double_byte(r - bits)) & m)) {
                return false;
            }
            r -= bits;
        }
        return true;
    };

    while(true) {
        if(r < bits) {
            if(const auto src{get()}) {
                w <<= byte_bits(); // NOLINT(hicpp-signed-bitwise)
                w |= double_byte(*src);
                r += byte_bits();
            } else {
                break;
            }
        }
        if(not push()) {
            return false;
        }
    }

    if(r > 0) {
        assert(r < bits);
        if(not put(byte(w << double_byte(bits - r)) & m)) {
            return false;
        }
    }
    return true;
}
//------------------------------------------------------------------------------
export template <typename Getter, typename Putter>
auto do_concentrate_bits(Getter get, Putter put, const span_size_t bits) noexcept
  -> bool {
    assert(bits >= 1);
    assert(bits <= byte_bits());

    span_size_t r = 0;
    double_byte w = 0U;
    const double_byte m = (1U << bits) - 1U; // NOLINT(hicpp-signed-bitwise)

    bool done = false;

    while(not done) {
        while(r < byte_bits()) {
            if(const auto src{get()}) {
                w <<= bits; // NOLINT(hicpp-signed-bitwise)
                // NOLINTNEXTLINE(hicpp-signed-bitwise)
                w = (w & ~m) | (double_byte(*src) & m);
                r += bits;
            } else {
                done = true;
                break;
            }
        }
        if(r >= byte_bits()) {
            if(not put(byte(w >> double_byte(r - byte_bits())))) {
                return false;
            }
            r -= byte_bits();
        }
    }
    return true;
}
//------------------------------------------------------------------------------
} // namespace eagine
