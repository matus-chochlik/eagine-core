/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.utility;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
auto fill_with_random_bytes(span<byte> dst) noexcept -> span<byte> {
    std::random_device rd;
    std::default_random_engine re{rd()};
    return fill_with_random_bytes(dst, re);
}
//------------------------------------------------------------------------------
auto do_fill_with_random_identifier(
  auto& dst,
  std::default_random_engine& re) noexcept {
    static const std::string_view next_char{
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789"};
    static const std::string_view first_char{next_char.substr(0, 52)};

    const auto pick{[&re](const std::string_view s) {
        return s[std::uniform_int_distribution<std::size_t>{
          0Z, s.size() - 1Z}(re)];
    }};

    bool first{true};
    for(char& c : dst) {
        if(first) {
            first = false;
            c = pick(first_char);
        } else {
            c = pick(next_char);
        }
    }
    return dst;
}
//------------------------------------------------------------------------------
auto fill_with_random_identifier(
  span<char> dst,
  std::default_random_engine& re) noexcept -> span<char> {
    do_fill_with_random_identifier(dst, re);
    return dst;
}
//------------------------------------------------------------------------------
auto fill_with_random_identifier(span<char> dst) noexcept -> span<char> {
    std::random_device rd;
    std::default_random_engine re{rd()};
    return fill_with_random_identifier(dst, re);
}
//------------------------------------------------------------------------------
auto fill_with_random_identifier(
  std::string& dst,
  std::default_random_engine& re) noexcept -> std::string& {
    do_fill_with_random_identifier(dst, re);
    return dst;
}
//------------------------------------------------------------------------------
} // namespace eagine
