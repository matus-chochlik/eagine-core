/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.identifier;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.simd;

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
    static constexpr auto chars() noexcept -> memory::string_view {
        return {CharSet::values};
    }

    /// @brief Indicates if the specified string view can be encoded by this.
    static constexpr auto can_be_encoded(string_view str) noexcept -> bool {
        for(const char c : str) {
            if(not memory::has_element(chars(), c)) {
                return false;
            }
        }
        return true;
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
        return ((i < L) and (c != '\0'))
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
      : _str{c..., '\n'}
      , _len{std::uint8_t(len)} {}

    /// @brief Alias for the string length type.
    using size_type = span_size_t;

    /// @brief Alias for the string character type.
    using value_type = char;

    /// @brief Alias for the iterator type.
    using iterator = const char*;

    /// @brief Alias for the const iterator type.
    using const_iterator = iterator;

    /// @brief Returns the pointer to the start of the unpacked identifier name.
    [[nodiscard]] constexpr auto data() const noexcept {
        return _str.data();
    }

    /// @brief Returns the length of the unpacked character string.
    [[nodiscard]] constexpr auto size() const noexcept {
        return size_type(_len);
    }

    /// @brief Returns an iterator to the start of the unpacked identifier name.
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return _str.data();
    }

    /// @brief Returns an iterator past the end of the unpacked identifier name.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return _str.data() + size();
    }

    /// @brief Returns a string view covering the unpacked identifier name.
    [[nodiscard]] constexpr auto view() const noexcept -> memory::string_view {
        // negative size indicates that the data is zero terminated
        return {data(), -size()};
    }

    /// @brief Returns the unpacked identifier name as a standard string.
    [[nodiscard]] auto str() const noexcept -> std::string {
        return {_str.data(), _len};
    }

    /// @brief Assigns the unpacked identifier name into a standard string.
    /// @returns s
    auto str(std::string& s) const noexcept -> std::string& {
        s.assign(_str.data(), _len);
        return s;
    }

private:
    memory::fixed_size_string<M + 1> _str{};
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
export template <
  std::size_t M,
  std::size_t B,
  typename CharSet,
  typename UIntT,
  bool V = true>
class basic_identifier {
    using _bites_t = biteset<M, B, std::uint8_t, V>;

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

    /// @brief Indicates if the specified string view can be encoded into identifier.
    [[nodiscard]] static constexpr auto can_be_encoded(string_view str) noexcept
      -> bool {
        if(str.size() <= max_length) {
            return encoding::can_be_encoded(str);
        }
        return false;
    }

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
    [[nodiscard]] static constexpr auto max_size() noexcept -> size_type {
        return M;
    }

    /// @brief Indicates if this identifier is empty.
    /// @see size
    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return value() == 0;
    }

    /// @brief Indicates if this identifier is not empty.
    /// @see is_empty
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return value() != 0;
    }

    /// @brief Returns the size of this identifier.
    /// @see is_empty
    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return is_empty() ? 0 : _get_size(0);
    }

    /// @brief Subscript operator. Allows to access individual characters.
    [[nodiscard]] constexpr auto operator[](const size_type idx) const noexcept
      -> value_type {
        return encoding::decode(_bites[idx]);
    }

    [[nodiscard]] constexpr auto value() const noexcept -> UIntT {
        return _bites.bytes().template as<UIntT>();
    }

    [[nodiscard]] constexpr auto matches(const UIntT what) const noexcept {
        return value() == what;
    }

    [[nodiscard]] constexpr auto matches(
      memory::string_view what) const noexcept {
        return name() == what;
    }

    /// @brief Returns this identifier as unpacked identifier_name.
    /// @see identifier_name
    [[nodiscard]] constexpr auto name() const noexcept -> name_type {
        return _get_name(std::make_index_sequence<M>{});
    }

    /// @brief Returns this identifier as unpacked standard string.
    /// @see identifier_name
    [[nodiscard]] auto str() const -> std::string {
        return name().str();
    }

    [[nodiscard]] auto bites() const noexcept -> _bites_t {
        return _bites;
    }

    /// @brief Returns a new identifier by incrementing the argument by one.
    [[nodiscard]] friend auto increment(basic_identifier i) noexcept
      -> basic_identifier {
        const auto _inc_bites{[](_bites_t v) {
            using T = std::uint8_t;
            using D = std::uintmax_t;
            const D dmax{1U << B};
            span_size_t p{v.size()};
            while(p > 0) {
                --p;
                const D next{D(v.get(p)) + 1U};
                if(next < dmax) {
                    v.set(p, T(next));
                    return v;
                }
                v.set(p, T(0));
            }
            return _bites_t{};
        }};
        return basic_identifier{_inc_bites(i._bites)};
    }

    /// @brief Comparison.
    [[nodiscard]] constexpr auto operator<=>(
      const basic_identifier&) const noexcept = default;

