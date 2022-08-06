/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_tribool;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
export struct yes_no_maybe : tribool {
    constexpr yes_no_maybe(tribool v) noexcept
      : tribool{v} {}
};
//------------------------------------------------------------------------------
export auto adapt_entry_arg(const identifier name, yes_no_maybe value) noexcept {
    struct _adapter {
        const identifier name;
        const yes_no_maybe value;

        void operator()(logger_backend& backend) const noexcept {
            backend.add_string(
              name,
              "YesNoMaybe",
              value.is(true)    ? string_view{"yes"}
              : value.is(false) ? string_view{"no"}
                                : string_view{"maybe"});
        }
    };
    return _adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

