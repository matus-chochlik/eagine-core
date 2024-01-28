/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:overloaded;

import std;

namespace eagine {

export template <typename... Func>
struct overloaded;

export template <typename Func>
struct overloaded<Func> : Func {
    overloaded(const Func& func)
      : Func{func} {}

    overloaded(Func&& func)
      : Func{std::move(func)} {}

    using Func::operator();
};

export template <typename Func, typename... Funcs>
struct overloaded<Func, Funcs...>
  : Func
  , overloaded<Funcs...> {
    template <typename... F>
    overloaded(const Func& func, F&&... funcs)
      : Func{func}
      , overloaded<Funcs...>{std::forward<Funcs>(funcs)...} {}

    template <typename... F>
    overloaded(Func&& func, F&&... funcs)
      : Func{std::move(func)}
      , overloaded<Funcs...>{std::forward<Funcs>(funcs)...} {}

    using Func::operator();
    using overloaded<Funcs...>::operator();
};

export template <typename... F>
overloaded(const F&...) -> overloaded<F...>;

} // namespace eagine
