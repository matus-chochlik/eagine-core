/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_IOSTREAM_BACKEND_HPP
#define EAGINE_CONSOLE_IOSTREAM_BACKEND_HPP

#include "../double_buffer.hpp"
#include "../str_var_subst.hpp"
#include "backend.hpp"
#include <array>
#include <charconv>
#include <istream>
#include <mutex>
#include <ostream>

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable = std::mutex>
class iostream_console_backend : public console_backend {
public:
    iostream_console_backend(std::ostream& out) noexcept
      : _out{out} {}

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return EAGINE_ID(InOutStrm);
    }

    auto entry_backend(const identifier, const console_entry_kind) noexcept
      -> console_backend* override {
        return this;
    }

    auto begin_message(
      const identifier source,
      const console_entry_kind kind,
      const string_view format) noexcept -> bool final {
        _lockable.lock();
        assign_to(_message.back(), format);
        _message.swap();
        _source = source;
        _kind = kind;
        return true;
    }

    void add_identifier(
      const identifier arg,
      const identifier,
      const identifier value) noexcept final {
        _add(arg, value.name().view());
    }

    void add_bool(
      const identifier arg,
      const identifier,
      const bool value) noexcept final {
        if(value) {
            _add(arg, string_view{"true"});
        } else {
            _add(arg, string_view{"false"});
        }
    }

    void add_integer(
      const identifier arg,
      const identifier,
      const std::intmax_t value) noexcept final {
        return _add(arg, value);
    }

    void add_unsigned(
      const identifier arg,
      const identifier,
      const std::uintmax_t value) noexcept final {
        return _add(arg, value);
    }

    void add_float(
      const identifier arg,
      const identifier,
      const double value) noexcept final {
        return _add(arg, value);
    }

    void add_duration(
      const identifier arg,
      const identifier,
      const std::chrono::duration<float> value) noexcept final {
        // TODO
        return _add(arg, value.count());
    }

    void add_string(
      const identifier arg,
      const identifier,
      const string_view value) noexcept final {
        _add(arg, value);
    }

    void finish_message() noexcept final {
        try {
            _out << _message.front() << '\n';
            _lockable.unlock();
        } catch(...) {
        }
    }

private:
    template <typename T>
    auto _to_str(T value) noexcept -> optionally_valid<string_view> {
        auto b{_temp.data()};
        auto e{b + _temp.size()};
        if(const auto conv{std::to_chars(b, e, value)}; conv.ec == std::errc())
          [[likely]] {
            return {head(view(_temp), std::distance(b, conv.ptr)), true};
        }
        return {};
    }

    void _add(const identifier arg, const string_view value) noexcept {
        _message.back().clear();
        substitute_variable_into(
          _message.back(), _message.front(), arg.name(), value);
        _message.swap();
    }

    void _add(const identifier arg, const std::string& value) noexcept {
        _add(arg, string_view{value});
    }

    template <typename T>
    void _add(const identifier arg, T value) noexcept {
        if(const auto str_val{_to_str(value)}) {
            _add(arg, extract(str_val));
        }
    }

    Lockable _lockable{};
    std::ostream& _out;
    std::array<char, 64> _temp{};
    double_buffer<std::string> _message;
    identifier _source{};
    console_entry_kind _kind{console_entry_kind::info};
};
} // namespace eagine
#endif
