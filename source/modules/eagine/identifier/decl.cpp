/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.identifier:decl;

import eagine.core.types;
import eagine.core.memory;
import <cstdint>;
import <iostream>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Helper class implementing identifier encoding functionality.
/// @ingroup identifiers
/// @see identifier
/// @note Do not use directly.
export template <typename CharSet>
class identifier_encoding {
public:
    /// @brief Encoded the specified character as N-bit byte.
    static constexpr auto encode(const char c) noexcept -> std::uint8_t {
        return _do_encode(c, 0, CharSet::values);
    }

    /// @brief Encoded the specified N-bit byte as character.
    static constexpr auto decode(const std::uint8_t i) noexcept -> char {
        return _do_decode(i, CharSet::values);
    }

    /// @brief Returns the invalid value from the encoding.
    static constexpr auto invalid() noexcept {
        return _get_invalid(CharSet::values);
    }

    /// @brief Indicates if the encoded value is invalid.
    static constexpr auto is_invalid(const std::uint8_t c) noexcept {
        return c >= invalid();
    }

    /// @brief Returns the valid character set as a string_view
    static auto chars() -> memory::string_view {
        return {CharSet::values};
    }

private:
    template <auto L>
    static constexpr auto _get_invalid(const char (&)[L]) noexcept {
        return std::uint8_t(L);
    }

    template <auto L>
    static constexpr auto _do_encode(
      const char c,
      const std::uint8_t i,
      const char (&enc)[L]) noexcept -> std::uint8_t {
        return ((i < L) && (c != '\0'))
                 ? ((c == enc[i]) ? i : _do_encode(c, i + 1, enc))
                 : invalid();
    }

    template <auto L>
    static constexpr auto _do_decode(
      const std::uint8_t i,
      const char (&enc)[L]) noexcept -> char {
        return (i < invalid()) ? enc[i] : '\0';
    }
};
//------------------------------------------------------------------------------
/// @brief Characted encoding table for identifier.
/// @ingroup identifiers
/// @see identifier
export struct default_identifier_char_set {
    /// @brief The default identifier character set.
    /// @showinitializer
    static constexpr const char values[63] = {
      '_', 'e', 't', 'a', 'o', 'i', 'n', 's', 'r', 'h', 'l', 'd', 'c',
      'u', 'm', 'f', 'p', 'g', 'w', 'y', 'b', 'v', 'k', 'x', 'j', 'q',
      'z', 'T', 'A', 'I', 'S', 'O', 'W', 'H', 'B', 'C', 'M', 'F', 'P',
      'D', 'R', 'L', 'E', 'G', 'N', 'Y', 'U', 'K', 'V', 'J', 'Q', 'X',
      'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
};
//------------------------------------------------------------------------------
/// @brief Helper template for unpacking of identifier into a character string.
/// @ingroup identifiers
/// @see indentifier
/// @note Do not use directly. Use identifier.
export template <auto M>
class identifier_name {
public:
    template <typename... C>
    constexpr identifier_name(span_size_t len, C... c) noexcept
      : _str{c...}
      , _len{std::uint8_t(len)} {}

    /// @brief Alias for the string lenght type.
    using size_type = span_size_t;

    /// @brief Alias for the string character type.
    using value_type = char;

    /// @brief Alias for the iterator type.
    using iterator = const char*;

    /// @brief Alias for the const iterator type.
    using const_iterator = iterator;

    /// @brief Returns the pointer to the start of the unpacked identifier name.
    constexpr auto data() const noexcept {
        return _str.data();
    }

    /// @brief Returns the length of the unpacked character string.
    constexpr auto size() const noexcept {
        return size_type(_len);
    }

    /// @brief Returns an iterator to the start of the unpacked identifier name.
    constexpr auto begin() const noexcept -> const_iterator {
        return _str.data();
    }

    /// @brief Returns an iterator past the end of the unpacked identifier name.
    constexpr auto end() const noexcept -> const_iterator {
        return _str.data() + size();
    }

