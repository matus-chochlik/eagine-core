/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MAIN_FWD_HPP
#define EAGINE_MAIN_FWD_HPP

#include "main_ctx_fwd.hpp"

namespace eagine {

struct main_ctx_options;

auto main_impl(int, const char**, main_ctx_options&, int (*main_impl)(main_ctx&))
  -> int;

} // namespace eagine

#endif // EAGINE_MAIN_FWD_HPP
