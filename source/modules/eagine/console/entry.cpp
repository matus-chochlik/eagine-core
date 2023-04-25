/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.console:entry;

import std;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.runtime;
import eagine.core.logging;
import :backend;

namespace eagine {
export class console_entry;

export class console_entry_continuation {
public:
    console_entry_continuation(const console_entry& entry) noexcept;
    console_entry_continuation(console_entry_continuation&&) = delete;
    console_entry_continuation(const console_entry_continuation&) = delete;
    auto operator=(console_entry_continuation&&) = delete;
    auto operator=(const console_entry_continuation&) = delete;
    ~console_entry_continuation() noexcept;

    auto print(console_entry_kind kind, const string_view format) const noexcept
      -> console_entry;

    auto print(const string_view format) const noexcept -> console_entry;

    auto warning(const string_view format) const noexcept -> console_entry;

    auto error(const string_view format) const noexcept -> console_entry;

private:
    auto _entry_backend(const identifier source, const console_entry_kind kind)
      const noexcept -> std::tuple<console_backend*, console_entry_id_t>;

    auto _make_log_entry(
      const console_entry_kind kind,
      const string_view format) const noexcept -> console_entry;

    console_backend* const _backend{};
    const console_entry_id_t _parent_id{};
    const console_entry_id_t _entry_id{};
    const identifier _source_id{};
    const console_entry_kind _kind{};
};

/// @brief Class representing a single log entry / message.
/// @ingroup console
/// @note Do not use directly, use console instead.
export class console_entry {
public:
    /// @brief Constructor.
    console_entry(
      const identifier source_id,
      const console_entry_id_t parent_id,
      const console_entry_id_t entry_id,
      const console_entry_kind kind,
      const string_view format,
      console_backend* backend) noexcept
      : _backend{backend}
      , _parent_id{parent_id}
      , _entry_id{entry_id}
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
            if(_backend->begin_message(
                 _source_id, parent_id(), id(), _kind, _format)) {
                _args(*_backend);
                _backend->finish_message();
            }
        }
    }

    /// @brief Returns the id of this entry.
    /// @see parent_id
    auto id() const noexcept -> console_entry_id_t {
        return reinterpret_cast<console_entry_id_t>(this);
    }

    /// @brief Returns the id of the parent entry.
    /// @see id
    auto parent_id() const noexcept -> console_entry_id_t {
        return _parent_id;
    }

    /// @brief Adds a separator after this entry message is printed.
    auto separate() noexcept -> auto& {
        if(_backend) {
            _backend->add_separator();
        }
        return *this;
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
            _args.add([=](console_backend& backend) {
                backend.add_identifier(name, tag, value);
            });
        }
        return *this;
    }

    /// @brief Adds a new message argument with identifier value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param value the value of the argument.
    auto arg(const identifier name, const identifier value) noexcept -> auto& {
        return arg(name, "Identifier", value);
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
        return arg(name, "bool", value);
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
        return arg(name, "signed", value);
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
        return arg(name, "unsigned", value);
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
        return arg(name, "float", value);
    }

    auto arg(
      const identifier name,
      const std::integral auto value,
      const std::integral auto max) noexcept -> auto& {
        return arg(name, "Ratio", double(value) / double(max));
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
            _args.add([=](console_backend& backend) {
                backend.add_duration(
                  name,
                  tag,
                  std::chrono::duration_cast<std::chrono::duration<float>>(
                    value));
            });
        }
        return *this;
    }

    template <typename R, typename P>
    auto arg(
      const identifier name,
      const std::chrono::duration<R, P> value) noexcept -> auto& {
        return arg(name, "seconds", value);
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
        return arg(name, "str", value);
    }

    auto arg(const identifier name, const std::string& value) noexcept
      -> auto& {
        return arg(name, string_view{value});
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
            _args.add([=](console_backend& backend) {
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
        return arg(name, "block", value);
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
    template <adapted_for_log_entry T>
        requires(not std::integral<T> and not std::floating_point<T>)
    auto arg(const identifier name, T value) noexcept -> console_entry& {
        if(_backend) {
            _args.add(adapt_entry_arg(name, std::move(value)));
        }
        return *this;
    }

    /// @brief Adds a new message argument with valid_if_or_fallback value.
    /// @param name the argument name identifier. Used in message substitution.
    /// @param tag the argument type identifier. Used in value formatting.
    /// @param opt the value of the argument.
    /// @see valid_if_or_fallback
    template <
      argument_of_log<console_entry> F,
      argument_of_log<console_entry> T,
      typename P,
      typename L>
    auto arg(
      const identifier name,
      const identifier tag,
      valid_if_or_fallback<F, T, P, L>&& opt) noexcept -> console_entry& {
        if(opt.has_value()) {
            return arg(name, tag, std::move(opt.value()));
        }
        return arg(name, std::move(opt.fallback()));
    }

    template <
      argument_of_log<console_entry> F,
      argument_of_log<console_entry> T,
      typename P,
      typename L>
    auto arg(
      const identifier name,
      valid_if_or_fallback<F, T, P, L>&& opt) noexcept -> console_entry& {
        if(opt.has_value()) {
            return arg(name, std::move(opt.value()));
        }
        return arg(name, std::move(opt.fallback()));
    }

    auto to_be_continued() const noexcept -> console_entry_continuation {
        return {*this};
    }

private:
    friend class console_entry_continuation;

    console_backend* const _backend{nullptr};
    const console_entry_id_t _parent_id{};
    const console_entry_id_t _entry_id{};
    const identifier _source_id{};
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

console_entry_continuation::console_entry_continuation(
  const console_entry& entry) noexcept
  : _backend{entry._backend}
  , _parent_id{entry._parent_id}
  , _entry_id{entry._entry_id}
  , _source_id{entry._source_id}
  , _kind{entry._kind} {
    if(_backend) {
        _backend->to_be_continued(_parent_id, _entry_id);
    }
}

console_entry_continuation::~console_entry_continuation() noexcept {
    if(_backend) {
        _backend->concluded(_entry_id);
    }
}

auto console_entry_continuation::_entry_backend(
  const identifier source,
  const console_entry_kind kind) const noexcept
  -> std::tuple<console_backend*, console_entry_id_t> {
    if(_backend) [[likely]] {
        return _backend->entry_backend(source, kind);
    }
    return {nullptr, 0};
}

auto console_entry_continuation::_make_log_entry(
  const console_entry_kind kind,
  const string_view format) const noexcept -> console_entry {
    const auto [backend, new_id] = _entry_backend(_source_id, kind);
    return {_source_id, _entry_id, new_id, kind, format, backend};
}

auto console_entry_continuation::print(
  console_entry_kind kind,
  const string_view format) const noexcept -> console_entry {
    return _make_log_entry(kind, format);
}

auto console_entry_continuation::print(const string_view format) const noexcept
  -> console_entry {
    return print(_kind, format);
}

auto console_entry_continuation::warning(const string_view format) const noexcept
  -> console_entry {
    return print(console_entry_kind::warning, format);
}

auto console_entry_continuation::error(const string_view format) const noexcept
  -> console_entry {
    return print(console_entry_kind::error, format);
}

} // namespace eagine