    /// @brief Returns a string view covering the unpacked identifier name.
    constexpr auto view() const noexcept -> memory::string_view {
        // negative size indicates that the data is zero terminated
        return {data(), -size()};
    }

    /// @brief Returns the unpacked identifier name as a standard string.
    auto str() const noexcept -> std::string {
        return {_str.data(), _len};
    }

    /// @brief Assigns the unpacked identifier name into a standard string.
    /// @returns s
    auto str(std::string& s) const noexcept -> std::string& {
        s.assign(_str.data(), _len);
        return s;
    }

private:
    memory::fixed_size_string<M> _str{};
    std::uint8_t _len{0};
};
//------------------------------------------------------------------------------
/// @brief Operator for writing identifier_name into output streams.
/// @ingroup identifiers
/// @see identifier_name
export template <std::size_t M>
auto operator<<(std::ostream& out, const identifier_name<M>& n)
  -> std::ostream& {
    return out.write(n.data(), std::streamsize(n.size()));
}
//------------------------------------------------------------------------------
/// @brief Basic template for limited length, packed string identifiers.
/// @ingroup identifiers
/// @see basic_identifier_value
/// @see identifier
/// @see long_identifier
///
/// Packed identifier store short constant strings with limited allowed set
/// of characters that are used as object identifiers throughout the project.
/// The strings are @c constexpr encoded into unsigned integer values and are
/// stored without dynamic memory allocation.
/// Comparison operations on identifiers are typically more efficient than
/// regular character string comparisons, but note that integer comparisons
/// are used instead lexicographical comparisons.
export template <std::size_t M, std::size_t B, typename CharSet, typename UIntT>
class basic_identifier {
    using _bites_t = biteset<M, B, std::uint8_t>;

public:
    static_assert(
      (1U << B) > sizeof(CharSet::values),
      "B-bits are not sufficient to represent CharSet");

    /// @brief Alias for the encoding type.
    using encoding = identifier_encoding<CharSet>;

    /// @brief Alias for the length type.
    using size_type = span_size_t;

    /// @brief Alias for the element type.
    using value_type = char;

    /// @brief Alias for the unpacked identifier_name type.
    using name_type = identifier_name<M>;

    /// @brief The maximum length of the identifier string.
    static constexpr const std::size_t max_length = M;

    /// @brief Default constructor. Constructs an empty identifier.
    constexpr basic_identifier() noexcept = default;

    /// @brief Construction from a C-string literal.
    template <auto L>
    constexpr basic_identifier(const char (&init)[L]) noexcept
        requires(L <= M + 1)
      : _bites{_make_bites(
          static_cast<const char*>(init),
          L,
          std::make_index_sequence<M>{})} {}

    /// @brief Construction from a const span of characters.
    explicit constexpr basic_identifier(
      const memory::span<const char> init) noexcept
      : _bites{_make_bites(
          init.data(),
          init.size(),
          std::make_index_sequence<M>{})} {}

    explicit constexpr basic_identifier(const UIntT init) noexcept
      : _bites{_bites_t::from_value(init)} {}

    explicit constexpr basic_identifier(const _bites_t init) noexcept
      : _bites{std::move(init)} {}

    /// @brief Returns the maximum length of this identifier type.
    static constexpr auto max_size() noexcept -> size_type {
        return M;
    }

    /// @brief Returns the size of this identifier.
    /// @see size()
    constexpr auto is_empty() const noexcept -> bool {
        return value() == 0;
    }

    constexpr explicit operator bool() const noexcept {
        return !is_empty();
    }

    /// @brief Returns the size of this identifier.
    /// @see is_empty()
    constexpr auto size() const noexcept -> size_type {
        return is_empty() ? 0 : _get_size(0);
    }

    /// @brief Subscript operator. Allows to access individual characters.
    constexpr auto operator[](const size_type idx) const noexcept
      -> value_type {
        return encoding::decode(_bites[idx]);
    }

