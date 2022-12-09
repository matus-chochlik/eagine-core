/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_exception;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import <stdexcept>;
import <system_error>;

namespace eagine {
//------------------------------------------------------------------------------
struct std_exception_entry_adapter {
    const identifier name;
    const std::exception& value;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "Exception", string_view(value.what()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::exception& value) noexcept {
    return std_exception_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct std_runtime_error_entry_adapter {
    const identifier name;
    const std::runtime_error& value;
    void operator()(auto& backend) noexcept {
        backend.add_string(name, "RuntmError", string_view(value.what()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::runtime_error& value) noexcept {
    return std_runtime_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct std_system_error_entry_adapter {
    const identifier name;
    const std::system_error& value;

    void operator()(auto& backend) noexcept {
        backend.add_string(name, "SystmError", string_view(value.what()));
        backend.add_string(
          "category",
          "ErrorCtgry",
          string_view(value.code().category().name()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::system_error& value) noexcept {
    return std_system_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

