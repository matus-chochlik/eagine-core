/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.identifier:message_id;

import eagine.core.types;
import eagine.core.vectorization;
import :decl;
import <tuple>;

namespace eagine {
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
    /// @post !is_valid()
    constexpr message_id() noexcept = default;

    ///	@brief Construction from two identifier values.
    constexpr message_id(
      const identifier_value c,
      const identifier_value m) noexcept
      : _data{c, m} {}

    ///	@brief Construction from two identifier objects.
    constexpr message_id(const identifier c, const identifier m) noexcept
      : message_id{c.value(), m.value()} {}

    ///	@brief Construction from two string literals.
    template <auto CL, auto ML>
        requires(identifier_literal_length<CL> && identifier_literal_length<ML>)
    constexpr message_id(const char (&c)[CL], const char (&m)[ML]) noexcept
      : message_id{identifier{c}, identifier{m}} {}

    ///	@brief Construction from a tuple of two identifier objects.
    constexpr message_id(const std::tuple<identifier, identifier> t) noexcept
      : message_id{std::get<0>(t), std::get<1>(t)} {}

    ///	@brief Construction from static_message_id value.
    template <identifier_value ClassId, identifier_value MethodId>
    constexpr message_id(const static_message_id<ClassId, MethodId>&) noexcept
      : message_id{ClassId, MethodId} {}

    constexpr auto operator<=>(const message_id&) const noexcept = default;
    /// @brief Equality comparison
    constexpr auto operator==(const message_id&) const noexcept
      -> bool = default;

    /// @brief Returns the class identifier value.
    constexpr auto class_id() const noexcept -> identifier_t {
        return _data[0];
    }

    /// @brief Returns the class identifier.
    constexpr auto class_() const noexcept -> identifier {
        return identifier{class_id()};
    }

    /// @brief Returns the method identifier value.
    constexpr auto method_id() const noexcept -> identifier_t {
        return _data[1];
    }

    /// @brief Returns the method identifier.
    constexpr auto method() const noexcept -> identifier {
        return identifier{method_id()};
    }

    /// @brief Checks if the stored identifier values are non-zero.
    constexpr auto is_valid() const noexcept {
        return (class_id() != 0U) && (method_id() != 0U);
    }

    /// @brief Returns the class and method identifiers in a tuple.
    /// See class_
    /// See method
    constexpr auto id_tuple() const noexcept
      -> std::tuple<identifier, identifier> {
        return {class_(), method()};
    }

    /// @brief Checks if the class identifier matches the argument.
    /// @see has_method
    constexpr auto has_class(const identifier_value idv) const noexcept
      -> bool {
        return class_id() == idv._value;
    }

    /// @brief Checks if the method identifier matches the argument.
    /// @see has_class
    constexpr auto has_method(const identifier_value idv) const noexcept
      -> bool {
        return method_id() == idv._value;
    }

private:
    vect::data_t<identifier_t, 2, false> _data{0U, 0U};
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
    static constexpr auto class_id() noexcept -> identifier_t {
        return ClassId;
    }

    /// @brief Returns the class identifier.
    static constexpr auto class_() noexcept -> identifier {
        return identifier{class_id()};
    }

    /// @brief Returns the method identifier value.
    static constexpr auto method_id() noexcept -> identifier_t {
        return MethodId;
    }

    /// @brief Returns the method identifier.
    static constexpr auto method() noexcept -> identifier {
        return identifier{method_id()};
    }
};
//------------------------------------------------------------------------------
/// @brief Equality comparison between message_id and static_message_id.
/// @ingroup identifiers
export template <identifier_value ClassId, identifier_value MethodId>
auto operator==(
  const message_id l,
  const static_message_id<ClassId, MethodId> r) noexcept {
    return l == message_id{r};
}
//------------------------------------------------------------------------------
} // namespace eagine
