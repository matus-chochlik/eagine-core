/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.runtime:program_args;

import std;
import eagine.core.debug;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
export class program_args;
export class program_arg_iterator;

/// @brief Class representing a single main function (command-line) argument.
/// @ingroup main_context
/// @see program_args
/// @note Do not construct directly. Use program_arguments.
export class program_arg {
public:
    /// @brief Default constructor.
    constexpr program_arg() noexcept = default;

    program_arg(const int argi, const int argc, const char** argv) noexcept
      : _argi{argi}
      , _argc{argc}
      , _argv{argv} {}

    /// @brief Alias for the argument value type.
    using value_type = memory::string_view;

    /// @brief Indicates if the arguments is valid.
    auto has_value() const noexcept -> bool {
        return (0 <= _argi) and (_argi < _argc) and (_argv != nullptr) and
               (_argv[_argi] != nullptr);
    }

    /// @brief Indicates if the arguments is valid.
    /// @see has_value
    operator bool() const noexcept {
        return has_value();
    }

    /// @brief Indicates if this argument's value is equal to the specified string.
    auto operator==(const value_type& v) const noexcept {
        return are_equal(get(), v);
    }

    /// @brief Indicates if this argument's value is different than the specified string.
    auto operator!=(const value_type& v) const noexcept {
        return not are_equal(get(), v);
    }

    /// @brief Returns the index of this argument.
    auto position() const noexcept {
        return _argi;
    }

    /// @brief Indicates if this is the first command-line argument.
    auto is_first() const noexcept -> bool {
        return _argi == 1;
    }

    /// @brief Indicates if this is the last command-line argument.
    auto is_last() const noexcept -> bool {
        return _argi == _argc - 1;
    }

    /// @brief Returns the stored value if has value or returns fallback.
    template <std::convertible_to<value_type> U>
    auto value_or(U&& fallback) noexcept -> value_type {
        if(has_value()) {
            return value_type(_argv[_argi]);
        }
        return value_type(std::forward<U>(fallback));
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <
      typename F,
      optional_like R =
        std::remove_cvref_t<std::invoke_result_t<F, const value_type&>>>
    auto and_then(F&& function) const -> R {
        if(has_value()) {
            return std::invoke(
              std::forward<F>(function), value_type(_argv[_argi]));
        } else {
            return R{};
        }
    }

    /// @brief Returns the result of function if empty or argument value otherwise.
    /// @transform
    /// @and_then
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto or_else(F&& function) const -> R {
        if(has_value()) {
            return R{value_type(_argv[_argi])};
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    /// @brief Returns the value of this argument if valid, an empty string view otherwise.
    auto get() const noexcept -> value_type {
        if(has_value()) {
            return value_type(_argv[_argi]);
        }
        return {};
    }

    /// @brief Returns the value of this argument if valid, an empty string view otherwise.
    auto or_default() noexcept -> value_type {
        return get();
    }

    /// @brief Implicit cast to value_type.
    /// @see get
    operator value_type() const noexcept {
        return get();
    }

    /// @brief Returns the value of this argument as a standard string.
    auto get_string() const -> std::string {
        return get().to_string();
    }

    /// @brief Returns the value of this argument as a const memory block.
    auto block() const noexcept -> memory::const_block {
        return memory::as_bytes(get());
    }

    /// @brief Indicates if this argument value starts with the specified string.
    /// @see ends_with
    /// @see is_short_tag
    /// @see is_long_tag
    auto starts_with(const value_type str) const noexcept {
        return memory::starts_with(get(), str);
    }

    /// @brief Indicates if this argument value ends with the specified string.
    /// @see starts_with
    /// @see is_short_tag
    /// @see is_long_tag
    auto ends_with(const value_type str) const noexcept {
        return memory::ends_with(get(), str);
    }

    /// @brief Indicates if this argument value is equal to the specified string.
    /// @see is_short_tag
    /// @see is_long_tag
    auto is_tag(const value_type tag) const noexcept {
        return are_equal(get(), tag);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '-'.
    /// @see is_tag
    /// @see is_long_tag
    /// @see is_prefixed_tag
    auto is_short_tag(const value_type name) const noexcept {
        return (safe_add_eq(1, name.size(), get().size()) and
                starts_with("-") and ends_with(name)) or
               is_tag(name);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '--'.
    /// @see is_tag
    /// @see is_short_tag
    /// @see is_prefixed_tag
    auto is_long_tag(const value_type name) const noexcept {
        return (safe_add_eq(2, name.size(), get().size()) and
                starts_with("--") and ends_with(name)) or
               is_tag(name);
    }

    /// @brief Indicates if this argument value is one of the two specified strings.
    auto is_tag(const value_type short_tag, const value_type long_tag)
      const noexcept {
        return is_short_tag(short_tag) or is_long_tag(long_tag);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '--'.
    /// @see is_tag
    /// @see is_long_tag
    /// @see is_short_tag
    auto is_prefixed_tag(const value_type prefix, const value_type name)
      const noexcept {
        return safe_add_eq(prefix.size(), name.size(), get().size()) and
               starts_with(prefix) and ends_with(name);
    }

    /// @brief Indicates if this argument is a "show help" argument (like "--help").
    auto is_help_arg() const noexcept {
        return is_tag(value_type("-h"), value_type("--help"));
    }

    /// @brief Returns the argument following this one.
    /// @see prev
    auto next() const noexcept -> program_arg {
        return {_argi + 1, _argc, _argv};
    }

    /// @brief Returns the argument preceding this one.
    /// @see next
    auto prev() const noexcept -> program_arg {
        return {_argi - 1, _argc, _argv};
    }

private:
    int _argi{0};
    int _argc{0};
    const char** _argv{nullptr};

    friend class program_arg_iterator;
    friend class program_args;
};
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
auto from_string(
  const program_arg& arg,
  const std::type_identity<T> tid,
  const selector<V> self) noexcept -> optionally_valid<T> {
    if(arg.has_value()) {
        return from_string(arg.get(), tid, self);
    }
    return {};
}

export template <typename T, identifier_t V>
auto assign_if_fits(
  const program_arg& src,
  T& dst,
  const selector<V> sel = default_selector) noexcept -> bool {
    return assign_if_fits(src.get(), dst, sel);
}

export template <typename T>
auto assign_if_fits(const program_arg& src, T& dst) noexcept -> bool {
    return assign_if_fits(src, dst, default_selector);
}
//------------------------------------------------------------------------------
export auto to_string(const program_arg& arg) {
    return arg.get_string();
}
//------------------------------------------------------------------------------
export auto operator<<(std::ostream& out, const program_arg& arg)
  -> std::ostream& {
    return out << arg.get();
}
//------------------------------------------------------------------------------
/// @brief Iterator type over program_arg instances.
export class program_arg_iterator {
    using this_class = program_arg_iterator;

public:
    constexpr program_arg_iterator() noexcept = default;

    constexpr program_arg_iterator(const program_arg arg) noexcept
      : _a{arg} {}

    /// @brief Alias for the referenced value type.
    using value_type = program_arg;

    /// @brief Alias for difference type.
    using difference_type = int;

    /// @brief Alias for reference type.
    using reference = const program_arg&;

    /// @brief Alias for const reference type.
    using const_reference = const program_arg&;

    /// @brief Alias for pointer type.
    using pointer = const program_arg*;

    /// @brief Alias for const pointer type.
    using const_pointer = const program_arg*;

    /// @brief Alias for iterator category.
    using iterator_category = std::random_access_iterator_tag;

    /// @brief Equality comparison.
    friend constexpr auto operator==(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) == 0;
    }

    /// @brief Nonequality comparison.
    friend constexpr auto operator!=(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) != 0;
    }

    /// @brief Less-than comparison.
    friend constexpr auto operator<(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) < 0;
    }

    /// @brief Less-equal comparison.
    friend constexpr auto operator<=(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) <= 0;
    }

    /// @brief Greater-than comparison.
    friend constexpr auto operator>(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) > 0;
    }

    /// @brief Greater-equal comparison.
    friend constexpr auto operator>=(
      const this_class& l,
      const this_class& r) noexcept {
        return _cmp(l._a, r._a) >= 0;
    }

    /// @brief Difference.
    friend constexpr auto operator-(
      const this_class& l,
      const this_class& r) noexcept -> difference_type {
        return _cmp(l._a, r._a);
    }

    /// @brief Pre-increment operator.
    constexpr auto operator++() noexcept -> this_class& {
        ++_a._argi;
        return *this;
    }

    /// @brief Pre-decrement operator.
    constexpr auto operator--() noexcept -> this_class& {
        --_a._argi;
        return *this;
    }

    /// @brief Post-increment operator.
    constexpr auto operator++(int) noexcept -> this_class {
        this_class result{*this};
        ++_a._argi;
        return result;
    }

    /// @brief Post-decrement operator.
    constexpr auto operator--(int) noexcept -> this_class {
        this_class result{*this};
        --_a._argi;
        return result;
    }

    /// @brief Increment operator.
    constexpr auto operator+=(const difference_type dist) noexcept
      -> this_class& {
        _a._argi += dist;
        return *this;
    }

    /// @brief Decrement operator.
    constexpr auto operator-=(const difference_type dist) noexcept
      -> this_class& {
        _a._argi -= dist;
        return *this;
    }

    /// @brief Difference addition.
    constexpr auto operator+(const difference_type dist) noexcept
      -> this_class {
        this_class result{*this};
        result._a._argi += dist;
        return result;
    }

    /// @brief Difference subtraction.
    constexpr auto operator-(const difference_type dist) noexcept
      -> this_class {
        this_class result{*this};
        result._a._argi -= dist;
        return result;
    }

    /// @brief Arrow operator.
    constexpr auto operator->() noexcept -> pointer {
        return &_a;
    }

    /// @brief Const arrow operator.
    constexpr auto operator->() const noexcept -> const_pointer {
        return &_a;
    }

    /// @brief Dereference operator.
    constexpr auto operator*() noexcept -> reference {
        return _a;
    }

    /// @brief Const dereference operator.
    constexpr auto operator*() const noexcept -> const_reference {
        return _a;
    }

private:
    static constexpr auto _cmp(
      const program_arg& l,
      const program_arg& r) noexcept -> int {
        assert(l._argv == r._argv);
        return l._argi - r._argi;
    }

    program_arg _a{};
};
//------------------------------------------------------------------------------
/// @brief Class wrapping the main function arguments, providing a convenient API.
/// @ingroup main_context
export class program_args {
public:
    /// @brief  Construction from the length and pointer to the argument list.
    program_args(const span_size_t argn, char** args) noexcept
      : _argc{int(argn)}
      , _argv{const_cast<const char**>(args)} {}

    /// @brief  Construction from the length and const pointer to the argument list.
    program_args(const span_size_t argn, const char** args) noexcept
      : _argc{int(argn)}
      , _argv{args} {}

    /// @brief Alias for the element value type.
    using value_type = memory::string_view;

    /// @brief Alias for the element count type.
    using size_type = int;

    /// @brief Alias for iterator type.
    using iterator = program_arg_iterator;

    /// @brief Returns the number of arguments.
    /// @see argv
    auto argc() const noexcept -> int {
        return _argc;
    }

    /// @brief Returns a pointer to tha list of argument C-strings.
    /// @see argc
    auto argv() const noexcept -> const char** {
        return _argv;
    }

    /// @brief Indicates if the argument list is completely empty.
    /// @see none
    /// @see size
    auto empty() const noexcept -> bool {
        return _argc <= 0;
    }

    /// @brief Indicates if the argument list does not contain any arguments.
    /// @see empty
    /// @see size
    auto none() const noexcept -> bool {
        return _argc <= 1;
    }

    /// @brief Returns the count of arguments (counting in the command name)
    /// @see empty
    /// @see none
    auto size() const noexcept -> int {
        return _argc;
    }

    /// @brief Returns the command line argument value at the specified position.
    /// @pre has_value(pos)
    auto get(const size_type pos) const noexcept -> program_arg {
        return {pos, _argc, _argv};
    }

    /// @brief Returns the command line argument value at the specified position.
    /// @pre has_value(pos)
    auto operator[](const size_type pos) const noexcept -> value_type {
        return get(pos).get();
    }

    /// @brief Returns the command name.
    auto command() const noexcept -> program_arg {
        return get(0);
    }

    /// @brief Returns first argument.
    /// @see last
    auto first() const noexcept -> program_arg {
        return get(1);
    }

    /// @brief Returns the first argument past the last.
    /// @see first
    auto past_last() const noexcept -> program_arg {
        return get(_argc == 0 ? 1 : _argc);
    }

    /// @brief Returns an iterator to the first argument (not the command name).
    /// @see end
    auto begin() const noexcept -> iterator {
        return {first()};
    }

    /// @brief Returns an iterator past the last argument.
    /// @begin
    auto end() const noexcept -> iterator {
        return {past_last()};
    }

    /// @brief Indicates if the specified argument is found in this list.
    auto has(const value_type what) const noexcept -> bool {
        if(_argv != nullptr) {
            int i = 1;
            while(i < _argc) {
                if(_argv[i] != nullptr) {
                    if(are_equal(value_type(_argv[i]), what)) {
                        return true;
                    }
                }
                ++i;
            }
        }
        return false;
    }

    /// @brief Indicates if any of the specified arguments are found in this list.
    template <std::convertible_to<value_type>... Str>
        requires(sizeof...(Str) > 0)
    auto has_any(Str&&... what) const noexcept -> bool {
        return (... or has(std::forward<Str>(what)));
    }

    /// @brief Finds and returns the argument with the specified value.
    /// @see has
    /// @see has_any
    /// @see all_like
    auto find(const value_type what) const noexcept -> program_arg {
        if(_argv != nullptr) {
            int i = 1;
            while(i < _argc) {
                if(_argv[i] != nullptr) {
                    if(are_equal(value_type(_argv[i]), what)) {
                        break;
                    }
                }
                ++i;
            }
            return get(i);
        }
        return past_last();
    }

    /// @brief Lists all arguments with the specified value.
    /// @see has
    /// @see has_any
    /// @see find
    auto all_like(const value_type what) const noexcept
      -> pointee_generator<program_arg_iterator> {
        if(_argv != nullptr) {
            int i = 1;
            while(i < _argc) {
                if(_argv[i] != nullptr) {
                    if(are_equal(value_type(_argv[i]), what)) {
                        co_yield get(i);
                    }
                }
                ++i;
            }
        }
    }

private:
    const int _argc{0};
    const char** _argv{nullptr};
};
//------------------------------------------------------------------------------
} // namespace eagine
