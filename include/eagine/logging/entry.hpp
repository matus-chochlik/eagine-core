/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_ENTRY_HPP
#define EAGINE_LOGGING_ENTRY_HPP

#include "../bitfield.hpp"
#include "../memory/object_storage.hpp"
#include "../message_id.hpp"
#include "../valid_if/decl.hpp"
#include "backend.hpp"
#include <sstream>

namespace eagine {
//------------------------------------------------------------------------------
static inline auto adapt_entry_arg(
  const identifier name,
  logger_backend* value) {
    return [name, value](logger_backend& backend) noexcept {
        if(value) {
            backend.add_identifier(
              name, EAGINE_ID(LogBkEndId), value->type_id());
        } else {
            backend.add_nothing(name, EAGINE_ID(LogBkEndId));
        }
    };
}
//------------------------------------------------------------------------------
class logger;

/// @brief Class representing a single log entry / message.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
class log_entry {
public:
    /// @brief Constructor.
    log_entry(
      const identifier source_id,
      const logger_instance_id instance_id,
      const log_event_severity severity,
      const string_view format,
      logger_backend* backend) noexcept
      : _backend{backend}
      , _source_id{source_id}
      , _instance_id{instance_id}
      , _format{format}
      , _args{_be_alloc(_backend)}
      , _severity{severity} {
        if(_backend) {
            _args.reserve(16);
        }
    }

    /// @brief Not moveable.
    log_entry(log_entry&&) = delete;
    /// @brief Not copyable.
    log_entry(const log_entry&) = delete;
    /// @brief Not copy assignable.
    auto operator=(log_entry&&) = delete;
    /// @brief Not move assignable.
    auto operator=(const log_entry&) = delete;

    /// @brief Destructor. Passed the actual entry to the backend.
    ~log_entry() noexcept {
        if(_backend) {
            if(_backend->begin_message(
                 _source_id, _entry_tag, _instance_id, _severity, _format))
              [[likely]] {
                _args(*_backend);
                _backend->finish_message();
            }
        }
    }

    /// @brief Adds a new message argument with identifier value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const identifier value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_identifier(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with identifier value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const identifier value) noexcept -> auto& {
        return arg(name, EAGINE_ID(Identifier), value);
    }

    /// @brief Adds a new message argument with message_id type value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const message_id value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_message_id(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with identifier value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const message_id value) noexcept -> auto& {
        return arg(name, EAGINE_ID(MessageId), value);
    }

    /// @brief Adds a new message argument with 64-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::int64_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_integer(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 64-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::int64_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(int64), value);
    }

    /// @brief Adds a new message argument with 64-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::int64_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_integer(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 64-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::int64_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(int64), values);
    }

    /// @brief Adds a new message argument with 32-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::int32_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_integer(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 32-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::int32_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(int32), value);
    }

    /// @brief Adds a new message argument with 32-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::int32_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_integer(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 32-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::int32_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(int32), values);
    }

    /// @brief Adds a new message argument with 16-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::int16_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_integer(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 16-bit signed integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::int16_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(int16), value);
    }

    /// @brief Adds a new message argument with 16-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::int16_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_integer(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 16-bit signed integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::int16_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(int16), values);
    }

    /// @brief Adds a new message argument with 64-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::uint64_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_unsigned(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 64-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::uint64_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(int64), value);
    }

    /// @brief Adds a new message argument with 64-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::uint64_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_unsigned(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 64-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::uint64_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(uint64), values);
    }

    /// @brief Adds a new message argument with 32-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::uint32_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_unsigned(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 32-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::uint32_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(uint32), value);
    }

    /// @brief Adds a new message argument with 32-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::uint32_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_unsigned(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 32-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::uint32_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(uint32), values);
    }

    /// @brief Adds a new message argument with 16-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::uint16_t value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_unsigned(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 16-bit unsigned integer value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::uint16_t value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(uint16), value);
    }

    /// @brief Adds a new message argument with 16-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const std::uint16_t> values) noexcept -> log_entry& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                for(const auto value : values) {
                    backend.add_unsigned(name, tag, value);
                }
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with 16-bit unsigned integer span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(
      const identifier name,
      const span<const std::uint16_t> values) noexcept -> auto& {
        return arg(name, EAGINE_ID(uint16), values);
    }

    /// @brief Adds a new message argument with floating-point value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const float value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_float(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with floating-point value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const float value) noexcept -> auto& {
        return arg(name, EAGINE_ID(real), value);
    }

    /// @brief Adds a new message argument with double-precision float value.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(const identifier name, const double value) noexcept -> auto& {
        return arg(name, float(value));
    }

    /// @brief Adds a new message argument with floating-point span.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    auto arg(
      const identifier name,
      const identifier tag,
      const span<const float> values) noexcept -> log_entry& {
        if(_backend) {
            for(const auto value : values) {
                _args.add([=](logger_backend& backend) {
                    backend.add_float(name, tag, value);
                });
            }
        }
        return *this;
    }

    /// @brief Adds a new message argument with floating-point span.
    /// @param name the argument name identifier. Used in message substitution.
    auto arg(const identifier name, const span<const float> values) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(real), values);
    }

    /// @brief Adds a new message argument with floating-point value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_float(name, tag, min, value, max);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with floating-point value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const float min,
      const float value,
      const float max) noexcept -> auto& {
        return arg(name, EAGINE_ID(real), min, value, max);
    }

    /// @brief Adds a new message argument with time duration value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    template <typename R, typename P>
    auto arg(
      const identifier name,
      const identifier tag,
      const std::chrono::duration<R, P> value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_duration(
                  name,
                  tag,
                  std::chrono::duration_cast<std::chrono::duration<float>>(
                    value));
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with time duration value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    template <typename R, typename P>
    auto arg(
      const identifier name,
      const std::chrono::duration<R, P> value) noexcept -> auto& {
        return arg(name, EAGINE_ID(duration), value);
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
            _args.add([=](logger_backend& backend) {
                backend.add_string(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with string value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const string_view value) noexcept -> auto& {
        return arg(name, EAGINE_ID(str), value);
    }

    /// @brief Adds a new message argument with string value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const std::string& value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_string(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with string value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const std::string& value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(str), value);
    }

    /// @brief Adds a new message argument with BLOB value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param value the value of the argument.
    auto arg(
      const identifier name,
      const identifier tag,
      const memory::const_block value) noexcept -> auto& {
        if(_backend) {
            _args.add([=](logger_backend& backend) {
                backend.add_blob(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with BLOB value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const memory::const_block value) noexcept
      -> auto& {
        return arg(name, EAGINE_ID(block), value);
    }

    /// @brief Adds a new message argument adapted by the specified function.
    template <typename Func>
    auto arg_func(Func function) -> auto& {
        if(_backend) {
            _args.add(std::move(function));
        }
        return *this;
    }

    /// @brief Adds a new message argument with adaptable-type value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    /// @see has_log_entry_adapter_v
    template <typename T>
    auto arg(const identifier name, T&& value) noexcept
      -> log_entry& requires(has_entry_adapter_v<std::decay_t<T>>) {
        if(_backend) {
            _args.add(adapt_entry_arg(name, std::forward<T>(value)));
        }
        return *this;
    }

    /// @brief Adds a new message argument with valid_if_or_fallback value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param opt the value of the argument.
    /// @see valid_if_or_fallback
    template <typename F, typename T, typename P, typename L>
    auto arg(
      const identifier name,
      const identifier tag,
      valid_if_or_fallback<F, T, P, L>&& opt) noexcept
      -> log_entry& requires(
        has_entry_function_v<log_entry, std::decay_t<T>>&&
          has_entry_function_v<log_entry, std::decay_t<F>>) {
        if(opt.is_valid()) {
            return arg(name, tag, std::move(opt.value()));
        }
        return arg(name, std::move(opt.fallback()));
    }

    /// @brief Adds a new message argument with valid_if or fallback value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param opt the primary optional value of the argument.
    /// @param fbck the fallback value of opt is not valid.
    /// @see valid_if_or_fallback
    template <typename F, typename T, typename P, typename L>
    auto arg(
      const identifier name,
      const identifier tag,
      basic_valid_if<T, P, L> opt,
      F fbck) noexcept
      -> log_entry& requires(
        has_entry_function_v<log_entry, std::decay_t<T>>&&
          has_entry_function_v<log_entry, std::decay_t<F>>) {
        return arg(name, tag, either_or(std::move(opt), std::move(fbck)));
    }

    /// @brief Adds an optional tag to this log entry.
    auto tag(const identifier entry_tag) noexcept -> auto& {
        _entry_tag = entry_tag;
        return *this;
    }

private:
    logger_backend* const _backend{nullptr};
    const identifier _source_id{};
    identifier _entry_tag{};
    const logger_instance_id _instance_id{};
    const string_view _format{};
    memory::callable_storage<void(logger_backend&)> _args;
    const log_event_severity _severity{log_event_severity::info};

    static auto _be_alloc(logger_backend* backend) noexcept
      -> memory::shared_byte_allocator {
        if(backend) {
            return backend->allocator();
        }
        return {};
    }
};
//------------------------------------------------------------------------------
/// @brief Do-nothing variant of log_entry with compatible API.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
struct no_log_entry {
    template <typename T>
    constexpr auto arg(const identifier, T&&) noexcept -> auto& {
        return *this;
    }
    template <typename T>
    constexpr auto arg(const identifier, const identifier, T&&) noexcept
      -> auto& {
        return *this;
    }
    constexpr auto arg(
      const identifier,
      const identifier,
      const float,
      const float,
      const float) noexcept -> auto& {
        return *this;
    }

    template <typename T>
    constexpr auto arg(const identifier, const span<const T>) noexcept
      -> auto& {
        return *this;
    }

    template <typename T>
    constexpr auto arg(
      const identifier,
      const identifier,
      const span<const T>) noexcept -> auto& {
        return *this;
    }

    template <typename T, typename P, typename F>
    constexpr auto arg(
      const identifier,
      const identifier,
      const valid_if<T, P>,
      const F&) noexcept -> auto& {
        return *this;
    }

    template <typename Func>
    constexpr auto arg_func(const Func&) -> auto& {
        return *this;
    }

    constexpr auto tag(const identifier) noexcept -> auto& {
        return *this;
    }
};
//------------------------------------------------------------------------------
/// @brief Log entry helper containing an istream for creating the log message.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
class stream_log_entry {
public:
    /// @brief Implicit conversion to an istream.
    operator std::ostream&() noexcept {
        return _out;
    }

    stream_log_entry(
      const identifier source_id,
      const logger_instance_id instance_id,
      const log_event_severity severity,
      logger_backend* backend) noexcept
      : _source_id{source_id}
      , _instance_id{instance_id}
      , _backend{backend}
      , _severity{severity} {}

    stream_log_entry(stream_log_entry&&) = default;
    stream_log_entry(const stream_log_entry&) = delete;
    auto operator=(stream_log_entry&&) = delete;
    auto operator=(const stream_log_entry&) = delete;

    ~stream_log_entry() noexcept {
        try {
            auto fmt_str(_out.str());
            if(!fmt_str.empty()) {
                if(_backend) {
                    if(_backend->begin_message(
                         _source_id,
                         _entry_tag,
                         _instance_id,
                         _severity,
                         fmt_str)) [[likely]] {
                        _backend->finish_message();
                        _backend = nullptr;
                    }
                }
            }
        } catch(...) {
        }
    }

    auto tag(const identifier entry_tag) noexcept -> auto& {
        _entry_tag = entry_tag;
        return *this;
    }

private:
    std::stringstream _out{};
    identifier _source_id{};
    identifier _entry_tag{};
    logger_instance_id _instance_id{};
    logger_backend* _backend{nullptr};
    const log_event_severity _severity{log_event_severity::info};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_ENTRY_HPP
