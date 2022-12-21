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

    /// @brief Checks if the stored identifier values are non-zero.
    [[nodiscard]] constexpr auto is_valid() const noexcept {
        return (class_id() != 0U) and (method_id() != 0U);
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

private:
#if __SIZEOF_INT128__
    __uint128_t _data{0U};
#else
    vect::data_t<identifier_t, 2, false> _data{0U, 0U};
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
} // namespace eagine
