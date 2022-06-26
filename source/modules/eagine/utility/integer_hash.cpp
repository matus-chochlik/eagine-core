/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:integer_hash;

import <cstdint>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
template <std::size_t N, typename I>
constexpr auto integer_rotate_right(
  const I x,
  const std::type_identity<I> = {},
  const std::integral_constant<std::size_t, N> = {}) noexcept -> I {
    static_assert(N < sizeof(I) * 8);
    return I(x << N) | I(x >> (-N & (sizeof(I) * 8U - 1U)));
}
//------------------------------------------------------------------------------
template <typename H, typename I>
constexpr auto integer_hash_init(
  const I x,
  const std::type_identity<H> = {},
  const std::type_identity<I> = {}) noexcept -> H {
    using std::is_same_v;

    using UI = std::make_unsigned_t<I>;
    UI ux(x);

    if constexpr(sizeof(H) > sizeof(UI)) {
        H result{};
        for(std::size_t i = 0; i < sizeof(H) / sizeof(UI); ++i) {
            result <<= 8U * sizeof(UI);
            result ^= H(ux);
        }
        return result;
    } else if constexpr(sizeof(H) < sizeof(UI)) {
        H result{};
        for(std::size_t i = 0; i < sizeof(UI) / sizeof(H); ++i) {
            result ^= H(ux);
            ux >>= 8U * sizeof(H);
        }
        return result;
    } else {
        return ux;
    }
}
//------------------------------------------------------------------------------
/// @brief Creates a hash value with type @p H from an integer value of type @p I.
/// @ingroup type_utils
export template <typename H, typename I>
auto integer_hash(
  const I x,
  const std::type_identity<H> hid = {},
  const std::type_identity<I> iid = {}) -> H {
    using std::is_same_v;
    if constexpr(std::is_unsigned_v<H>) {
        auto h = integer_hash_init(x, hid, iid);
        if constexpr(is_same_v<H, std::uint16_t>) {
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xff51;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xafd7;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xed55;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0x8ccd;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xc4ce;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xb9fe;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0x1a85;
            h ^= integer_rotate_right<9U>(h, hid);
            h *= 0xec53;
            h ^= integer_rotate_right<9U>(h, hid);
        }
        if constexpr(is_same_v<H, std::uint32_t>) {
            h ^= integer_rotate_right<17U>(h, hid);
            h *= 0xff51afd7;
            h ^= integer_rotate_right<17U>(h, hid);
            h *= 0xed558ccd;
            h ^= integer_rotate_right<17U>(h, hid);
            h *= 0xc4ceb9fe;
            h ^= integer_rotate_right<17U>(h, hid);
            h *= 0x1a85ec53;
            h ^= integer_rotate_right<17U>(h, hid);
        }
        if constexpr(is_same_v<H, std::uint64_t>) {
            h ^= integer_rotate_right<33U>(h, hid);
            h *= 0xff51afd7ed558ccd;
            h ^= integer_rotate_right<33U>(h, hid);
            h *= 0xed558ccdff51afd7;
            h ^= integer_rotate_right<33U>(h, hid);
            h *= 0xc4ceb9fe1a85ec53;
            h ^= integer_rotate_right<33U>(h, hid);
            h *= 0x1a85ec53c4ceb9fe;
            h ^= integer_rotate_right<33U>(h, hid);
        }
        return h;
    }

    using UH = std::make_unsigned_t<H>;
    return H(integer_hash<UH>(x));
}
//------------------------------------------------------------------------------
} // namespace eagine