    constexpr auto value() const noexcept -> UIntT {
        return _bites.bytes().template as<UIntT>();
    }

    constexpr auto matches(const UIntT what) const noexcept {
        return value() == what;
    }

    constexpr auto matches(memory::string_view what) const noexcept {
        // TODO: this could be optimized: compare char-by-char and bail early
        return name() == what;
    }

    /// @brief Returns this identifier as unpacked identifier_name.
    /// @see identifier_name
    constexpr auto name() const noexcept -> name_type {
        return _get_name(std::make_index_sequence<M>{});
    }

    /// @brief Returns this identifier as unpacked standard string.
    /// @see identifier_name
    auto str() const -> std::string {
        return name().str();
    }

    /// @brief Comparison.
    constexpr auto operator<=>(const basic_identifier&) const noexcept = default;

private:
    _bites_t _bites;

    template <std::size_t... I>
    static constexpr auto _make_bites(
      const char* init,
      std::size_t l,
      std::index_sequence<I...>) noexcept {
        return biteset<M, B, std::uint8_t>{
          encoding::encode((I < l) ? init[I] : '\0')...};
    }

    template <std::size_t... I>
    constexpr auto _get_name(std::index_sequence<I...>) const noexcept
      -> name_type {
        return name_type{size(), (*this)[size_type(I)]...};
    }

    constexpr auto _get_size(std::size_t s) const noexcept -> std::size_t {
        return (s < M) ? encoding::is_invalid(_bites[size_type(s)])
                           ? s
                           : _get_size(s + 1)
                       : M;
    }
};
//------------------------------------------------------------------------------
/// @brief The numeric value of an identifier. Can be used as a NTTP.
/// @ingroup identifiers
/// @see basic_identifier
/// @see identifier_value
export template <std::size_t M, std::size_t B, typename CharSet, typename UIntT>
struct basic_identifier_value {
    UIntT _value{};

    /// @brief The numeric type of the identifier value.
    using value_type = UIntT;

    /// @brief The type of the identifier instantiation.
    using identifier_type = basic_identifier<M, B, CharSet, UIntT>;

    /// @brief Construction from the value type.
    constexpr basic_identifier_value(const value_type value) noexcept
      : _value{value} {}

    /// @brief Construction from a string literal.
    template <auto L>
    constexpr basic_identifier_value(const char (&str)[L]) noexcept
        requires(L <= M + 1)
      : _value{identifier_type{str}.value()} {}

    /// @brief Conversion to numeric value type.
    constexpr operator value_type() const noexcept {
        return _value;
    }

    /// @brief Conversion to identifier type.
    constexpr operator identifier_type() const noexcept {
        return identifier_type{_value};
    }
};
//------------------------------------------------------------------------------
/// @brief Default short string identifier type used throughout the project.
/// @ingroup identifiers
/// @see basic_identifier
/// @see default_identifier_char_set
/// @see identifier_t
/// @see dec_to_identifier
/// @see byte_to_identifier
/// @see random_identifier
///
/// Allows to store short constant string identifiers with maximum length of 10 characters.
export using identifier =
  basic_identifier<10, 6, default_identifier_char_set, identifier_t>;

/// @brief Alias for identifier.
/// @ingroup identifiers
/// @see id_v
export using id = identifier;

/// @brief Default instantiation of identifier value.
/// @ingroup identifiers
/// @see identifier
export using identifier_value =
  basic_identifier_value<10, 6, default_identifier_char_set, identifier_t>;

export template <auto L>
concept identifier_literal_length = (L <= identifier::max_length + 1U);

/// @brief Returns the numeric value of the specified identifier string.
/// @ingroup identifiers
/// @see id
export template <std::size_t N>
consteval auto id_v(const char (&str)[N]) noexcept -> identifier_t {
    return identifier{str}.value();
}
//------------------------------------------------------------------------------
} // namespace eagine
