/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:byteset;

import eagine.core.types;
import :span;
import std;

namespace eagine {

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

    /// @brief Construiction from unsigned integer that is then split into bytes.
    template <typename UInt>
    explicit constexpr byteset(const UInt init) noexcept
        requires(
          (sizeof(UInt) >= N) and std::is_integral_v<UInt> and
          std::is_unsigned_v<UInt>)
      : byteset(std::make_index_sequence<N>(), init) {}

    /// @brief Returns a pointer to the byte sequence start.
    /// @see size
    constexpr auto data() noexcept -> pointer {
        return _bytes;
    }

    /// @brief Returns a const pointer to the byte sequence start.
    /// @see size
    constexpr auto data() const noexcept -> const_pointer {
        return _bytes;
    }

    /// @brief Returns the count of bytes in the stored sequence.
    constexpr auto size() const noexcept -> size_type {
        return N;
    }

    /// @brief Creates a const view over the stored sequence of bytes.
    constexpr auto block() const noexcept -> memory::const_block {
        return {data(), size()};
    }

    /// @brief Subscript operator.
    constexpr auto operator[](const size_type i) noexcept -> reference {
        return _bytes[i];
    }

    /// @brief Subscript operator.
    constexpr auto operator[](const size_type i) const noexcept
      -> const_reference {
        return _bytes[i];
    }

    /// @brief Returns the first byte in the sequence.
    /// @see back
    constexpr auto front() noexcept -> reference {
        return _bytes[0];
    }

    /// @brief Returns the first byte in the sequence.
    /// @see back
    constexpr auto front() const noexcept -> const_reference {
        return _bytes[0];
    }

    /// @brief Returns the last byte in the sequence.
    /// @see front
    constexpr auto back() noexcept -> reference {
        return _bytes[N - 1];
    }

    /// @brief Returns the last byte in the sequence.
    /// @see front
    constexpr auto back() const noexcept -> const_reference {
        return _bytes[N - 1];
    }

    /// @brief Returns an iterator to the start of the byte sequence.
    constexpr auto begin() noexcept -> iterator {
        return _bytes + 0;
    }

    /// @brief Returns an iterator past the end of the byte sequence.
    constexpr auto end() noexcept -> iterator {
        return _bytes + N;
    }

    /// @brief Returns a const iterator to the start of the byte sequence.
    constexpr auto begin() const noexcept -> const_iterator {
        return _bytes + 0;
    }

    /// @brief Returns a const iterator past the end of the byte sequence.
    constexpr auto end() const noexcept -> const_iterator {
        return _bytes + N;
    }

    /// @brief Comparison operator.
    constexpr auto operator<=>(const byteset& that) const noexcept
      -> std::strong_ordering {
        return _do_cmp(*this, that, std::make_index_sequence<N>{});
    }
    constexpr auto operator==(const byteset& that) const noexcept
      -> bool = default;
    constexpr auto operator<(const byteset& that) const noexcept
      -> bool = default;

    /// @brief Converts the byte sequence into an unsigned integer value.
    template <typename UInt>
    constexpr auto as(const UInt i = 0) const noexcept
        requires(
          (sizeof(UInt) >= N) and (
#if __SIZEOF_INT128__
                                    std::is_same_v<UInt, __uint128_t> or
                                    std::is_same_v<UInt, __int128_t> or
#endif
                                    std::is_integral_v<UInt>))
    {
        return _push_back_to(i, 0);
    }

private:
    value_type _bytes[N]{};

    template <typename UInt>
    constexpr auto _push_back_to(const UInt state, const std::size_t i)
      const noexcept -> UInt {
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        return (i < N) ? _push_back_to((state << 8U) | _bytes[i], i + 1)
                       : state;
    }

    static constexpr auto _cmp_byte(
      const value_type a,
      const value_type b) noexcept -> std::strong_ordering {
        return (a == b)  ? std::strong_ordering::equal
               : (a < b) ? std::strong_ordering::less
                         : std::strong_ordering::greater;
    }

    static constexpr auto _do_cmp(
      const byteset&,
      const byteset&,
      const std::index_sequence<>) noexcept -> std::strong_ordering {
        return std::strong_ordering::equal;
    }

    template <std::size_t I, std::size_t... In>
    static constexpr auto _do_cmp(
      const byteset& a,
      const byteset& b,
      const std::index_sequence<I, In...>) noexcept -> std::strong_ordering {
        return (a._bytes[I] == b._bytes[I])
                 ? _do_cmp(a, b, std::index_sequence<In...>{})
                 : _cmp_byte(a._bytes[I], b._bytes[I]);
    }
};

} // namespace eagine
