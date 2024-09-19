/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.identifier;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
auto byte_to_identifier(const byte b) noexcept -> identifier {
    // clang-format off
    static const char hd[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    // clang-format on
    const char src[5] = {
      '0',
      'x',
      hd[integer((b >> 4U) & 0xFU)], // NOLINT(hicpp-signed-bitwise)
      hd[integer(b & 0xFU)],         // NOLINT(hicpp-signed-bitwise)
      '\0'};
    return identifier{src};
}
//------------------------------------------------------------------------------
template <std::size_t L>
static auto _ident_dec_to_str(int i, char (&tmp)[L]) noexcept -> identifier {
    tmp[L - 1] = '\0';
    std::size_t l = L - 1;
    while(l-- > 0) {
        tmp[l] = char(i % 10 + '0');
        i /= 10;
    }
    return identifier{tmp};
}
//------------------------------------------------------------------------------
auto dec_to_identifier(const int i) noexcept -> identifier {
    assert(i >= 0);
    if(i < 10) {
        char tmp[2];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 100) {
        char tmp[3];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 1000) {
        char tmp[4];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 10000) {
        char tmp[5];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 100000) {
        char tmp[6];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 1000000) {
        char tmp[7];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 10000000) {
        char tmp[8];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 100000000) {
        char tmp[9];
        return _ident_dec_to_str(i, tmp);
    }
    if(i < 1000000000) {
        char tmp[10];
        return _ident_dec_to_str(i, tmp);
    }
    char tmp[11];
    return _ident_dec_to_str(i, tmp);
}
//------------------------------------------------------------------------------
template <std::size_t... I, typename Engine>
void fill_random_charset_string(
  span<char> dst,
  const span<const char> src,
  std::index_sequence<I...>,
  Engine& engine) {
    std::uniform_int_distribution<std::size_t> dist(0, src.size() - 1);
    dst[sizeof...(I)] = '\0';
    ((dst[I] = src[dist(engine)]), ...);
}
//------------------------------------------------------------------------------
template <
  std::size_t M,
  std::size_t B,
  typename CharSet,
  typename UIntT,
  bool V,
  typename Engine>
auto make_random_basic_identifier(
  std::type_identity<basic_identifier<M, B, CharSet, UIntT, V>>,
  Engine& engine) {
    char temp[M + 1];
    fill_random_charset_string(
      cover(temp), view(CharSet::values), std::make_index_sequence<M>(), engine);
    return basic_identifier<M, B, CharSet, UIntT, V>{temp};
}
//------------------------------------------------------------------------------
template <
  std::size_t M,
  std::size_t B,
  typename CharSet,
  typename UIntT,
  bool V,
  typename Engine>
auto make_random_basic_identifier(
  std::type_identity<basic_identifier<M, B, CharSet, UIntT, V>>,
  const span<const char> src,
  Engine& engine) {
    char temp[M + 1];
    fill_random_charset_string(
      cover(temp), src, std::make_index_sequence<M>(), engine);
    return basic_identifier<M, B, CharSet, UIntT, V>{temp};
}
//------------------------------------------------------------------------------
template <typename Engine>
auto get_random_identifier(Engine& engine) -> identifier {
    return make_random_basic_identifier(
      std::type_identity<identifier>(), engine);
}
//------------------------------------------------------------------------------
template <typename Engine>
auto get_random_identifier(const span<const char> src, Engine& engine)
  -> identifier {
    return make_random_basic_identifier(
      std::type_identity<identifier>(), src, engine);
}
//------------------------------------------------------------------------------
auto random_identifier(std::default_random_engine& re) -> identifier {
    return get_random_identifier(re);
}
//------------------------------------------------------------------------------
auto random_identifier(const string_view charset, std::default_random_engine& re)
  -> identifier {
    return get_random_identifier(charset, re);
}
//------------------------------------------------------------------------------
auto random_identifier() -> identifier {
    std::random_device rd;
    std::default_random_engine re{rd()};
    return random_identifier(re);
}
//------------------------------------------------------------------------------
auto random_identifier(const string_view charset) -> identifier {
    std::random_device rd;
    std::default_random_engine re{rd()};
    return random_identifier(charset, re);
}
//------------------------------------------------------------------------------
} // namespace eagine
