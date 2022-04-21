/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_CONSTANT_HPP
#define EAGINE_C_API_CONSTANT_HPP

#include "../string_span.hpp"
#include "enum_class.hpp"
#include <concepts>

namespace eagine::c_api {
//------------------------------------------------------------------------------
/// @brief Common base class for wrapping C-API constant values.
/// @ingroup c_api_wrap
/// @see no_c_api_constant
/// @see static_c_api_constant
/// @see dynamic_c_api_constant
struct constant_base {
public:
    constexpr constant_base(const string_view name) noexcept
      : _name{name} {}

    /// @brief Returns the name of the constant as a string.
    constexpr auto name() const noexcept {
        return _name;
    }

private:
    string_view _name{};
};
//------------------------------------------------------------------------------
/// @brief Class wrapping a constant with missing or unknown value.
/// @ingroup c_api_wrap
template <typename T, typename Tag = nothing_t, bool isIndexed = false>
struct no_constant
  : constant_base
  , no_enum_value<T, Tag> {
public:
    template <typename ApiTraits, typename Api>
    constexpr no_constant(const string_view name, ApiTraits&, Api&) noexcept
      : constant_base{name} {}

    /// @brief Adds the specified value to the constant (if it isIndexed).
    template <std::integral I>
    constexpr auto operator+(const I) const noexcept
      -> no_enum_value<T, Tag> requires(isIndexed) {
        return {};
    }
};
//------------------------------------------------------------------------------
/// @brief Class wrapping a constant with value known at compile-time.
/// @ingroup c_api_wrap
template <
  typename ClassList,
  typename T,
  T value,
  typename Tag = nothing_t,
  bool isIndexed = false>
struct static_constant
  : constant_base
  , enum_value<T, ClassList, Tag> {
public:
    template <typename ApiTraits, typename Api>
    constexpr static_constant(string_view name, ApiTraits&, Api&) noexcept
      : constant_base{name}
      , enum_value<T, ClassList, Tag>{value} {}

    /// @brief Adds the specified value to the constant (it it isIndexed).
    template <typename I>
    constexpr auto operator+(const I index) const noexcept
      -> enum_value<T, ClassList, Tag> requires(
        isIndexed&& std::is_integral_v<I>) {
        using O = std::conditional_t<
          std::is_signed_v<T>,
          std::make_signed_t<I>,
          std::make_unsigned_t<I>>;
        using eagine::limit_cast;
        return {limit_cast<T>(value + limit_cast<O>(index))};
    }
};
//------------------------------------------------------------------------------
/// @brief Class wrapping a constant with value loaded at run-time.
/// @ingroup c_api_wrap
template <
  typename ClassList,
  typename T,
  typename Tag = nothing_t,
  bool isIndexed = false>
struct dynamic_constant
  : constant_base
  , opt_enum_value<T, ClassList, Tag> {
public:
    using tag_type = Tag;

    template <typename ApiTraits, typename Api>
    constexpr dynamic_constant(
      const string_view name,
      ApiTraits& traits,
      Api& api) noexcept
      : constant_base{name}
      , opt_enum_value<T, ClassList, Tag>{
          traits.load_constant(api, name, type_identity<T>())} {}

    /// @brief Adds the specified value to the constant (it it isIndexed).
    template <typename I>
    constexpr auto operator+(const I index) const noexcept
      -> opt_enum_value<T, ClassList, Tag> requires(
        isIndexed&& std::is_integral_v<I>) {
        using O = std::conditional_t<
          std::is_signed_v<T>,
          std::make_signed_t<I>,
          std::make_unsigned_t<I>>;
        return {limit_cast<T>(this->value + limit_cast<O>(index))};
    }
};
//------------------------------------------------------------------------------
template <typename ClassList, typename Constant, typename Tag, bool isIndexed>
struct get_opt_constant;

template <typename ClassList, typename T, T value, typename Tag, bool isIndexed>
struct get_opt_constant<ClassList, std::integral_constant<T, value>, Tag, isIndexed>
  : type_identity<static_constant<ClassList, T, value, Tag, isIndexed>> {};

template <typename ClassList, typename T, typename Tag, bool isIndexed>
struct get_opt_constant<ClassList, type_identity<T>, Tag, isIndexed>
  : type_identity<dynamic_constant<ClassList, T, Tag, isIndexed>> {};

/// @brief Template alias used for switching between static and dynamic constants.
/// @tparam ClassList a list of enum_class types into which the constant can
///         be converted.
/// @tparam Constant integral constant specifying the value or a placeholder.
/// @tparam Tag a tag type that can be used in custiomization of some operations
///         on the constant.
/// @tparam isIndexed indicates if the constant is indexed, which enables
///         additional operations on the constants.
/// @ingroup c_api_wrap
///
/// @see no_constant
/// @see static_constant
/// @see dynamic_constant
/// @see enum_class
/// @see enum_value
/// @see mp_list
///
/// C-API constants can be either known at compile time or loaded dynamically
/// by using some API function(s).
template <
  typename ClassList,
  typename Constant,
  typename Tag = nothing_t,
  bool isIndexed = false>
using opt_constant =
  typename get_opt_constant<ClassList, Constant, Tag, isIndexed>::type;
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif
