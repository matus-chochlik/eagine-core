/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:coroutine;

import std;

namespace eagine {
//------------------------------------------------------------------------------
export template <std::forward_iterator Iterator>
class pointee_generator {
public:
    class promise_type {
    public:
        using iterator = Iterator;

        auto get_return_object() noexcept -> pointee_generator {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        static auto initial_suspend() noexcept -> std::suspend_always {
            return {};
        }

        static auto final_suspend() noexcept -> std::suspend_always {
            return {};
        }

        auto yield_value(iterator value) noexcept -> std::suspend_always {
            _pos = std::move(value);
            return {};
        }

        static void unhandled_exception() {
            throw;
        }

        auto get() noexcept -> auto& {
            return *_pos;
        }

    private:
        iterator _pos{};
    };

    using handle_type = std::coroutine_handle<promise_type>;

    pointee_generator() noexcept = default;

    pointee_generator(handle_type handle) noexcept
      : _handle{std::move(handle)} {}

    pointee_generator(pointee_generator&& that) noexcept
      : _handle{std::exchange(that._handle, handle_type{})} {}
    pointee_generator(const pointee_generator&) = delete;

    auto operator=(pointee_generator&& that) noexcept -> pointee_generator& {
        if(this != &that) [[likely]] {
            std::swap(_handle, that._handle);
        }
        return *this;
    }
    auto operator=(const pointee_generator&) = delete;

    ~pointee_generator() noexcept {
        if(_handle) {
            _handle.destroy();
        }
    }

    struct sentinel {};

    class iterator {
    public:
        iterator(handle_type handle) noexcept
          : _handle{std::move(handle)} {}

        void operator++() noexcept {
            _handle.resume();
        }

        [[nodiscard]] auto operator*() noexcept -> auto& {
            return _handle.promise().get();
        }

        [[nodiscard]] auto operator==(sentinel) const noexcept -> bool {
            return not _handle or _handle.done();
        }

        [[nodiscard]] auto operator!=(sentinel) const noexcept -> bool {
            return _handle and not _handle.done();
        }

    private:
        handle_type _handle;
    };

    [[nodiscard]] auto begin() const noexcept -> iterator {
        if(_handle) [[likely]] {
            _handle.resume();
        }
        return {_handle};
    }

    [[nodiscard]] auto end() const noexcept -> sentinel {
        return {};
    }

private:
    std::coroutine_handle<promise_type> _handle;
};
//------------------------------------------------------------------------------
} // namespace eagine
