/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_TYPE_BUILD_INFO_HPP
#define EAGINE_LOGGING_TYPE_BUILD_INFO_HPP

#include "../../build_info.hpp"
#include "../entry_arg.hpp"

namespace eagine {
//------------------------------------------------------------------------------
static inline auto adapt_entry_arg(
  const identifier name,
  const build_info& value) noexcept {
    return [name, value](auto& backend) {
        if(const auto opt_maj{value.version_major()}) {
            backend.add_integer(name, EAGINE_ID(VerMajor), extract(opt_maj));
            if(const auto opt_min{value.version_minor()}) {
                backend.add_integer(
                  name, EAGINE_ID(VerMinor), extract(opt_min));
                if(const auto opt_ptch{value.version_patch()}) {
                    backend.add_integer(
                      name, EAGINE_ID(VerPatch), extract(opt_ptch));
                    if(const auto opt_cmit{value.version_commit()}) {
                        backend.add_integer(
                          name, EAGINE_ID(VerCommit), extract(opt_cmit));
                    }
                }
            }
        }
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_TYPE_BUILD_INFO_HPP
