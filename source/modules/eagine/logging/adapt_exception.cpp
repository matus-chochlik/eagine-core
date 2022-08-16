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
import :backend;
import <stdexcept>;
import <system_error>;

namespace eagine {
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const std::exception& value) noexcept {
    struct _adapter {
        const identifier name;
        const std::exception& value;
        void operator()(logger_backend& backend) const noexcept {
            backend.add_string(name, "Exception", string_view(value.what()));
        }
    };
    return _adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const std::runtime_error& value) noexcept {
    struct _adapter {
        const identifier name;
        const std::runtime_error& value;
        void operator()(logger_backend& backend) noexcept {
            backend.add_string(name, "RuntmError", string_view(value.what()));
        }
    };
    return _adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const std::system_error& value) noexcept {
    struct _adapter {
        const identifier name;
        const std::system_error& value;

        void operator()(logger_backend& backend) noexcept {
            backend.add_string(name, "SystmError", string_view(value.what()));
            backend.add_string(
              "category",
              "ErrorCtgry",
              string_view(value.code().category().name()));
        }
    };
    return _adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

