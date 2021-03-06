/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_TYPE_ENUM_HPP
#define EAGINE_LOGGING_TYPE_ENUM_HPP

#include "../../c_api/enum_class.hpp"
#include "../backend.hpp"
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
template <typename Self, typename T, identifier_t LibId, identifier_t Id>
static constexpr auto adapt_entry_arg(
  const identifier name,
  const enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept requires(std::is_signed_v<T>) {
    return [=](auto& backend) {
        backend.add_integer(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
template <typename Self, typename T, identifier_t LibId, identifier_t Id>
static constexpr auto adapt_entry_arg(
  const identifier name,
  const enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept requires(std::is_unsigned_v<T>) {
    return [=](auto& backend) {
        backend.add_unsigned(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif
