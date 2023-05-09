/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:memoized;

import std;
import eagine.core.concepts;
import :callable_ref;

namespace eagine {

export template <typename F>
class memoized;

export template <typename R, typename... P>
class memoized<R(P...)> {
public:
    template <does_not_hide<memoized> Func>
    memoized(Func&& func) noexcept
      : _func{std::forward<Func>(func)} {}

    template <typename F>
    [[nodiscard]] auto operator()(P... p, const F& f) -> R {
        T t(p...);
        auto i = _memo.find(t);
        if(i == _memo.end()) {
            i = _memo.insert(E(t, f(p..., *this))).first;
        }
        return i->second;
    }

    [[nodiscard]] auto operator()(P... p) -> R {
        return _func(p..., *this);
    }

    void reset(P... p) {
        _memo.erase(T(p...));
    }

    void clear() {
        _memo.clear();
    }

private:
    using T = std::tuple<P...>;
    using E = std::pair<T, R>;
    std::map<T, R> _memo;
    callable_ref<R(P..., memoized&)> _func;
};

} // namespace eagine
