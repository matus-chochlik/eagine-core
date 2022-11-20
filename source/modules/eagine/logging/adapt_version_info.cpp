/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_version_info;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.build_info;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
struct version_info_entry_adapter {
    const identifier name;
    const version_info& value;

    void operator()(auto& backend) const noexcept {
        if(const auto opt_maj{value.version_major()}) {
            backend.add_integer(name, "VerMajor", extract(opt_maj));
            if(const auto opt_min{value.version_minor()}) {
                backend.add_integer(name, "VerMinor", extract(opt_min));
                if(const auto opt_ptch{value.version_patch()}) {
                    backend.add_integer(name, "VerPatch", extract(opt_ptch));
                    if(const auto opt_cmit{value.version_commit()}) {
                        backend.add_integer(
                          name, "VerCommit", extract(opt_cmit));
                    }
                }
            }
        }
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const version_info& value) noexcept {
    return version_info_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

