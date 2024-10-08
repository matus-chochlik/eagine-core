/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:random_bytes;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Engine>
auto fill_with_random_bytes(span<byte> dst, Engine& engine) noexcept
  -> span<byte> {
    using ui_t = typename Engine::result_type;

    const ui_t mask = ((1U << 8U) - 1U);
    std::independent_bits_engine<Engine, 8U, ui_t> ibe(engine);

    generate(dst, [&] { return static_cast<byte>(ibe() & mask); });
    return dst;
}
//------------------------------------------------------------------------------
export auto fill_with_random_bytes(span<byte> dst) noexcept -> span<byte>;
//------------------------------------------------------------------------------
export auto fill_with_random_identifier(
  span<char> dst,
  std::default_random_engine&) noexcept -> span<char>;
export auto fill_with_random_identifier(span<char> dst) noexcept -> span<char>;
export auto fill_with_random_identifier(
  std::string& dst,
  std::default_random_engine&) noexcept -> std::string&;
//------------------------------------------------------------------------------
} // namespace eagine
