/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <utility>
module;

#include <cassert>

export module eagine.core.valid_if:decl;

import eagine.core.types;
import eagine.core.concepts;
import <sstream>;
import <stdexcept>;
import <type_traits>;
import <utility>;
export import <optional>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Policy for optionally valid values, indicated by a boolean flag.
/// @ingroup valid_if
export struct valid_flag_policy {
    bool _is_valid{false};

    constexpr valid_flag_policy() noexcept = default;

    constexpr valid_flag_policy(const bool is_valid) noexcept
      : _is_valid{is_valid} {}

    /// @brief Returns value validity depending on internally stored flag.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept -> bool {
        return _is_valid;
    }

    struct do_log {
        template <typename X>
        constexpr do_log(const X&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T&) const {
            log << "Getting the value of an empty optional";
        }
    };
};
//------------------------------------------------------------------------------
/// @brief Basic template for conditionally-valid values.
/// @ingroup valid_if
/// @tparam T type of the stored, conditionally valid value.
/// @tparam Policy indicates under what conditions is the stored value valid.
export template <typename T, typename Policy, typename DoLog, typename... P>
class basic_valid_if {
    static_assert(
      std::is_nothrow_default_constructible_v<Policy> or
      std::is_nothrow_move_constructible_v<Policy>);

public:
    using value_type = std::remove_cv_t<std::remove_reference_t<T>>;
    using reference = T&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /// @brief Returns a reference to this object's policy.
    [[nodiscard]] constexpr auto policy() noexcept -> Policy& {
        return _policy;
    }

    /// @brief Returns a const reference to this object's policy.
    [[nodiscard]] constexpr auto policy() const noexcept -> const Policy& {
        return _policy;
    }

    /// @brief Default constructor.
    constexpr basic_valid_if() noexcept(
      std::is_nothrow_default_constructible_v<T>) = default;

