/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:main;

import :interface;

namespace eagine {

export class main_ctx;
// TODO: actual separate implementation
export using test_ctx = main_ctx;
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
export [[nodiscard]] auto test_main_impl(
  int,
  const char**,
  int (*main_func)(test_ctx&)) -> int;
//------------------------------------------------------------------------------
} // namespace eagine
