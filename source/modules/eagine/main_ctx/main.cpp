/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:main;

import :interface;

namespace eagine {

export class main_ctx;

export auto main_impl(
  int,
  const char**,
  main_ctx_options&,
  int (*main_func)(main_ctx&)) -> int;

export [[nodiscard]] auto default_main(
  int argc,
  const char** argv,
  int (*main_func)(main_ctx&)) -> int {
    main_ctx_options options{};
    return main_impl(argc, argv, options, main_func);
}

} // namespace eagine
