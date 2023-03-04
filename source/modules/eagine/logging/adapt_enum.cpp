/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_enum;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.c_api;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Self, typename T, identifier_t LibId, identifier_t Id>
constexpr auto adapt_entry_arg(
  const identifier name,
  const c_api::enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept
    requires(std::is_signed_v<T>)
{
    return [=](auto& backend) {
        backend.add_integer(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
export template <typename Self, typename T, identifier_t LibId, identifier_t Id>
constexpr auto adapt_entry_arg(
  const identifier name,
  const c_api::enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept
    requires(std::is_unsigned_v<T>)
{
    return [=](auto& backend) {
        backend.add_unsigned(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

