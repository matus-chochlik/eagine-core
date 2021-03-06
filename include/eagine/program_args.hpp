/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_PROGRAM_ARGS_HPP
#define EAGINE_PROGRAM_ARGS_HPP

#include "assert.hpp"
#include "from_string.hpp"
#include "integer_range.hpp"
#include "interface.hpp"
#include "memory/block.hpp"
#include "memory/span_algo.hpp"
#include "range_types.hpp"
#include "selector.hpp"
#include "span.hpp"
#include "string_span.hpp"
#include "type_name.hpp"
#include "valid_if/in_list.hpp"
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

namespace eagine {
//------------------------------------------------------------------------------
class program_args;
class program_arg_iterator;

/// @brief Class representing a single main function (command-line) argument.
/// @ingroup main_context
/// @see program_args
/// @note Do not construct directly. Use program_arguments.
class program_arg {
public:
    /// @brief Default constructor.
    constexpr program_arg() noexcept = default;

    program_arg(const int argi, const int argc, const char** argv) noexcept
      : _argi{argi}
      , _argc{argc}
      , _argv{argv} {}

    /// @brief Alias for the argument value type.
    using value_type = string_view;

    /// @brief Indicates if the arguments is valid.
    auto is_valid() const noexcept -> bool {
        return (0 < _argi) && (_argi < _argc) && (_argv != nullptr) &&
               (_argv[_argi] != nullptr);
    }

    /// @brief Indicates if the arguments is valid.
    /// @see is_valid
    operator bool() const noexcept {
        return is_valid();
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

    /// @brief Returns the value of this argument if valid, an empty string view otherwise.
    auto get() const noexcept -> value_type {
        if(is_valid()) {
            return value_type(_argv[_argi]);
        }
        return {};
    }

    /// @brief Returns the value of this argument as a const memory block.
    auto block() const noexcept -> memory::const_block {
        return memory::as_bytes(get());
    }

    /// @brief Returns the value of this argument as a standard string.
    auto get_string() const -> std::string {
        return get().to_string();
    }

    /// @brief Implicit cast to value_type.
    /// @see get
    operator value_type() const noexcept {
        return get();
    }

    /// @brief Indicates if this argument value starts with the specified string.
    /// @see ends_with
    /// @see is_short_tag
    /// @see is_long_tag
    auto starts_with(const string_view str) const noexcept {
        return memory::starts_with(get(), str);
    }

    /// @brief Indicates if this argument value ends with the specified string.
    /// @see starts_with
    /// @see is_short_tag
    /// @see is_long_tag
    auto ends_with(const string_view str) const noexcept {
        return memory::ends_with(get(), str);
    }

    /// @brief Indicates if this argument value is equal to the specified string.
    /// @see is_short_tag
    /// @see is_long_tag
    auto is_tag(const string_view tag) const noexcept {
        return are_equal(get(), tag);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '-'.
    /// @see is_tag
    /// @see is_long_tag
    /// @see is_prefixed_tag
    auto is_short_tag(const string_view name) const noexcept {
        return (safe_add_eq(1, name.size(), get().size()) && starts_with("-") &&
                ends_with(name)) ||
               is_tag(name);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '--'.
    /// @see is_tag
    /// @see is_short_tag
    /// @see is_prefixed_tag
    auto is_long_tag(const string_view name) const noexcept {
        return (safe_add_eq(2, name.size(), get().size()) &&
                starts_with("--") && ends_with(name)) ||
               is_tag(name);
    }

    /// @brief Indicates if argument is same as the specified string prefixed by '--'.
    /// @see is_tag
    /// @see is_long_tag
    /// @see is_short_tag
    auto is_prefixed_tag(const string_view prefix, const string_view name)
      const noexcept {
        return safe_add_eq(prefix.size(), name.size(), get().size()) &&
               starts_with(prefix) && ends_with(name);
    }

    /// @brief Indicates if this argument value is one of the two specified strings.
    auto is_tag(const string_view short_tag, const string_view long_tag)
      const noexcept {
        return is_short_tag(short_tag) || is_long_tag(long_tag);
    }

    /// @brief Indicates if this argument is a "show help" argument (like "--help").
    auto is_help_arg() const noexcept {
        return is_tag(string_view("-h"), string_view("--help"));
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

    /// @brief Tries to parse this argument's value into @p dest.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T, identifier_t V>
    auto parse(T& dest, const selector<V> sel, std::ostream& parse_log) const
      -> bool {
        if(is_valid()) {
            T temp = dest;
            if(_do_parse(temp, sel, parse_log)) {
                dest = std::move(temp);
                return true;
            }
        }
        return false;
    }

    /// @brief Tries to parse this argument's value into @p dest.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T>
    auto parse(T& dest, std::ostream& parse_log) const {
        return parse(dest, default_selector, parse_log);
    }

    /// @brief Tries to parse the following argument's value into @p dest.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T, identifier_t V>
    auto parse_next(T& dest, const selector<V> sel, std::ostream& parse_log)
      const {
        return next().parse(dest, sel, parse_log);
    }

    /// @brief Tries to parse the following argument's value into @p dest.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T>
    auto parse_next(T& dest, std::ostream& parse_log) const {
        return parse_next(dest, default_selector, parse_log);
    }

    auto missing_handler(std::ostream& errorlog) {
        return [&errorlog](const string_view arg_tag) {
            errorlog << "Missing value after '" << arg_tag << "'." << std::endl;
        };
    }

    auto invalid_handler(std::ostream& errorlog) {
        return [&errorlog](
                 const string_view arg_tag,
                 const string_view arg_val,
                 const string_view log_str) {
            errorlog << "Invalid value '" << arg_val << "' after '" << arg_tag
                     << "'. " << log_str << std::endl;
        };
    }

    template <typename T, typename MissingFunc, typename InvalidFunc>
    auto do_consume_next(
      T& dest,
      MissingFunc handle_missing,
      InvalidFunc handle_invalid) -> bool {
        if(next()) {
            std::stringstream parse_log;
            if(parse_next(dest, parse_log)) {
                *this = next();
                return true;
            } else {
                handle_invalid(
                  get(), next().get(), string_view(parse_log.str()));
            }
        } else {
            handle_missing(get());
        }
        return false;
    }

    /// @brief Tries to parse the following argument's value into @p dest.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T>
    auto consume_next(T& dest, std::ostream& errorlog) -> bool {
        auto if_missing = missing_handler(errorlog);
        auto if_invalid = invalid_handler(errorlog);
        return do_consume_next(dest, if_missing, if_invalid);
    }

    template <typename T, typename MissingFunc, typename InvalidFunc>
    auto do_consume_next(
      T& dest,
      const span<const T> choices,
      MissingFunc handle_missing,
      InvalidFunc handle_invalid) {
        valid_if_in_list<T, span<const T>> temp(T(), choices);
        if(do_consume_next(temp, handle_missing, handle_invalid)) {
            dest = temp.value();
            return true;
        }
        return false;
    }

    template <typename T, typename P, typename L, class MissingFunc, class InvalidFunc>
    auto do_consume_next(
      valid_if<T, P, L>& dest,
      const span<const T> choices,
      MissingFunc handle_missing,
      InvalidFunc handle_invalid) {
        T temp = {};
        if(do_consume_next(temp, choices, handle_missing, handle_invalid)) {
            if(dest.is_valid(temp)) {
                dest = std::move(temp);
                return true;
            }
        }
        return false;
    }

    template <typename T, typename C>
    auto consume_next(
      T& dest,
      const span<const C> choices,
      std::ostream& errorlog) -> bool {
        auto if_missing{missing_handler(errorlog)};
        auto if_invalid{invalid_handler(errorlog)};
        return do_consume_next(dest, choices, if_missing, if_invalid);
    }

    template <typename T, typename MissingFunc, typename InvalidFunc>
    auto do_consume_next(
      T& dest,
      const span<const string_view> symbols,
      const span<const T> translations,
      MissingFunc handle_missing,
      InvalidFunc handle_invalid) {
        EAGINE_ASSERT(symbols.size() <= translations.size());

        string_view parsed;
        if(do_consume_next(parsed, symbols, handle_missing, handle_invalid)) {
            for(const auto i : integer_range(symbols.size())) {
                if(are_equal(parsed, symbols[i])) {
                    dest = translations[i];
                    return true;
                }
            }
        }
        return false;
    }

    template <typename T, typename P, typename L, class MissingFunc, class InvalidFunc>
    auto do_consume_next(
      valid_if<T, P, L>& dest,
      const span<const string_view> symbols,
      const span<const T> translations,
      MissingFunc handle_missing,
      InvalidFunc handle_invalid) {
        T temp{};
        if(do_consume_next(
             temp, symbols, translations, handle_missing, handle_invalid)) {
            if(dest.is_valid(temp)) {
                dest = std::move(temp);
                return true;
            }
        }
        return false;
    }

    /// @brief Tries to parse values into starting from the following argument.
    /// @returns True if the parse is successful, false otherwise.
    template <typename T, typename R>
    auto consume_next(
      T& dest,
      const span<const string_view> symbols,
      const span<const R> translations,
      std::ostream& errorlog) -> bool {
        auto if_missing{missing_handler(errorlog)};
        auto if_invalid{invalid_handler(errorlog)};
        return do_consume_next(
          dest, symbols, translations, if_missing, if_invalid);
    }

    /// @brief Indicates if this argument's value is equal to the specified string.
    auto operator==(const value_type& v) const noexcept {
        return are_equal(get(), v);
    }

    /// @brief Indicates if this argument's value is different than the specified string.
    auto operator!=(const value_type& v) const noexcept {
        return !are_equal(get(), v);
    }

private:
    int _argi{0};
    int _argc{0};
    const char** _argv{nullptr};

    friend class program_arg_iterator;
    friend class program_args;

    template <typename T, identifier_t V>
    auto _do_parse(T& dest, const selector<V> sel, const std::ostream&)
      const noexcept -> bool {
        if(auto opt_val{from_string<T>(get(), sel)}) {
            dest = std::move(extract(opt_val));
            return true;
        }
        return false;
    }

    template <identifier_t V>
    auto _do_parse(string_view& dest, const selector<V>, const std::ostream&)
      const noexcept -> bool {
        dest = get();
        return true;
    }

    template <identifier_t V>
    auto _do_parse(std::string& dest, const selector<V>, const std::ostream&)
      const -> bool {
        dest = get_string();
        return true;
    }

    template <typename T, typename P, typename L, identifier_t V>
    auto _do_parse(
      valid_if<T, P, L>& dest,
      const selector<V> sel,
      std::ostream& parse_log) const -> bool {
        typename valid_if<T, P, L>::value_type value{};
        if(parse(value, sel, parse_log)) {
            if(dest.is_valid(value)) {
                dest = std::move(value);
                return true;
            } else {
                dest.log_invalid(parse_log, value);
            }
        } else {
            parse_log << "'" << get() << "' "
                      << "is not a valid `" << type_name<T>() << "` value";
        }
        return false;
    }

    template <typename T, typename A, identifier_t V>
    auto _do_parse(
      std::vector<T, A>& dest,
      const selector<V> sel,
      std::ostream& parse_log) const -> bool {
        T value{};
        if(parse(value, sel, parse_log)) {
            dest.push_back(std::move(value));
            return true;
        }
        return false;
    }
};
//------------------------------------------------------------------------------
template <>
struct extract_traits<program_arg> {
    using value_type = typename program_arg::value_type;
    using result_type = value_type;
    using const_result_type = value_type;
};

static inline auto extract(const program_arg& arg) noexcept {
    return arg.get();
}
//------------------------------------------------------------------------------
static inline auto to_string(const program_arg& arg) {
    return arg.get_string();
}
//------------------------------------------------------------------------------
static inline auto operator<<(std::ostream& out, const program_arg& arg)
  -> std::ostream& {
    return out << arg.get();
}
//------------------------------------------------------------------------------
/// @brief Iterator type over program_arg instances.
class program_arg_iterator {
    using this_class = program_arg_iterator;

public:
    constexpr program_arg_iterator(const program_arg arg) noexcept
      : _a{arg} {}

    /// @brief Alias for the referenced value type.
    using value_type = program_arg;

    /// @brief Alias for difference type.
    using difference_type = int;

    /// @brief Alias for reference type.
    using reference = program_arg&;

    /// @brief Alias for const reference type.
    using const_reference = const program_arg&;

    /// @brief Alias for pointer type.
    using pointer = program_arg*;

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
        return EAGINE_CONSTEXPR_ASSERT(l._argv == r._argv, l._argi - r._argi);
    }

    program_arg _a{};
};
//------------------------------------------------------------------------------
/// @brief Class wrapping the main function arguments, providing a convenient API.
/// @ingroup main_context
class program_args {
public:
    /// @brief Default constructor.
    /// @post empty()
    program_args() noexcept = default;

    /// @brief  Construction from the length and pointer to the argument list.
    program_args(const span_size_t argn, char** args) noexcept
      : _argc{int(argn)}
      , _argv{const_cast<const char**>(args)} {}

    /// @brief  Construction from the length and const pointer to the argument list.
    program_args(const span_size_t argn, const char** args) noexcept
      : _argc{int(argn)}
      , _argv{args} {}

    /// @brief Alias for the element value type.
    using value_type = string_view;

    /// @brief Alias for the element count type.
    using size_type = int;

    /// @brief Alias for valid index type.
    using valid_index = valid_range_index<program_args>;

    /// @brief Alias for iterator type.
    using iterator = program_arg_iterator;

    /// @brief Returns the number of arguments.
    auto argc() const noexcept -> int {
        return _argc;
    }

    /// @brief Returns a pointer to tha list of argument C-strings.
    auto argv() const noexcept -> const char** {
        return _argv;
    }

    /// @brief Indicates if the argument list is completely empty.
    auto empty() const noexcept -> bool {
        return _argc <= 0;
    }

    /// @brief Indicates if the argument list does not contain any arguments.
    auto none() const noexcept -> bool {
        return _argc <= 1;
    }

    /// @brief Returns the count of arguments (counting in the command name)
    auto size() const noexcept -> int {
        return _argc;
    }

    /// @brief Tests if the specified index references a command line argument.
    auto is_valid(const valid_index& pos) const noexcept -> bool {
        return pos.is_valid(*this) && (_argv != nullptr) &&
               (_argv[pos.value_or(-1, *this)] != nullptr);
    }

    /// @brief Returns the command line argument value at the specified position.
    /// @pre is_valid(pos)
    auto get(const valid_index& pos) const noexcept -> value_type {
        EAGINE_ASSERT(is_valid(pos));
        return value_type(_argv[pos.value_anyway()]);
    }

    /// @brief Returns the command line argument value at the specified position.
    /// @pre is_valid(pos)
    auto operator[](const valid_index& pos) const noexcept -> value_type {
        return get(pos);
    }

    /// @brief Returns the command name.
    auto command() const noexcept -> value_type {
        return get(0);
    }

    /// @brief Returns first argument.
    auto first() const noexcept -> program_arg {
        return {1, _argc, _argv};
    }

    /// @brief Returns an iterator to the first argument (not the command name).
    auto begin() const noexcept -> iterator {
        return {program_arg(1, _argc, _argv)};
    }

    /// @brief Returns an iterator past the last argument.
    auto end() const noexcept -> iterator {
        return {program_arg(_argc, _argc, _argv)};
    }

    /// @brief Finds and returns the argument with the specified value.
    auto find(const string_view what) const noexcept -> program_arg {
        int i = 1;
        while(i < _argc) {
            if((_argv != nullptr) && (_argv[i] != nullptr)) {
                if(are_equal(string_view(_argv[i]), what)) {
                    break;
                }
            }
            ++i;
        }
        return {i, _argc, _argv};
    }

private:
    const int _argc{0};
    const char** _argv{nullptr};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRAM_ARGS_HPP
