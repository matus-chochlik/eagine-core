/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/assert.hpp>
#include <array>

namespace eagine::mbs {
//------------------------------------------------------------------------------
inline auto do_decode_sequence_length(const byte b) noexcept
  -> valid_sequence_length {
    for(span_size_t l = 1; l <= 6; ++l) {
        if(is_valid_head_byte(b, l)) {
            return l;
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
inline auto is_valid_encoding(const valid_cbyte_span& vseq) noexcept -> bool {
    if(auto len = decode_sequence_length(vseq)) {
        const span_size_t l = len.value_anyway();
        span<const byte> seq = vseq.value_anyway();

        if(!is_valid_head_byte(seq[0], l)) {
            return false;
        }

        for(span_size_t i = 1; i < l; ++i) {
            if(!is_valid_tail_byte(seq[i], i, l)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
static constexpr auto do_decode_code_point_head(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<span_size_t, P2> bitshift) noexcept
  -> optionally_valid<code_point_t> {

    return {// NOLINTNEXTLINE(hicpp-signed-bitwise)
            code_point_t((b & mask.value_anyway()) << bitshift.value_anyway()),
            (mask.is_valid() && bitshift.is_valid())};
}
//------------------------------------------------------------------------------
static constexpr auto decode_code_point_head(
  const byte b,
  const valid_sequence_length& l) noexcept -> optionally_valid<code_point_t> {
    return do_decode_code_point_head(
      b, head_data_mask(l), head_data_bitshift(l));
}
//------------------------------------------------------------------------------
template <typename P1, typename P2>
static constexpr auto do_decode_code_point_tail(
  const byte b,
  const valid_if<byte, P1> mask,
  const valid_if<span_size_t, P2> bitshift) noexcept
  -> optionally_valid<code_point_t> {

    return {// NOLINTNEXTLINE(hicpp-signed-bitwise)
            code_point_t((b & mask.value_anyway()) << bitshift.value_anyway()),
            (mask.is_valid() && bitshift.is_valid())};
}
//------------------------------------------------------------------------------
static constexpr auto decode_code_point_tail(
  const byte b,
  const valid_sequence_length& i,
  const valid_sequence_length& l) noexcept -> optionally_valid<code_point_t> {
    return do_decode_code_point_tail(
      b, tail_data_mask(), tail_data_bitshift(i, l));
}
//------------------------------------------------------------------------------
inline auto do_decode_code_point(
  const valid_cbyte_span& vsrc,
  const valid_sequence_length& vl) noexcept -> code_point {
    if(vl.is_valid()) {
        const span_size_t l = vl.value_anyway();
        if(vsrc.is_valid(l - 1)) {
            span<const byte> src = vsrc.value_anyway();

            if(const auto h = decode_code_point_head(src[0], vl)) {
                code_point_t cp = h.value_anyway();

                for(span_size_t i = 1; i < l; ++i) {
                    if(const auto t = decode_code_point_tail(src[i], i, vl)) {
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
static constexpr auto do_encode_code_point_byte(
  const code_point_t cp,
  const valid_if<byte, P1> code,
  const valid_if<byte, P2> mask,
  const valid_if<span_size_t, P3> bitshift) noexcept -> optionally_valid<byte> {
    return {
      byte(
        (code.value_anyway()) |
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        (mask.value_anyway() & (cp >> bitshift.value_anyway()))),
      (code.is_valid() && mask.is_valid() && bitshift.is_valid())};
}
//------------------------------------------------------------------------------
static constexpr auto encode_code_point_head(
  const code_point_t cp,
  const valid_sequence_length& l) noexcept -> optionally_valid<byte> {
    return do_encode_code_point_byte(
      cp, head_code(l), head_data_mask(l), head_data_bitshift(l));
}
//------------------------------------------------------------------------------
static constexpr auto encode_code_point_tail(
  const code_point_t cp,
  const valid_sequence_length& i,
  const valid_sequence_length& l) noexcept -> optionally_valid<byte> {
    return do_encode_code_point_byte(
      cp, tail_code(), tail_data_mask(), tail_data_bitshift(i, l));
}
//------------------------------------------------------------------------------
inline auto do_encode_code_point(
  const code_point& cp,
  const valid_byte_span& vdest,
  const valid_sequence_length& vl) noexcept -> bool {

    if(cp && vl) {
        const span_size_t l = vl.value_anyway();
        if(vdest.is_valid(l - 1)) {
            span<byte> dest = vdest.value_anyway();

            code_point_t val = cp.value_anyway();

            if(const auto h = encode_code_point_head(val, vl)) {
                dest[0] = h.value_anyway();

                for(span_size_t i = 1; i < l; ++i) {
                    if(const auto t = encode_code_point_tail(val, i, vl)) {
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
inline auto encode_code_point(
  const code_point& cp,
  const valid_byte_span& dest) noexcept -> valid_sequence_length {
    auto len = required_sequence_length(cp.value());
    do_encode_code_point(cp, dest, len);
    return len;
}
//------------------------------------------------------------------------------
inline auto encode_code_point(const code_point& cp)
  -> valid_if_not_empty<std::string> {
    if(auto len = required_sequence_length(cp.value())) {
        std::array<byte, 7> tmp{};
        do_encode_code_point(cp, make_byte_span(cover(tmp)), len.value());
        return {std::string(
          reinterpret_cast<const char*>(tmp.data()), std_size(len.value()))};
    }
    return {};
}
//------------------------------------------------------------------------------
inline auto encoding_bytes_required(span<const code_point_t> cps) noexcept
  -> optionally_valid<span_size_t> {
    span_size_t result = 0;
    for(code_point_t cp : cps) {
        if(const auto len = required_sequence_length(cp)) {
            result += len.value_anyway();
        } else {
            return {0, false};
        }
    }
    return {result, true};
}
//------------------------------------------------------------------------------
inline auto encoding_bytes_required(span<const code_point> cps) noexcept
  -> optionally_valid<span_size_t> {
    span_size_t result = 0;
    for(code_point cp : cps) {
        if(cp.is_valid()) {
            if(const auto len = required_sequence_length(cp.value())) {
                result += len.value_anyway();
            } else {
                return {0, false};
            }
        } else {
            return {0, false};
        }
    }
    return {result, true};
}
//------------------------------------------------------------------------------
inline auto decoding_code_points_required(const valid_cbyte_span& bytes) noexcept
  -> optionally_valid<span_size_t> {
    span_size_t result = 0;

    auto i = bytes.value(0).begin();
    auto e = bytes.value(0).end();

    while(i != e) {
        if(auto len = do_decode_sequence_length(*i)) {
            ++result;
            i += span_size(len.value());
            EAGINE_ASSERT(!(i > e));
        } else {
            return {0, false};
        }
    }

    return {result, true};
}
//------------------------------------------------------------------------------
inline auto encode_code_points(
  span<const code_point> cps,
  const valid_byte_span& bytes) noexcept -> bool {
    span_size_t i = 0;

    for(code_point cp : cps) {
        if(!cp.is_valid()) {
            return false;
        }

        span<byte> sub{bytes.value(i).data() + i, bytes.value(i).size() - i};
        auto len = encode_code_point(cp.value(), sub);

        if(!len.is_valid()) {
            return false;
        }

        i += span_size(len.value());
    }
    return true;
}
//------------------------------------------------------------------------------
inline auto decode_code_points(
  const valid_cbyte_span& bytes,
  span<code_point> cps) -> bool {
    span_size_t i = 0;

    for(code_point& cp : cps) {
        span<const byte> sub{
          bytes.value(i).data() + i, bytes.value(i).size() - i};
        if(auto len = decode_sequence_length(sub)) {
            if(const auto tcp = do_decode_code_point(sub, len.value())) {
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
} // namespace eagine::mbs
