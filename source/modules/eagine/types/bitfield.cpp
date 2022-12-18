/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:bitfield;

import <concepts>;
import <type_traits>;

namespace eagine {

/// @brief Class for manipulating and testing a group of enumeration-based bits.
/// @ingroup type_utils
export template <typename Bit>
class bitfield {
public:
    /// @brief The enumeration type specifying individual bit values.
    using bit_type = Bit;

    /// @brief The Integral type used to store the bits.
    using value_type = std::underlying_type_t<bit_type>;

    static_assert(std::is_unsigned_v<value_type>);

    /// @brief Default constructor.
    constexpr bitfield() noexcept = default;

    /// @brief Explicit construction from the underlying interger type value.
    constexpr explicit bitfield(const value_type bits) noexcept
      : _bits{bits} {}

    /// @brief Construction from the bit enumeration type value.
    constexpr bitfield(const bit_type _bit) noexcept
      : _bits{value_type(_bit)} {}

    constexpr bitfield(const bit_type _bit_a, const bit_type _bit_b) noexcept
      : _bits(value_type(_bit_a) | value_type(_bit_b)) {}

    /// @brief Indicates that none of the bits are set.
    constexpr auto is_empty() const noexcept {
        return _bits == value_type(0);
    }

    /// @brief Indicates that some of the bits are set.
    /// @see is_empty
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return _bits != value_type(0);
    }

    /// @brief Explicit conversion to the underlying integer value type.
    /// @see bits
    [[nodiscard]] explicit constexpr operator value_type() const noexcept {
        return _bits;
    }

    /// @brief Comparison operator.
    [[nodiscard]] constexpr auto operator<=>(const bitfield&) const noexcept =
      default;

    /// @brief Returns the bits in the underlying integer value type.
    [[nodiscard]] constexpr auto bits() const noexcept -> value_type {
        return _bits;
    }

    /// @brief Tests if the specified bit is set.
    /// @see has_not
    /// @see has_only
    /// @see has_all
    /// @see has_any
    /// @see has_none
    /// @see has_at_most
    [[nodiscard]] constexpr auto has(const bit_type bit) const noexcept {
        return (_bits & value_type(bit)) == value_type(bit);
    }

    /// @brief Tests if the specified bit is not set.
    /// @see has
    /// @see has_all
    /// @see has_any
    /// @see has_only
    /// @see has_none
    /// @see has_at_most
    [[nodiscard]] constexpr auto has_not(const bit_type bit) const noexcept {
        return (_bits & value_type(bit)) != value_type(bit);
    }

    /// @brief Tests if all of the specified bits are set.
    /// @see has
    /// @see has_not
    /// @see has_only
    /// @see has_any
    /// @see has_none
    /// @see has_at_most
    template <std::same_as<bit_type>... B>
    [[nodiscard]] constexpr auto has_all(const bit_type bit, B... bits)
      const noexcept -> bool {
        return (has(bit) && ... && has(bits));
    }

    /// @brief Tests if any of the specified bits are set.
    /// @see has
    /// @see has_not
    /// @see has_only
    /// @see has_all
    /// @see has_none
    /// @see has_at_most
    template <std::same_as<bit_type>... B>
    [[nodiscard]] constexpr auto has_any(const bit_type bit, B... bits)
      const noexcept -> bool {
        return (has(bit) || ... || has(bits));
    }

    /// @brief Tests if none of the specified bits are set.
    /// @see has
    /// @see has_not
    /// @see has_only
    /// @see has_all
    /// @see has_any
    /// @see has_at_most
    template <std::same_as<bit_type>... B>
    [[nodiscard]] constexpr auto has_none(const bit_type bit, B... bits)
      const noexcept -> bool {
        return (has_not(bit) && ... && has_not(bits));
    }

    /// @brief Tests if only the specified bit is set.
    /// @see has
    /// @see has_not
    /// @see has_all
    /// @see has_any
    /// @see has_none
    /// @see has_at_most
    [[nodiscard]] constexpr auto has_only(const bit_type bit) const noexcept {
        return _bits == value_type(bit);
    }

    /// @brief Tests if at most the specified bit is set (or is empty).
    /// @see has
    /// @see has_not
    /// @see has_only
    /// @see has_all
    /// @see has_any
    /// @see has_none
    [[nodiscard]] constexpr auto has_at_most(const bit_type bit) const noexcept {
        return is_empty() || has_only(bit);
    }

    /// @brief Bitwise-or operator.
    auto operator|=(const bitfield b) noexcept -> bitfield& {
        _bits |= b._bits;
        return *this;
    }

    /// @brief Bitwise-or operator.
    [[nodiscard]] auto operator|(const bitfield b) const noexcept -> bitfield {
        return bitfield{value_type(_bits | b._bits)};
    }

    /// @brief Bitwise-or operator.
    [[nodiscard]] auto operator|(const Bit b) const noexcept -> bitfield {
        return *this | bitfield{b};
    }

    /// @brief Bitwise-and operator.
    auto operator&=(const bitfield b) noexcept -> bitfield& {
        _bits &= b._bits;
        return *this;
    }

    /// @brief Bitwise-and operator.
    [[nodiscard]] auto operator&(const bitfield b) const noexcept -> bitfield {
        return bitfield{value_type(_bits & b._bits)};
    }

    /// @brief Bitwise-and operator.
    [[nodiscard]] auto operator&(const Bit b) const noexcept -> bitfield {
        return *this & bitfield{b};
    }

    /// @brief Bit inversion operator
    [[nodiscard]] auto operator~() const noexcept -> bitfield {
        return bitfield{value_type(~_bits)};
    }

    /// @brief Sets the specified bit.
    /// @see clear
    /// @see merge
    auto set(const bit_type b) noexcept -> bitfield& {
        _bits |= value_type(b); // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    /// @brief Merges the specified bitfield into this bitfield.
    /// @see set
    /// @see subtract
    auto merge(const bitfield b) noexcept -> bitfield& {
        _bits |= b._bits; // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    /// @brief Clears the specified bit.
    /// @see set
    /// @see merge
    auto clear(const bit_type b) noexcept -> bitfield& {
        _bits &= ~value_type(b); // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    /// @brief Subtracts the specified bitfield from this bitfield.
    /// @see clear
    /// @see merge
    auto subtract(const bitfield b) noexcept -> bitfield& {
        _bits &= ~b._bits; // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    /// @brief Clears all bits.
    /// @post is_empty()
    auto clear() noexcept -> bitfield& {
        _bits = value_type(0);
        return *this;
    }

private:
    value_type _bits{0U};
};

} // namespace eagine