private:
    _bites_t _bites;

    template <std::size_t... I>
    static constexpr auto _make_bites(
      const char* init,
      std::size_t l,
      std::index_sequence<I...>) noexcept {
        return _bites_t{encoding::encode((I < l) ? init[I] : '\0')...};
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
export template <
  std::size_t M,
  std::size_t B,
  typename CharSet,
  typename UIntT,
  bool V = false>
struct basic_identifier_value {
    UIntT _value{};

    /// @brief The numeric type of the identifier value.
    using value_type = UIntT;

    /// @brief The type of the identifier instantiation.
    using identifier_type = basic_identifier<M, B, CharSet, UIntT, V>;

    /// @brief Alias for the unpacked identifier_name type.
    using name_type = identifier_name<M>;

    /// @brief Default constructor.
    constexpr basic_identifier_value() noexcept
      : _value{identifier_type{}.value()} {}

    /// @brief Construction from the value type.
    constexpr basic_identifier_value(const value_type value) noexcept
      : _value{value} {}

    /// @brief Construction from the identifier type.
    template <bool W>
    constexpr basic_identifier_value(
      const basic_identifier<M, B, CharSet, UIntT, W> id) noexcept
      : _value{id.value()} {}

    /// @brief Construction from a string literal.
    template <auto L>
    constexpr basic_identifier_value(const char (&str)[L]) noexcept
        requires(L <= M + 1)
      : _value{identifier_type{str}.value()} {}

    /// @brief Conversion to numeric value type.
    [[nodiscard]] constexpr operator value_type() const noexcept {
        return _value;
    }

    /// @brief Conversion to identifier type.
    [[nodiscard]] constexpr operator identifier_type() const noexcept {
        return identifier_type{_value};
    }

    /// @brief Conversion to identifier type.
    [[nodiscard]] constexpr auto identifier() const noexcept {
        return identifier_type{_value};
    }

    /// @brief Conversion to identifier name.
    [[nodiscard]] constexpr auto name() const noexcept {
        return identifier_type{_value}.name();
    }

    /// @brief Returns a new identifier_value by incrementing the argument by one.
    [[nodiscard]] friend auto increment(basic_identifier_value i) noexcept
      -> basic_identifier_value {
        return increment(identifier_type{i._value}).value();
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
[[nodiscard]] consteval auto id_v(const char (&str)[N]) noexcept
  -> identifier_t {
    using I =
      basic_identifier<10, 6, default_identifier_char_set, identifier_t, false>;
    return I{str}.value();
}
//------------------------------------------------------------------------------
// message_id
//------------------------------------------------------------------------------
export template <identifier_value ClassId, identifier_value MethodId>
struct static_message_id;
//------------------------------------------------------------------------------
/// @brief Class storing two identifier values representing class/method pair.
/// @ingroup identifiers
/// @see static_message_id
/// @see identifier_t
/// @see identifier
export class message_id {
public:
    ///	@brief Default constructor.
    /// @post not is_valid()
    constexpr message_id() noexcept = default;

    ///	@brief Construction from two identifier values.
    constexpr message_id(
      const identifier_value c,
      const identifier_value m) noexcept
#if __SIZEOF_INT128__
      : _data{(static_cast<__uint128_t>(c) << 64U) | static_cast<__uint128_t>(m)}
#else
      : _data{c, m}
#endif
    {
    }

    ///	@brief Construction from a tuple of two identifier objects.
    constexpr message_id(const std::tuple<identifier, identifier> t) noexcept
      : message_id{std::get<0>(t), std::get<1>(t)} {}

    ///	@brief Construction from static_message_id value.
    template <identifier_value ClassId, identifier_value MethodId>
    constexpr message_id(const static_message_id<ClassId, MethodId>&) noexcept
      : message_id{ClassId, MethodId} {}

    [[nodiscard]] constexpr auto operator<=>(const message_id&) const noexcept =
      default;
    /// @brief Equality comparison
    [[nodiscard]] constexpr auto operator==(const message_id&) const noexcept
      -> bool = default;

    /// @brief Returns the class identifier value.
    [[nodiscard]] constexpr auto class_id() const noexcept -> identifier_t {
#if __SIZEOF_INT128__
        return static_cast<identifier_t>(_data >> 64U);
#else
        return _data[0];
#endif
    }

    /// @brief Returns the class identifier.
    [[nodiscard]] constexpr auto class_() const noexcept -> identifier {
        return identifier{class_id()};
    }

    /// @brief Returns the method identifier value.
    [[nodiscard]] constexpr auto method_id() const noexcept -> identifier_t {
#if __SIZEOF_INT128__
        return static_cast<identifier_t>(_data);
#else
        return _data[1];
#endif
    }

    /// @brief Returns the method identifier.
    [[nodiscard]] constexpr auto method() const noexcept -> identifier {
        return identifier{method_id()};
    }

    /// @brief Checks if the class and method identifier values are non-zero.
    [[nodiscard]] constexpr auto is_valid() const noexcept {
        return (class_id() != 0U) and (method_id() != 0U);
    }

    /// @brief Checks if the class and method identifier values are non-zero.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Returns the class and method identifiers in a tuple.
    /// See class_
    /// See method
    [[nodiscard]] constexpr auto id_tuple() const noexcept
      -> std::tuple<identifier, identifier> {
        return {class_(), method()};
    }

    /// @brief Checks if the class identifier matches the argument.
    /// @see has_method
    [[nodiscard]] constexpr auto has_class(
      const identifier_value idv) const noexcept -> bool {
        return class_id() == idv._value;
    }

    /// @brief Checks if the method identifier matches the argument.
    /// @see has_class
    [[nodiscard]] constexpr auto has_method(
      const identifier_value idv) const noexcept -> bool {
        return method_id() == idv._value;
    }

    /// @brief Checks if the method identifier matches the argument.
    /// @see has_class
    /// @see has_method
    [[nodiscard]] constexpr auto is(
      const identifier_value idc,
      const identifier_value idm) const noexcept -> bool {
        return has_class(idc) and has_method(idm);
    }

private:
#if __SIZEOF_INT128__
    __uint128_t _data{0U};
#else
    simd::data_t<identifier_t, 2, false> _data{0U, 0U};
#endif
};
//------------------------------------------------------------------------------
/// @brief Template with two identifier parameters representing class/method pair.
/// @ingroup identifiers
/// @see message_id
///
/// This class encodes an identifier pair in its template parameters.
/// It is implicitly convertible to message_id.
export template <identifier_value ClassId, identifier_value MethodId>
struct static_message_id {
    using type = static_message_id;

    /// @brief Returns the class identifier value.
    [[nodiscard]] static constexpr auto class_id() noexcept -> identifier_t {
        return ClassId;
    }

    /// @brief Returns the class identifier.
    [[nodiscard]] static constexpr auto class_() noexcept -> identifier {
        return identifier{class_id()};
    }

    /// @brief Returns the method identifier value.
    [[nodiscard]] static constexpr auto method_id() noexcept -> identifier_t {
        return MethodId;
    }

    /// @brief Returns the method identifier.
    [[nodiscard]] static constexpr auto method() noexcept -> identifier {
        return identifier{method_id()};
    }
};
//------------------------------------------------------------------------------
/// @brief Equality comparison between message_id and static_message_id.
/// @ingroup identifiers
export template <identifier_value ClassId, identifier_value MethodId>
[[nodiscard]] auto operator==(
  const message_id l,
  const static_message_id<ClassId, MethodId> r) noexcept {
    return l == message_id{r};
}
//------------------------------------------------------------------------------
// construction functions
//------------------------------------------------------------------------------
/// @brief Creates an identifier representing a byte value as a hex string.
/// @ingroup identifiers
/// @see identifier
/// @see dec_to_identifier
/// @see random_identifier
export [[nodiscard]] auto byte_to_identifier(const byte b) noexcept
  -> identifier;

/// @brief Creates an identifier representing an integer value as a decimal string.
/// @ingroup identifiers
/// @see identifier
/// @see byte_to_identifier
/// @see random_identifier
export [[nodiscard]] auto dec_to_identifier(const int i) noexcept -> identifier;
//------------------------------------------------------------------------------
export [[nodiscard]] auto random_identifier(std::default_random_engine&)
  -> identifier;

export [[nodiscard]] auto random_identifier(
  const string_view charset,
  std::default_random_engine&) -> identifier;
//------------------------------------------------------------------------------
/// @brief Creates an random identifier using a default random engine.
/// @ingroup identifiers
/// @see identifier
/// @see byte_to_identifier
/// @see dec_to_identifier
export [[nodiscard]] auto random_identifier() -> identifier;

export [[nodiscard]] auto random_identifier(const string_view charset)
  -> identifier;
//------------------------------------------------------------------------------
} // namespace eagine

