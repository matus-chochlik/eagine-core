/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:base64;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
export constexpr auto make_base64_encode_transform() {
    return [](const auto b) -> always_valid<char> {
        const auto i = int(b);
        if(i < 26) {
            return {char('A' + i)};
        }
        if(i < 52) {
            return {char('a' + i - 26)};
        }
        if(i < 62) {
            return {char('0' + i - 52)};
        }
        if(i == 62) {
            return {'+'};
        }
        if(i == 63) {
            return {'/'};
        }
        return {'='};
    };
}
//------------------------------------------------------------------------------
export constexpr auto make_base64_decode_transform() {
    return [](const char c) -> optionally_valid<byte> {
        if((c >= 'A') && (c <= 'Z')) {
            return {byte(c - 'A'), true};
        }
        if((c >= 'a') && (c <= 'z')) {
            return {byte(c - 'a' + 26), true};
        }
        if((c >= '0') && (c <= '9')) {
            return {byte(c - '0' + 52), true};
        }
        if(c == '+') {
            return {byte(62), true};
        }
        if(c == '/') {
            return {byte(63), true};
        }
        return {};
    };
}
//------------------------------------------------------------------------------
export auto base64_encoded_length(const span_size_t orig_size) noexcept {
    return dissolved_bits_length(orig_size, 6);
}
//------------------------------------------------------------------------------
export auto base64_decoded_length(const span_size_t orig_size) noexcept {
    return concentrated_bits_length(orig_size, 6);
}
//------------------------------------------------------------------------------
export template <typename Ps, typename Ss, typename Pd, typename Sd>
auto base64_encode(
  const memory::basic_span<const byte, Ps, Ss> src,
  memory::basic_span<char, Pd, Sd> dst) -> memory::basic_span<char, Pd, Sd> {
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_dissolve_bits(
         make_span_getter(i, src),
         make_span_putter(o, dst, make_base64_encode_transform()),
         6)) {
        return head(dst, o);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename P, typename S, typename Dst>
auto base64_encode(const memory::basic_span<const byte, P, S> src, Dst& dst)
  -> optional_reference_wrapper<Dst> {
    using Ds = typename Dst::size_type;
    dst.resize(Ds(base64_encoded_length(src.size())));
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_dissolve_bits(
         memory::make_span_getter(i, src),
         memory::make_span_putter(o, dst, make_base64_encode_transform()),
         6)) {
        dst.resize(Ds(o));
        return {dst};
    }
    return {nothing};
}
//------------------------------------------------------------------------------
export template <typename Ps, typename Ss, typename Pd, typename Sd>
auto base64_decode(
  const memory::basic_span<const char, Ps, Ss> src,
  memory::basic_span<byte, Pd, Sd> dst) -> memory::basic_span<byte, Pd, Sd> {
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_concentrate_bits(
         memory::make_span_getter(i, src, make_base64_decode_transform()),
         memory::make_span_putter(o, dst),
         6)) {
        return head(dst, o);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename P, typename S, typename Dst>
auto base64_decode(const memory::basic_span<const char, P, S> src, Dst& dst)
  -> optional_reference_wrapper<Dst> {
    using Ds = typename Dst::size_type;
    dst.resize(Ds(base64_decoded_length(src.size())));
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_concentrate_bits(
         memory::make_span_getter(i, src, make_base64_decode_transform()),
         memory::make_span_putter(o, dst),
         6)) {
        dst.resize(Ds(o));
        return {dst};
    }
    return {nothing};
}
//------------------------------------------------------------------------------
} // namespace eagine
