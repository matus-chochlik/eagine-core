/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:coroutine;

import std;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
class generator : public std::ranges::view_interface<generator<T>> {
public:
    class promise_type {
    public:
        auto get_return_object() noexcept -> generator {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        static auto initial_suspend() noexcept -> std::suspend_always {
            return {};
        }

        static auto final_suspend() noexcept -> std::suspend_always {
            return {};
        }

        auto yield_value(T value) noexcept -> std::suspend_always {
            _value.emplace(std::move(value));
            return {};
        }

        static void unhandled_exception() {
            throw;
        }

        auto get() noexcept -> auto& {
            return *_value;
        }

    private:
        std::optional<T> _value{};
    };

    using handle_type = std::coroutine_handle<promise_type>;

    generator() noexcept = default;

    generator(handle_type handle) noexcept
      : _handle{std::move(handle)} {}

    generator(generator&& that) noexcept
      : _handle{std::exchange(that._handle, handle_type{})} {}
    generator(const generator&) = delete;

    auto operator=(generator&& that) noexcept -> generator& {
        if(this != &that) [[likely]] {
            std::swap(_handle, that._handle);
        }
        return *this;
    }
    auto operator=(const generator&) = delete;

    ~generator() noexcept {
        if(_handle) {
            _handle.destroy();
        }
    }

    using sentinel = std::default_sentinel_t;

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
export template <typename... T>
class tuple_generator
  : public std::ranges::view_interface<tuple_generator<T...>> {
public:
    class promise_type {
    public:
        auto get_return_object() noexcept -> tuple_generator {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        static auto initial_suspend() noexcept -> std::suspend_always {
            return {};
        }

        static auto final_suspend() noexcept -> std::suspend_always {
            return {};
        }

        auto yield_value(std::tuple<T...> value) noexcept
          -> std::suspend_always {
            _value.emplace(std::move(value));
            return {};
        }

        static void unhandled_exception() {
            throw;
        }

        auto get() noexcept -> auto& {
            return *_value;
        }

    private:
        std::optional<std::tuple<T...>> _value{};
    };

    using handle_type = std::coroutine_handle<promise_type>;

    tuple_generator() noexcept = default;

    tuple_generator(handle_type handle) noexcept
      : _handle{std::move(handle)} {}

    tuple_generator(tuple_generator&& that) noexcept
      : _handle{std::exchange(that._handle, handle_type{})} {}
    tuple_generator(const tuple_generator&) = delete;

    auto operator=(tuple_generator&& that) noexcept -> tuple_generator& {
        if(this != &that) [[likely]] {
            std::swap(_handle, that._handle);
        }
        return *this;
    }
    auto operator=(const tuple_generator&) = delete;

    ~tuple_generator() noexcept {
        if(_handle) {
            _handle.destroy();
        }
    }

    using sentinel = std::default_sentinel_t;

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
export template <std::forward_iterator Iterator>
class pointee_generator
  : public std::ranges::view_interface<pointee_generator<Iterator>> {
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

    using sentinel = std::default_sentinel_t;

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