    /// @brief Constructor initializing the stored value by @p val.
    constexpr basic_valid_if(T val) noexcept(
      std::is_nothrow_move_constructible_v<T>)
      : _value{std::move(val)} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T val, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<value_type>&&
                 std::is_nothrow_move_constructible_v<Policy>))
      : _value{std::move(val)}
      , _policy{std::move(plcy)} {}

    /// @brief Move constructor.
    constexpr basic_valid_if(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_constructible_v<value_type>&&
        std::is_nothrow_move_constructible_v<Policy>)
      : _value{std::move(that._value)}
      , _policy{std::move(that._policy)} {}

    /// @brief Copy constructor.
    constexpr basic_valid_if(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<T>&&
        std::is_nothrow_copy_constructible_v<Policy>)
      : _value{that._value}
      , _policy{that._policy} {}

    /// @brief Move assignment operator.
    constexpr auto operator=(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_assignable_v<value_type>&&
        std::is_nothrow_move_assignable_v<Policy>) -> auto& {
        if(this != std::addressof(that)) {
            _value = std::move(that._value);
            _policy = std::move(that._policy);
            _do_log = DoLog(_policy);
        }
        return *this;
    }

    /// @brief Copy assignment operator.
    // NOLINTNEXTLINE(bugprone-unhandled-self-assignment,cert-oop54-cpp)
    constexpr auto operator=(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<value_type>&&
        std::is_nothrow_copy_constructible_v<Policy>) -> auto& {
        if(this != std::addressof(that)) {
            basic_valid_if temp{that};
            *this = std::move(temp);
        }
        return *this;
    }

    /// @brief Copies @p value into this instance.
    constexpr auto operator=(const value_type& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = value;
        return *this;
    }

    constexpr auto operator=(value_type&& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = std::move(value);
        return *this;
    }

    /// @brief The destructor.
    ~basic_valid_if() noexcept = default;

    /// @brief Checks if @p val is valid according to this object's policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto is_valid(const_reference val, P... p)
      const noexcept -> bool {
        return _policy(val, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto is_valid(P... p) const noexcept -> bool {
        return _policy(_value, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto has_value(P... p) const noexcept {
        return is_valid(p...);
    }

    /// @brief Indicates if the stored value is valid according to policy.
    /// @see is_valid
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Equality comparison.
    [[nodiscard]] constexpr auto operator==(
      const basic_valid_if& that) const noexcept -> bool {
        return (_value == that._value) and is_valid() and that.is_valid();
    }

    /// @brief Equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator==(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() == v, not is_valid()};
    }

    /// @brief Non-equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator!=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() != v, not is_valid()};
    }

    /// @brief Less-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() < v, not is_valid()};
    }

    /// @brief Greater-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() > v, not is_valid()};
    }

    /// @brief Less-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() <= v, not is_valid()};
    }

    /// @brief Greater-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() >= v, not is_valid()};
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, const value_type& v, P... p) const {
        assert(not is_valid(v, p...));
        _do_log(log, v, p...);
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, P... p) const {
        log_invalid(log, _value, p...);
    }

    /// @brief Calls the specified function if the stored value is invalid.
    /// @param function the function to be called.
    /// @param p additional parameters for the policy validity check function.
    template <typename F>
    constexpr auto or_else(F&& function, P... p) -> auto& {
        if(not is_valid(p...)) {
            std::forward<F>(function)(_do_log, _value, p...);
        }
        return *this;
    }

    /// @brief Throws an exception if the stored value is invalid.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    void throw_if_invalid(P... p) const {
        if(not is_valid(p...)) [[unlikely]] {
            std::stringstream ss;
            log_invalid(ss, p...);
            throw std::runtime_error(ss.str());
        }
    }

    /// @brief Returns the stored value if it is valid otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    [[nodiscard]] auto value(P... p) -> reference {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if it is valid, otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    [[nodiscard]] auto value(P... p) const -> const_reference {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto value_or(reference fallback, P... p) noexcept
      -> reference {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto value_or(const_reference fallback, P... p)
      const noexcept -> const_reference {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() const noexcept -> const T& {
        return _value;
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() noexcept -> T& {
        return _value;
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C>
    [[nodiscard]] auto construct() noexcept(noexcept(T(std::declval<T&>())))
      -> basic_valid_if<C, valid_flag_policy, typename valid_flag_policy::do_log> {
        if(has_value()) {
            return {C{this->value_anyway()}, true};
        }
        return {};
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see construct
    /// @see transform
    template <typename F>
        requires(optional_like<std::remove_cvref_t<std::invoke_result_t<F, T&>>>)
    [[nodiscard]] auto and_then(F&& function) {
        using R = std::remove_cvref_t<std::invoke_result_t<F, T&>>;
        if(has_value()) {
            return std::invoke(std::forward<F>(function), this->value_anyway());
        } else {
            return R{};
        }
    }

    /// @brief Calls the specified function if the stored value is valid.
    /// @param function the function to be called.
    /// @see and_then
    template <typename F>
        requires(not std::is_same_v<std::invoke_result_t<F, T>, void>)
    [[nodiscard]] constexpr auto transform(F&& function) const {
        using R = std::invoke_result_t<F, T&>;
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using U = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<U>{
                  std::invoke(std::forward<F>(function), this->value_anyway())};
            } else {
                return optional_reference<U>{nothing};
            }
        } else {
            using V = std::remove_cvref_t<R>;
            if(has_value()) {
                return basic_valid_if<
                  V,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{
                  std::invoke(std::forward<F>(function), this->value_anyway()),
                  true};
            } else {
                return basic_valid_if<
                  V,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{};
            }
        }
    }

    /// @brief Calls the specified function if the stored valus is valid.
    /// @param function the function to call.
    /// @see transform
    template <typename F>
    [[nodiscard]] constexpr auto operator|(const F& function) const {
        return transform(function);
    }

    template <typename M, std::same_as<T> C>
    [[nodiscard]] auto member(M C::*ptr) noexcept {
        if(has_value()) {
            return optional_reference<M>{this->value_anyway().*ptr};
        } else {
            return optional_reference<M>{nothing};
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] auto member(M C::*ptr) const noexcept {
        if(has_value()) {
            return optional_reference<std::add_const_t<M>>{
              this->value_anyway().*ptr};
        } else {
            return optional_reference<std::add_const_t<M>>{nothing};
        }
    }

    /// @brief Calls a binary transforming function on {value, is_valid()} pair.
    /// @param function the function to be called.
    template <typename F>
    [[nodiscard]] constexpr auto transformed(F function, P... p) const noexcept
      -> basic_valid_if<
        std::invoke_result_t<F, T, bool>,
        valid_flag_policy,
        typename valid_flag_policy::do_log> {
        const auto v{is_valid(p...)};
        auto r{function(_value, v)};
        if constexpr(extractable<decltype(r)>) {
            return r;
        } else {
            return {std::move(r), v};
        }
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    [[nodiscard]] constexpr auto operator*() const -> const_reference {
        return value();
    }

    /// @brief Returns pointer to the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    [[nodiscard]] constexpr auto operator->() const -> const_pointer {
        return &value();
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<T>() const
      noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if(has_value()) {
            return {this->value_anyway()};
        }
        return {};
    }

private:
    T _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename DoLog, typename... P>
class basic_valid_if<T&, Policy, DoLog, P...> {
    static_assert(
      std::is_nothrow_default_constructible_v<Policy> or
      std::is_nothrow_move_constructible_v<Policy>);

public:
    using value_type = std::remove_cv_t<T>;
    using reference = T&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /// @brief Returns a reference to this object's policy.
    [[nodiscard]] constexpr auto policy() noexcept -> Policy& {
        return _policy;
    }

    /// @brief Returns a const reference to this object's policy.
    [[nodiscard]] constexpr auto policy() const noexcept -> const Policy& {
        return _policy;
    }

    /// @brief Constructor initializing the stored value by @p val.
    constexpr basic_valid_if(T& ref) noexcept
      : _value{ref} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T& ref, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<Policy>))
      : _value{ref}
      , _policy{std::move(plcy)} {}

    /// @brief Move constructor.
    constexpr basic_valid_if(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_constructible_v<value_type>&&
        std::is_nothrow_move_constructible_v<Policy>)
      : _value{that._value}
      , _policy{std::move(that._policy)} {}

    /// @brief Copy constructor.
    constexpr basic_valid_if(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<T>&&
        std::is_nothrow_copy_constructible_v<Policy>)
      : _value{that._value}
      , _policy{that._policy} {}

    /// @brief Move assignment operator.
    auto operator=(basic_valid_if&&) = delete;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const basic_valid_if&) = delete;

    /// @brief Copies @p value into this instance.
    constexpr auto operator=(const value_type& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = value;
        return *this;
    }

    constexpr auto operator=(value_type&& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = std::move(value);
        return *this;
    }

    /// @brief The destructor.
    ~basic_valid_if() noexcept = default;

    /// @brief Checks if @p val is valid according to this object's policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto is_valid(const_reference val, P... p)
      const noexcept -> bool {
        return _policy(val, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto is_valid(P... p) const noexcept -> bool {
        return _policy(_value, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto has_value(P... p) const noexcept {
        return is_valid(p...);
    }

    /// @brief Indicates if the stored value is valid according to policy.
    /// @see is_valid
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Equality comparison.
    [[nodiscard]] constexpr auto operator==(
      const basic_valid_if& that) const noexcept -> bool {
        return (_value == that._value) and is_valid() and that.is_valid();
    }

    /// @brief Equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator==(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() == v, not is_valid()};
    }

    /// @brief Non-equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator!=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() != v, not is_valid()};
    }

    /// @brief Less-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() < v, not is_valid()};
    }

    /// @brief Greater-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() > v, not is_valid()};
    }

    /// @brief Less-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() <= v, not is_valid()};
    }

    /// @brief Greater-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>=(const value_type& v) const noexcept
      -> tribool {
        return {this->value_anyway() >= v, not is_valid()};
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, const value_type& v, P... p) const {
        assert(not is_valid(v, p...));
        _do_log(log, v, p...);
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, P... p) const {
        log_invalid(log, _value, p...);
    }

    /// @brief Calls the specified function if the stored value is invalid.
    /// @param function the function to be called.
    /// @param p additional parameters for the policy validity check function.
    template <typename F>
    constexpr auto or_else(F&& function, P... p) -> auto& {
        if(not is_valid(p...)) {
            std::forward<F>(function)(_do_log, _value, p...);
        }
        return *this;
    }

    /// @brief Throws an exception if the stored value is invalid.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    void throw_if_invalid(P... p) const {
        if(not is_valid(p...)) {
            std::stringstream ss;
            log_invalid(ss, p...);
            throw std::runtime_error(ss.str());
        }
    }

    /// @brief Returns the stored value if it is valid otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    [[nodiscard]] auto value(P... p) -> T& {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if it is valid, otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    [[nodiscard]] auto value(P... p) const -> const T& {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto value_or(reference fallback, P... p) noexcept
      -> auto& {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto value_or(const_reference fallback, P... p)
      const noexcept -> auto& {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() const noexcept -> const T& {
        return _value;
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() noexcept -> T& {
        return _value;
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C>
    [[nodiscard]] auto construct() const noexcept(
      noexcept(T(std::declval<const T&>())))
      -> basic_valid_if<C, valid_flag_policy, typename valid_flag_policy::do_log> {
        if(has_value()) {
            return {C{this->value_anyway()}};
        }
        return {};
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see transform
    template <typename F>
        requires(optional_like<std::remove_cvref_t<std::invoke_result_t<F, T&>>>)
    [[nodiscard]] auto and_then(F&& function) const {
        using R = std::remove_cvref_t<std::invoke_result_t<F, T&>>;
        if(has_value()) {
            return std::invoke(std::forward<F>(function), this->value());
        } else {
            return R{};
        }
    }

    /// @brief Calls the specified function if the stored value is valid.
    /// @param function the function to be called.
    template <typename F>
        requires(not std::is_same_v<std::invoke_result_t<F, T>, void>)
    [[nodiscard]] constexpr auto transform(F&& function) const {
        using R = std::invoke_result_t<F, T&>;
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using U = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<U>{
                  std::invoke(std::forward<F>(function), this->value_anyway())};
            } else {
                return optional_reference<U>{nothing};
            }
        } else {
            using V = std::remove_cvref_t<R>;
            if(has_value()) {
                return basic_valid_if<
                  V,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{
                  std::invoke(std::forward<F>(function), this->value_anyway()),
                  true};
            } else {
                return basic_valid_if<
                  V,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{};
            }
        }
    }

    /// @brief Calls the specified function if the stored valus is valid.
    /// @param function the function to call.
    /// @see transform
    template <typename F>
    [[nodiscard]] constexpr auto operator|(const F& function) const {
        return transform(function);
    }

    template <typename M, std::same_as<T> C>
    [[nodiscard]] auto member(M C::*ptr) noexcept {
        if(has_value()) {
            return optional_reference<M>{this->value_anyway().*ptr};
        } else {
            return optional_reference<M>{nothing};
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] auto member(M C::*ptr) const noexcept {
        if(has_value()) {
            return optional_reference<std::add_const_t<M>>{
              this->value_anyway().*ptr};
        } else {
            return optional_reference<std::add_const_t<M>>{nothing};
        }
    }

    /// @brief Calls a binary transforming function on {value, is_valid()} pair.
    /// @param function the function to be called.
    template <typename F>
    [[nodiscard]] constexpr auto transformed(F function, P... p) const noexcept
      -> basic_valid_if<
        std::invoke_result_t<F, T, bool>,
        valid_flag_policy,
        typename valid_flag_policy::do_log> {
        const auto v{is_valid(p...)};
        auto r{function(_value, v)};
        if constexpr(extractable<decltype(r)>) {
            return r;
        } else {
            return {std::move(r), v};
        }
    }

    /// @brief Returns the stored value if valid, returns fallback otherwise.
    /// @see basic_valid_if::value_or
    [[nodiscard]] constexpr auto operator/(
      const_reference fallback) const noexcept -> const_reference {
        return value_or(fallback);
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    [[nodiscard]] constexpr auto operator*() const -> const_reference {
        return value();
    }

    /// @brief Returns pointer to the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    [[nodiscard]] constexpr auto operator->() const -> const_pointer {
        return &value();
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<std::reference_wrapper<T>>()
      const noexcept {
        if(has_value()) {
            return {this->value_anyway()};
        }
        return {};
    }

private:
    T& _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
export template <typename T, typename P, typename L, typename... A>
[[nodiscard]] constexpr auto has_value(
  const basic_valid_if<T, P, L>& v,
  A&&... a) noexcept -> bool {
    return v.has_value(std::forward<A>(a)...);
}

/// @brief Equality comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator==(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() == v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}

/// @brief Non-equality comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator!=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() != v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}

/// @brief Less-than comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator<(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() < v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}

/// @brief Greater-than comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator>(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() > v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}

/// @brief Less-equal comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator<=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() <= v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}

/// @brief Greater-equal comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
[[nodiscard]] constexpr auto operator>=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() >= v2.value_anyway()),
      (not v1.is_valid() or not v2.is_valid())};
}
//------------------------------------------------------------------------------
/// @brief Primary template for conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Policy>
using valid_if = basic_valid_if<T, Policy, typename Policy::do_log>;
//------------------------------------------------------------------------------
export template <typename T, typename P, typename L>
struct extract_traits<basic_valid_if<T, P, L>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
[[nodiscard]] constexpr auto extract(const basic_valid_if<T, P, L>& opt) noexcept
  -> const T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
[[nodiscard]] constexpr auto extract(basic_valid_if<T, P, L>& opt) noexcept
  -> T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
[[nodiscard]] constexpr auto extract(basic_valid_if<T, P, L>&& opt) noexcept
  -> T&& {
    assert(opt);
    return std::move(opt.value_anyway());
}
//------------------------------------------------------------------------------
/// @brief Helper class storing both conditionally valid value and fallback value.
/// @ingroup valid_if
export template <typename F, typename T, typename P, typename L>
class valid_if_or_fallback : public basic_valid_if<T, P, L> {
public:
    /// @brief Constructor
    constexpr valid_if_or_fallback(basic_valid_if<T, P, L> vi, F fallback) noexcept(
      noexcept(basic_valid_if<T, P, L>(
        std::declval<
          basic_valid_if<T, P, L>&&>())) and noexcept(F(std::declval<F&&>())))
      : basic_valid_if<T, P, L>{std::move(vi)}
      , _fallback{std::move(fallback)} {}

    /// @brief Returns the stored fallback value.
    [[nodiscard]] constexpr auto fallback() const noexcept -> const F& {
        return _fallback;
    }

    /// @brief Returns the stored fallback value.
    [[nodiscard]] constexpr auto fallback() noexcept -> F& {
        return _fallback;
    }

private:
    F _fallback{};
};
//------------------------------------------------------------------------------
/// @brief Constructor function for valid_if_or_fallback.
/// @ingroup valid_if
export template <typename F, typename T, typename P, typename L>
[[nodiscard]] auto either_or(basic_valid_if<T, P, L> vi, F f) noexcept(
  noexcept(basic_valid_if<T, P, L>(
    std::declval<
      basic_valid_if<T, P, L>&&>())) and noexcept(F(std::declval<F&&>())))
  -> valid_if_or_fallback<F, T, P, L> {
    return {std::move(vi), std::move(f)};
}
//------------------------------------------------------------------------------
export template <typename F, typename T, typename P, typename L>
[[nodiscard]] auto operator<<(
  std::ostream& out,
  const valid_if_or_fallback<F, T, P, L>& viof) -> std::ostream& {
    if(viof.has_value()) {
        out << viof.value();
    } else {
        out << viof.fallback();
    }
    return out;
}
//------------------------------------------------------------------------------
/// @brief Specialization of valid_if with flag indicating validity.
/// @ingroup valid_if
/// @see valid_if_indicated
export template <typename T>
using optionally_valid = valid_if<T, valid_flag_policy>;
//------------------------------------------------------------------------------
export template <typename Dst, typename P, typename L, typename... A, typename Src>
struct within_limits<basic_valid_if<Dst, P, L, A...>, Src> {
    static constexpr auto check(const Src value) noexcept {
        return within_limits<Dst, Src>::check(value);
    }
};
//------------------------------------------------------------------------------
/// @brief Policy for always valid values.
/// @ingroup valid_if
export struct always_valid_policy {

    /// @brief Indicates value validity. Always returns true.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept {
        return true;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log&, const T&) const {}
    };
};

