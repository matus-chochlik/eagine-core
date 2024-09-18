/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:byteset;

import std;
import eagine.core.types;
import :span;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class storing a sequence of bytes converting them to and from unsigned integer.
/// @ingroup type_utils
/// @tparam N the size - number of elements - in the byte sequence.
/// @see biteset
export template <std::size_t N>
class byteset {
public:
    static_assert(N > 0, "byteset size must be greater than zero");

    /// @brief Alias for size type.
    using size_type = span_size_t;

    /// @brief Alias for element value type.
    using value_type = byte;

    /// @brief Alias for element reference type.
    using reference = value_type&;

    /// @brief Alias for const reference type.
    using const_reference = const value_type&;

    /// @brief Alias for pointer to element type.
    using pointer = value_type*;

    /// @brief Alias for pointer to const element type.
    using const_pointer = const value_type*;

    /// @brief Alias for iterator type.
    using iterator = value_type*;

    /// @brief Alias for const iterator type.
    using const_iterator = const value_type*;

    /// @brief Default constructor.
    constexpr byteset() noexcept = default;

    /// @brief Construction from a pack of integer values.
    template <typename... B>
    explicit constexpr byteset(const B... b) noexcept
        requires(
          (sizeof...(B) == N) and (sizeof...(B) != 0) and
          std::conjunction_v<std::is_convertible<B, value_type>...>)
      : _bytes{value_type{b}...} {}

    template <std::size_t... I, typename UInt>
    constexpr byteset(const std::index_sequence<I...>, const UInt init) noexcept
        requires((sizeof(UInt) >= N) and std::is_integral_v<UInt>)
      : _bytes{value_type((init >> (8 * (N - I - 1))) & 0xFFU)...} {}

    /// @brief Construction from unsigned integer that is then split into bytes.
    template <typename UInt>
    explicit constexpr byteset(const UInt init) noexcept
        requires(
          (sizeof(UInt) >= N) and std::is_integral_v<UInt> and
          std::is_unsigned_v<UInt>)
      : byteset(std::make_index_sequence<N>(), init) {}

    /// @brief Returns the count of bytes in the stored sequence.
    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return N;
    }

    /// @brief Returns a pointer to the byte sequence start.
    /// @see size
    [[nodiscard]] constexpr auto data() noexcept -> pointer {
        return reinterpret_cast<pointer>(&_bytes);
    }

    /// @brief Returns a const pointer to the byte sequence start.
    /// @see size
    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
        return reinterpret_cast<const_pointer>(&_bytes);
    }

    /// @brief Creates a const view over the stored sequence of bytes.
    [[nodiscard]] constexpr auto block() const noexcept -> memory::const_block {
        return {data(), size()};
    }

    /// @brief Subscript operator.
    [[nodiscard]] constexpr auto operator[](const size_type i) noexcept
      -> reference {
        return _bytes[i];
    }

    /// @brief Subscript operator.
    [[nodiscard]] constexpr auto operator[](const size_type i) const noexcept
      -> value_type {
        return _bytes[i];
    }

    /// @brief Returns an iterator to the start of the byte sequence.
    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return data();
    }

    /// @brief Returns an iterator past the end of the byte sequence.
    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return data() + N;
    }

    /// @brief Returns a const iterator to the start of the byte sequence.
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return data();
    }

    /// @brief Returns a const iterator past the end of the byte sequence.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return data() + N;
    }

    /// @brief Comparison operator.
    constexpr auto operator<=>(const byteset& that) const noexcept
      -> std::strong_ordering = default;
    constexpr auto operator==(const byteset& that) const noexcept
      -> bool = default;
    constexpr auto operator<(const byteset& that) const noexcept
      -> bool = default;

    /// @brief Converts the byte sequence into an unsigned integer value.
    template <typename UInt>
    [[nodiscard]] constexpr auto as(const UInt i = 0) const noexcept
        requires(
          std::is_unsigned_v<UInt> and (sizeof(UInt) >= N) and
          (
#if __SIZEOF_INT128__
            std::is_same_v<UInt, __uint128_t> or
#endif
            std::is_integral_v<UInt>))
    {
        return _as<UInt>(std::make_index_sequence<N>());
    }

private:
    simd::data_t<value_type, N, false> _bytes{};

    template <typename UInt, std::size_t... I>
    constexpr auto _as(std::index_sequence<I...>) const noexcept -> UInt {
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        return (... | (UInt(_bytes[I]) << UInt(8Z * (N - I - 1Z))));
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
