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
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto make_byte_span(const span<char> s) noexcept
  -> span<byte> {
    return memory::accommodate<byte>(s);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_cbyte_span(const span<const char> s) noexcept
  -> span<const byte> {
    return memory::accommodate<const byte>(s);
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto max_code_point(
  const valid_sequence_length& len) noexcept
  -> valid_if_not_zero<code_point_t> {
    switch(len.value_or(0)) {
        case 1: //  7b
            return 0x0000007FU;
        case 2: // 11b
            return 0x000007FFU;
        case 3: // 16b
            return 0x0000FFFFU;
        case 4: // 21b
            return 0x001FFFFFU;
        case 5: // 26b
            return 0x03FFFFFFU;
        case 6: // 31b
            return 0x7FFFFFFFU;
        default:
            return 0x00000000U;
    }
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_data_bitshift(
  const valid_sequence_length& len) noexcept
  -> valid_if_nonnegative<span_size_t> {
    return {len ? (*len - 1) * 6 : -1};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_data_bitshift(
  const valid_sequence_length& idx,
  const valid_sequence_length& len) noexcept
  -> valid_if_nonnegative<span_size_t> {
    return {(idx and len) ? (*len - *idx - 1) * 6 : -1};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_code_mask(
  const valid_sequence_length& len) noexcept -> valid_if_not_zero<byte> {
    switch(len.value_or(0)) {
        case 1:
            return 0x80U;
        case 2:
            return 0xE0U;
        case 3:
            return 0xF0U;
        case 4:
            return 0xF8U;
        case 5:
            return 0xFCU;
        case 6:
            return 0xFEU;
        [[unlikely]] default:
            return 0x00U;
    }
}
//------------------------------------------------------------------------------
template <typename P>
[[nodiscard]] static constexpr auto inverted_byte(
  const valid_if<byte, P> b) noexcept -> optionally_valid<byte> {
    return {byte(~b.value_anyway()), b.has_value()};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_data_mask(
  const valid_sequence_length& len) noexcept -> optionally_valid<byte> {
    return inverted_byte(head_code_mask(len));
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_code_mask() noexcept
  -> always_valid<byte> {
    return 0xC0U;
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_data_mask() noexcept
  -> always_valid<byte> {
    return 0x3FU;
}
//------------------------------------------------------------------------------
template <typename P>
[[nodiscard]] static constexpr auto head_code_from_mask(
  const valid_if<byte, P> mask) noexcept -> optionally_valid<byte> {
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    return {byte((mask.value_anyway() << 1U) & 0xFFU), mask.has_value()};
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto head_code(
  const valid_sequence_length& len) noexcept -> optionally_valid<byte> {
    return head_code_from_mask(head_code_mask(len));
}
//------------------------------------------------------------------------------
[[nodiscard]] static constexpr auto tail_code() noexcept -> always_valid<byte> {
    return 0x80U;
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
[[nodiscard]] static constexpr auto is_valid_masked_code(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<byte, P2> code) noexcept -> bool {
    return (mask and code) and (b & *mask) == *code;
}
//------------------------------------------------------------------------------
[[nodiscard]] constexpr auto is_valid_head_byte(
  const byte b,
  const valid_sequence_length& l) noexcept -> bool {
    const auto mask{head_code_mask(l)};
    return is_valid_masked_code(b, mask, head_code_from_mask(mask));
}
//------------------------------------------------------------------------------
template <span_size_t L>
[[nodiscard]] constexpr auto is_valid_head_byte_c(const byte b) noexcept
  -> bool {
    static const auto mask{head_code_mask(L)};
    return is_valid_masked_code(b, mask, head_code_from_mask(mask));
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto is_valid_head_byte(const byte b) noexcept
  -> bool {
    return is_valid_head_byte_c<1>(b) or is_valid_head_byte_c<2>(b) or
           is_valid_head_byte_c<3>(b) or is_valid_head_byte_c<4>(b) or
           is_valid_head_byte_c<5>(b) or is_valid_head_byte_c<6>(b);
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
    return is_valid_head_byte_c<1>(b)   ? 1
           : is_valid_head_byte_c<2>(b) ? 2
           : is_valid_head_byte_c<3>(b) ? 3
           : is_valid_head_byte_c<4>(b) ? 4
           : is_valid_head_byte_c<5>(b) ? 5
           : is_valid_head_byte_c<6>(b) ? 6
                                        : 0;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decode_sequence_length(
  const span<const byte> seq) noexcept -> valid_sequence_length {
    return do_decode_sequence_length(byte(seq[0]));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto is_valid_encoding(const span<const byte> seq) noexcept
  -> bool {
    if(const auto len{decode_sequence_length(seq)}) {
        const span_size_t l = len.value_anyway();

        if(not is_valid_head_byte(seq[0], l)) {
            return false;
        }
        if(l > seq.size()) {
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
            (mask.has_value() and bitshift.has_value())};
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
            (mask.has_value() and bitshift.has_value())};
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
  const span<const byte>& src,
  const valid_sequence_length& vl) noexcept -> code_point {
    if(vl.has_value()) [[likely]] {
        const span_size_t l = vl.value_anyway();
        if(l - 1 < src.size()) [[likely]] {
            if(const auto h{decode_code_point_head(src[0], vl)}) [[likely]] {
                code_point_t cp{h.value_anyway()};

                for(const auto i : integer_range(1, l)) {
                    if(const auto t{decode_code_point_tail(src[i], i, vl)})
                      [[likely]] {
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
      (code.has_value() and mask.has_value() and bitshift.has_value())};
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
  span<byte> dest,
  const valid_sequence_length& vl) noexcept -> bool {

    if(cp and vl) {
        const span_size_t l = vl.value_anyway();
        if(l - 1 < dest.size()) {
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
  const span<byte>& dest) noexcept -> valid_sequence_length {
    const auto len{required_sequence_length(cp.value())};
    do_encode_code_point(cp, dest, len);
    return len; // NOLINT(performance-no-automatic-move)
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto encode_code_point(const code_point& cp)
  -> valid_if_not_empty<std::string> {
    if(const auto len{required_sequence_length(cp.value())}) {
        std::array<byte, 7> tmp{};
        do_encode_code_point(cp, cover(tmp), len.value());
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
export [[nodiscard]] auto decode_code_point(const span<const byte>& src) noexcept
  -> code_point {
    return do_decode_code_point(src, decode_sequence_length(src));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decoding_code_points_required(
  const span<const byte> bytes) noexcept -> optionally_valid<span_size_t> {
    span_size_t result = 0;

    auto i = bytes.begin();
    const auto e = bytes.end();

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
  const span<byte> bytes) noexcept -> bool {
    span_size_t i = 0;

    for(const code_point& cp : cps) {
        if(not cp.has_value()) {
            return false;
        }
        if(i >= bytes.size()) {
            return false;
        }

        span<byte> sub{bytes.data() + i, bytes.size() - i};
        const auto len{encode_code_point(cp.value(), sub)};

        if(not len.has_value()) {
            return false;
        }

        i += span_size(len.value());
    }
    return true;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto decode_code_points(
  const span<const byte>& bytes,
  span<code_point> cps) noexcept -> bool {
    span_size_t i = 0;

    for(code_point& cp : cps) {
        if(i >= bytes.size()) [[unlikely]] {
            return false;
        }
        const span<const byte> sub{bytes.data() + i, bytes.size() - i};
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
