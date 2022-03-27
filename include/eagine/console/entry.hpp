/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_ENTRY_HPP
#define EAGINE_CONSOLE_ENTRY_HPP

#include "../memory/object_storage.hpp"
#include "backend.hpp"
#include <concepts>

namespace eagine {
/// @brief Class representing a single log entry / message.
/// @ingroup console
/// @note Do not use directly, use console instead.
class console_entry {
public:
    /// @brief Constructor.
    console_entry(
      const identifier source_id,
      const console_entry_kind kind,
      const string_view format,
      console_backend* backend) noexcept
      : _backend{backend}
      , _source_id{source_id}
      , _format{format}
      , _args{_be_alloc(_backend)}
      , _kind{kind} {
        if(_backend) {
            _args.reserve(16);
        }
    }

    /// @brief Not moveable.
    console_entry(console_entry&&) = delete;
    /// @brief Not copyable.
    console_entry(const console_entry&) = delete;
    /// @brief Not copy assignable.
    auto operator=(console_entry&&) = delete;
    /// @brief Not move assignable.
    auto operator=(const console_entry&) = delete;

    /// @brief Destructor. Passed the actual entry to the backend.
    ~console_entry() noexcept {
        if(_backend) {
            if(_backend->begin_message(_source_id, _kind, _format)) {
                _args(*_backend);
                _backend->finish_message();
            }
        }
    }

    /// @brief Adds a new message argument with boolean value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const bool value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](console_backend& backend) {
                backend.add_bool(name, tag, value);
            });
        }
        return *this;
    }

    auto arg(const identifier name, const bool value) noexcept -> auto& {
        return arg(name, EAGINE_ID(bool), value);
    }

    /// @brief Adds a new message argument with signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::signed_integral auto value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](console_backend& backend) {
                backend.add_integer(name, tag, value);
            });
        }
        return *this;
    }

    auto arg(
      const identifier name,
      const std::signed_integral auto value) noexcept -> auto& {
        return arg(name, EAGINE_ID(signed), value);
    }

    /// @brief Adds a new message argument with unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::unsigned_integral auto value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](console_backend& backend) {
                backend.add_unsigned(name, tag, value);
            });
        }
        return *this;
    }

    auto arg(
      const identifier name,
      const std::unsigned_integral auto value) noexcept -> auto& {
        return arg(name, EAGINE_ID(unsigned), value);
    }

    /// @brief Adds a new message argument with floating-point value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::floating_point auto value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](console_backend& backend) {
                backend.add_float(name, tag, static_cast<double>(value));
            });
        }
        return *this;
    }

    auto arg(
      const identifier name,
      const std::floating_point auto value) noexcept -> auto& {
        return arg(name, EAGINE_ID(float), value);
    }

    /// @brief Adds a new message argument with string value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const string_view value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](console_backend& backend) {
                backend.add_string(name, tag, value);
            });
        }
        return *this;
    }

    auto arg(const identifier name, const string_view value) noexcept -> auto& {
        return arg(name, EAGINE_ID(str), value);
    }

    auto arg(const identifier name, const std::string& value) noexcept
      -> auto& {
        return arg(name, string_view{value});
    }

private:
    console_backend* const _backend{nullptr};
    identifier _source_id{};
    const string_view _format{};
    memory::callable_storage<void(console_backend&)> _args;
    const console_entry_kind _kind{console_entry_kind::info};

    static auto _be_alloc(console_backend* backend) noexcept
      -> memory::shared_byte_allocator {
        if(backend) {
            return backend->allocator();
        }
        return {};
    }
};
} // namespace eagine

#endif