/// @brief Specialization of valid_if, for always valid values.
/// @ingroup valid_if
/// @see never_valid
export template <typename T>
using always_valid = valid_if<T, always_valid_policy>;
//------------------------------------------------------------------------------
/// @brief Policy for never-valid values.
/// @ingroup valid_if
export struct never_valid_policy {

    /// @brief Indicates value validity. Always returns false.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept {
        return false;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T&) const {
            log << "value is not valid";
        }
    };
};

/// @brief Specialization of valid_if, for never-valid values.
/// @ingroup valid_if
/// @see always_valid
export template <typename T>
using never_valid = valid_if<T, never_valid_policy>;
//------------------------------------------------------------------------------
/// @brief Policy class for containers valid if their empty() member
/// function return false.
/// @ingroup valid_if
export template <typename T>
struct valid_if_not_empty_policy {

    /// @brief Indicates value validity, true if not range.empty().
    constexpr auto operator()(const T& range) const noexcept {
        return not range.empty();
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T&) const {
            log << "Empty range, string or container is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if not empty.
/// @ingroup valid_if
/// @see valid_if_size_gt
/// @see valid_if_lt_size
export template <typename T>
using valid_if_not_empty =
  valid_if<T, valid_if_not_empty_policy<std::remove_reference_t<T>>>;
//------------------------------------------------------------------------------
/// @brief Policy for values valid when non-boolean Indicator has Value.
/// @ingroup valid_if
export template <typename Indicator, typename Comparable, Comparable Value>
struct valid_if_indicated_policy {
    Indicator _indicator{};

    constexpr valid_if_indicated_policy() noexcept = default;

    constexpr valid_if_indicated_policy(Indicator indicator) noexcept
      : _indicator{std::move(indicator)} {}

    /// @brief Indicates value validity, true if indicator == Value.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept {
        return Comparable(_indicator) == Value;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T&) const {
            log << "indicator is " << Value;
        }
    };
};

/// @brief Specialization of valid_if, for values with non-boolean indicator.
/// @ingroup valid_if
/// @see optionally_valid
///
/// This is a more generalized for of optionally_valid for indicators of types
/// other than boolean.
export template <
  typename T,
  typename Indicator,
  typename Comparable = bool,
  Comparable Value = true>
using valid_if_indicated =
  valid_if<T, valid_if_indicated_policy<Indicator, Comparable, Value>>;
//------------------------------------------------------------------------------
} // namespace eagine
