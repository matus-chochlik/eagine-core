/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:cleanup_group;

import std;
import eagine.core.types;
import eagine.core.memory;
import :scope_exit;

namespace eagine {
//------------------------------------------------------------------------------
export class cleanup_group {
private:
    memory::object_storage _storage;

public:
    template <typename Func>
    auto add(Func func) -> auto& requires(
      std::is_invocable_v<Func>&& std::is_void_v<std::invoke_result_t<Func>>) {
        return _storage.template emplace<func_on_scope_exit<Func>>(
          std::move(func));
    }

    template <typename Func>
    auto add_ret(Func func) -> auto&
        requires(std::is_invocable_v<Func>)
    {
        return add([func{std::move(func)}]() mutable { func(); });
    }

    void reserve(span_size_t n) {
        _storage.reserve(n);
    }

    [[nodiscard]] auto is_empty() const noexcept {
        return _storage.is_empty();
    }

    void clear() noexcept {
        _storage.clear();
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

