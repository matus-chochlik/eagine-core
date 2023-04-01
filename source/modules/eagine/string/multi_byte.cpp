/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.string:multi_byte;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import eagine.core.utility;

namespace eagine::multi_byte {
//------------------------------------------------------------------------------
export using code_point_t = std::uint32_t;
export constexpr code_point_t invalid_code_point = 0x7FFFFFFFU;
export using code_point = valid_if_less_than<code_point_t, 0x7FFFFFFFU>;
//------------------------------------------------------------------------------
export using valid_sequence_length = valid_if_between<span_size_t, 1, 6>;
export using valid_byte_span = valid_if_size_gt<span<byte>, span_size_t>;
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_byte_span(const span<byte> s) noexcept
  -> valid_byte_span {
    return {s};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_byte_span(const span<char> s) noexcept
  -> valid_byte_span {
    return {memory::accommodate<byte>(s)};
}
//------------------------------------------------------------------------------
using valid_cbyte_span = valid_if_size_gt<span<const byte>, span_size_t>;
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_cbyte_span(const span<const byte> s) noexcept
  -> valid_cbyte_span {
    return {s};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_cbyte_span(const span<const char> s) noexcept
  -> valid_cbyte_span {
    return {memory::accommodate<const byte>(s)};
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto max_code_point(
  const valid_sequence_length& len) noexcept
  -> valid_if_not_zero<code_point_t> {
    return len == 1 ? 0x0000007F : //  7b
             len == 2 ? 0x000007FF
                      :            // 11b
             len == 3 ? 0x0000FFFF
                      :            // 16b
             len == 4 ? 0x001FFFFF
                      :            // 21b
             len == 5 ? 0x03FFFFFF
                      :            // 26b
             len == 6 ? 0x7FFFFFFF
                      :            // 31b
             0x00000000;
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_data_bitshift(
  const valid_sequence_length& len) noexcept
  -> valid_if_nonnegative<span_size_t> {
    return {len.is_valid() ? (extract(len) - 1) * 6 : -1};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_data_bitshift(
  const valid_sequence_length& idx,
  const valid_sequence_length& len) noexcept
  -> valid_if_nonnegative<span_size_t> {
    return {
      (idx.is_valid() and len.is_valid())
        ? (extract(len) - extract(idx) - 1) * 6
        : -1};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_code_mask(
  const valid_sequence_length& len) noexcept -> valid_if_not_zero<byte> {
    return len == 1   ? 0x80
           : len == 2 ? 0xE0
           : len == 3 ? 0xF0
           : len == 4 ? 0xF8
           : len == 5 ? 0xFC
           : len == 6 ? 0xFE
                      : 0x00;
}
//------------------------------------------------------------------------------
template <typename P>
[[nodiscard]] static constexpr auto inverted_byte(
  const valid_if<byte, P> b) noexcept -> optionally_valid<byte> {
    return {byte(~b.value_anyway()), b.is_valid()};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_data_mask(
  const valid_sequence_length& len) noexcept -> optionally_valid<byte> {
    return inverted_byte(head_code_mask(len));
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_code_mask() noexcept
  -> always_valid<byte> {
    return 0xC0;
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_data_mask() noexcept
  -> always_valid<byte> {
    return 0x3F;
}
//------------------------------------------------------------------------------
template <typename P>
[[nodiscard]] static constexpr auto head_code_from_mask(
  const valid_if<byte, P> mask) noexcept -> optionally_valid<byte> {
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    return {byte((mask.value_anyway() << 1) & 0xFF), mask.is_valid()};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_code(
  const valid_sequence_length& len) noexcept -> optionally_valid<byte> {
    return head_code_from_mask(head_code_mask(len));
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_code() noexcept -> always_valid<byte> {
    return 0x80;
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
[[nodiscard]] static constexpr auto is_valid_masked_code(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<byte, P2> code) noexcept -> bool {
    return (mask.is_valid() and code.is_valid())
             ? (b & extract(mask)) == extract(code)
             : false;
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto is_valid_head_byte(
  const byte b,
  const valid_sequence_length& l) noexcept -> bool {
    return is_valid_masked_code(b, head_code_mask(l), head_code(l));
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto is_valid_head_byte(const byte b) noexcept
  -> bool {
    return is_valid_head_byte(b, 1) or is_valid_head_byte(b, 2) or
           is_valid_head_byte(b, 3) or is_valid_head_byte(b, 4) or
           is_valid_head_byte(b, 5) or is_valid_head_byte(b, 6);
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto is_valid_tail_byte(
  const byte b,
  const valid_sequence_length&,
  const valid_sequence_length&) noexcept -> bool {
    return is_valid_masked_code(b, tail_code_mask(), tail_code());
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto required_sequence_length(
  const code_point_t cp) noexcept -> valid_sequence_length {
    return (max_code_point(1) > cp)   ? 1
           : (max_code_point(2) > cp) ? 2
           : (max_code_point(3) > cp) ? 3
           : (max_code_point(4) > cp) ? 4
           : (max_code_point(5) > cp) ? 5
           : (max_code_point(6) > cp) ? 6
                                      : 0;
}
//------------------------------------------------------------------------------
[[nodiscard]] auto do_decode_sequence_length(const byte b) noexcept
  -> valid_sequence_length {
    for(const auto l : integer_range(1, 7)) {
        if(is_valid_head_byte(b, l)) {
            return l;
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decode_sequence_length(
  const valid_cbyte_span& seq) noexcept -> valid_sequence_length {
    return do_decode_sequence_length(byte(seq.value(0)[0]));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto is_valid_encoding(
  const valid_cbyte_span& vseq) noexcept -> bool {
    if(const auto len{decode_sequence_length(vseq)}) {
        const span_size_t l = len.value_anyway();
        span<const byte> seq = vseq.value_anyway();

        if(not is_valid_head_byte(seq[0], l)) {
            return false;
        }

        for(const auto i : integer_range(1, l)) {
            if(not is_valid_tail_byte(seq[i], i, l)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
[[nodiscard]] constexpr auto do_decode_code_point_head(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<span_size_t, P2> bitshift) noexcept
  -> optionally_valid<code_point_t> {

    return {// NOLINTNEXTLINE(hicpp-signed-bitwise)
            code_point_t((b & mask.value_anyway()) << bitshift.value_anyway()),
            (mask.is_valid() and bitshift.is_valid())};
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto decode_code_point_head(
  const byte b,
  const valid_sequence_length& l) noexcept -> optionally_valid<code_point_t> {
    return do_decode_code_point_head(
      b, head_data_mask(l), head_data_bitshift(l));
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
[[nodiscard]] constexpr auto do_decode_code_point_tail(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<span_size_t, P2> bitshift) noexcept
  -> optionally_valid<code_point_t> {

    return {// NOLINTNEXTLINE(hicpp-signed-bitwise)
            code_point_t((b & mask.value_anyway()) << bitshift.value_anyway()),
            (mask.is_valid() and bitshift.is_valid())};
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto decode_code_point_tail(
  const byte b,
  const valid_sequence_length& i,
  const valid_sequence_length& l) noexcept -> optionally_valid<code_point_t> {
    return do_decode_code_point_tail(
      b, tail_data_mask(), tail_data_bitshift(i, l));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto do_decode_code_point(
  const valid_cbyte_span& vsrc,
  const valid_sequence_length& vl) noexcept -> code_point {
    if(vl.is_valid()) {
        const span_size_t l = vl.value_anyway();
        if(vsrc.is_valid(l - 1)) {
            span<const byte> src = vsrc.value_anyway();

            if(const auto h = decode_code_point_head(src[0], vl)) {
                code_point_t cp = h.value_anyway();

                for(const auto i : integer_range(1, l)) {
                    if(const auto t{decode_code_point_tail(src[i], i, vl)}) {
                        cp |= t.value_anyway();
                    } else {
                        return {invalid_code_point};
                    }
                }
                return cp;
            }
        }
    }
    return {invalid_code_point};
}
//------------------------------------------------------------------------------
template <typename P1, typename P2, typename P3>
[[nodiscard]] constexpr auto do_encode_code_point_byte(
  const code_point_t cp,
  const valid_if<byte, P1> code,
  const valid_if<byte, P2> mask,
  const valid_if<span_size_t, P3> bitshift) noexcept -> optionally_valid<byte> {
    return {
      byte(
        (code.value_anyway()) |
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        (mask.value_anyway() & (cp >> bitshift.value_anyway()))),
      (code.is_valid() and mask.is_valid() and bitshift.is_valid())};
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto encode_code_point_head(
  const code_point_t cp,
  const valid_sequence_length& l) noexcept -> optionally_valid<byte> {
    return do_encode_code_point_byte(
      cp, head_code(l), head_data_mask(l), head_data_bitshift(l));
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto encode_code_point_tail(
  const code_point_t cp,
  const valid_sequence_length& i,
  const valid_sequence_length& l) noexcept -> optionally_valid<byte> {
    return do_encode_code_point_byte(
      cp, tail_code(), tail_data_mask(), tail_data_bitshift(i, l));
}
//------------------------------------------------------------------------------
export auto do_encode_code_point(
  const code_point& cp,
  const valid_byte_span& vdest,
  const valid_sequence_length& vl) noexcept -> bool {

    if(cp and vl) {
        const span_size_t l = vl.value_anyway();
        if(vdest.is_valid(l - 1)) {
            span<byte> dest = vdest.value_anyway();

            const code_point_t val = cp.value_anyway();

            if(const auto h = encode_code_point_head(val, vl)) {
                dest[0] = h.value_anyway();

                for(const auto i : integer_range(1, l)) {
                    if(const auto t{encode_code_point_tail(val, i, vl)}) {
                        dest[i] = t.value_anyway();
                    } else {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto encode_code_point(
  const code_point& cp,
  const valid_byte_span& dest) noexcept -> valid_sequence_length {
    const auto len{required_sequence_length(cp.value())};
    do_encode_code_point(cp, dest, len);
    return len; // NOLINT(performance-no-automatic-move)
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto encode_code_point(const code_point& cp)
  -> valid_if_not_empty<std::string> {
    if(const auto len{required_sequence_length(cp.value())}) {
        std::array<byte, 7> tmp{};
        do_encode_code_point(cp, make_byte_span(cover(tmp)), len.value());
        return {std::string(
          reinterpret_cast<const char*>(tmp.data()), integer(len.value()))};
    }
    return {};
}
//------------------------------------------------------------------------------
export auto encode_nil(span<byte> dest) noexcept -> valid_sequence_length {
    if(dest) {
        dest.front() = 0xFFU;
        return 1;
    }
    return 0;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto encoding_bytes_required(
  span<const code_point> cps) noexcept -> optionally_valid<span_size_t> {
    span_size_t result = 0;
    for(const auto cp : cps) {
        if(const auto len{required_sequence_length(cp.value())}) {
            result += len.value_anyway();
        } else {
            return {0, false};
        }
    }
    return {result, true};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decode_code_point(const valid_cbyte_span& src) noexcept
  -> code_point {
    return do_decode_code_point(src, decode_sequence_length(src));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decoding_code_points_required(
  const valid_cbyte_span& bytes) noexcept -> optionally_valid<span_size_t> {
    span_size_t result = 0;

    auto i = bytes.value(0).begin();
    const auto e = bytes.value(0).end();

    while(i != e) {
        if(const auto len{do_decode_sequence_length(*i)}) {
            ++result;
            i += span_size(len.value());
            assert(not(i > e));
        } else {
            return {0, false};
        }
    }

    return {result, true};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto encode_code_points(
  span<const code_point> cps,
  const valid_byte_span& bytes) noexcept -> bool {
    span_size_t i = 0;

    for(const code_point& cp : cps) {
        if(not cp.is_valid()) {
            return false;
        }

        span<byte> sub{bytes.value(i).data() + i, bytes.value(i).size() - i};
        const auto len{encode_code_point(cp.value(), sub)};

        if(not len.is_valid()) {
            return false;
        }

        i += span_size(len.value());
    }
    return true;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decode_code_points(
  const valid_cbyte_span& bytes,
  span<code_point> cps) -> bool {
    span_size_t i = 0;

    for(code_point& cp : cps) {
        const span<const byte> sub{
          bytes.value(i).data() + i, bytes.value(i).size() - i};
        if(const auto len{decode_sequence_length(sub)}) {
            if(const auto tcp{do_decode_code_point(sub, len.value())}) {
                cp = tcp;
                i += span_size(len.value());
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}
//------------------------------------------------------------------------------
} // namespace eagine::multi_byte
