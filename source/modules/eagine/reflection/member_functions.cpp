/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:member_functions;

import std;
import eagine.core.types;
import eagine.core.memory;
import :decl_name;

namespace eagine {
//------------------------------------------------------------------------------
export template <std::size_t argCount>
struct member_function_names {
    string_view name;
    std::array<string_view, argCount> arg_names;

    constexpr member_function_names(
      const string_view n,
      const std::array<string_view, argCount> an) noexcept
      : name{n}
      , arg_names{an} {}
};
//------------------------------------------------------------------------------
export template <auto C>
struct member_function_wrapper
  : member_function_constant_t<C>
  , member_function_names<member_function_constant_t<C>::arity::value> {
    using names =
      member_function_names<member_function_constant_t<C>::arity::value>;
    using names::names;
};
//------------------------------------------------------------------------------
export template <auto... M>
constexpr auto make_member_function_mapping(
  member_function_wrapper<M>... m) noexcept {
    return std::make_tuple(m...);
}
//------------------------------------------------------------------------------
} // namespace eagine

