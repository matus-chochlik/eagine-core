/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.valid_if:decl;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Policy for optionally valid values, indicated by a boolean flag.
/// @ingroup valid_if
export struct valid_flag_policy {
    bool _has_value{false};

    constexpr valid_flag_policy() noexcept = default;

    constexpr valid_flag_policy(const bool has_value) noexcept
      : _has_value{has_value} {}

    constexpr valid_flag_policy(const tribool has_value) noexcept
      : _has_value{bool(has_value)} {}

    /// @brief Returns value validity depending on internally stored flag.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept -> bool {
        return _has_value;
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
export template <typename T, typename Policy, typename DoLog>
class basic_valid_if {
    static_assert(
      std::is_nothrow_default_constructible_v<Policy> or
      std::is_nothrow_move_constructible_v<Policy>);

    static constexpr auto _policy_with_value(bool has_val) noexcept {
        if constexpr(std::same_as<Policy, valid_flag_policy>) {
            return Policy{has_val};
        } else {
            return Policy{};
        }
    }

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
      : _value{std::move(val)}
      , _policy{_policy_with_value(true)} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T val, Policy plcy) noexcept(noexcept(
      std::is_nothrow_move_constructible_v<value_type> &&
      std::is_nothrow_move_constructible_v<Policy>))
      : _value{std::move(val)}
      , _policy{std::move(plcy)} {}

    template <std::convertible_to<T> U>
        requires(std::same_as<Policy, valid_flag_policy>)
    constexpr basic_valid_if(std::optional<U> opt) noexcept(
      std::is_nothrow_move_constructible_v<T>)
      : _value{opt.has_value() ? *opt : T{}}
      , _policy{_policy_with_value(opt.has_value())} {}

    /// @brief Move constructor.
    constexpr basic_valid_if(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_constructible_v<value_type> &&
      std::is_nothrow_move_constructible_v<Policy>)
      : _value{std::move(that._value)}
      , _policy{std::move(that._policy)} {}

    /// @brief Copy constructor.
    constexpr basic_valid_if(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<T> &&
      std::is_nothrow_copy_constructible_v<Policy>)
      : _value{that._value}
      , _policy{that._policy} {}

    /// @brief Move assignment operator.
    constexpr auto operator=(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_assignable_v<value_type> &&
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
      std::is_nothrow_copy_constructible_v<value_type> &&
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
    [[nodiscard]] constexpr auto has_value(const value_type& val) const noexcept
      -> bool {
        return _policy(val);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    [[nodiscard]] constexpr auto has_value() const noexcept {
        return _policy(_value);
    }

    /// @brief Indicates if the stored value is valid according to policy.
    /// @see has_value
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return _policy(_value);
    }

    /// @brief Equality comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator==(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() == that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator==(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() == v, has_value()};
    }

    /// @brief Non-equality comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator!=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() != that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Non-equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator!=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() != v, has_value()};
    }

    /// @brief Less-than comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator<(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() < that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Less-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() < v, has_value()};
    }

    /// @brief Greater-than comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator>(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() > that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Greater-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() > v, has_value()};
    }

    /// @brief Less-than or equal comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator<=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() <= that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Less-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() <= v, has_value()};
    }

    /// @brief Greater-than or equal comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator>=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() >= that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Greater-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() >= v, has_value()};
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, const value_type& v) const {
        assert(not has_value(v));
        _do_log(log, v);
    }

    template <typename Log>
    constexpr void log_invalid(Log& log) const {
        log_invalid(log, _value);
    }

    /// @brief Throws an exception if the stored value is invalid.
    /// @throws std::runtime_error
    void throw_if_invalid() const {
        if(not has_value(_value)) [[unlikely]] {
            std::stringstream ss;
            log_invalid(ss);
            throw std::runtime_error(ss.str());
        }
    }

    [[nodiscard]] auto value() & -> T& {
        throw_if_invalid();
        return _value;
    }

    [[nodiscard]] auto value() && -> T&& {
        throw_if_invalid();
        return std::move(_value);
    }

    /// @brief Returns the stored value if it is valid, otherwise throws.
    /// @throws std::runtime_error
    [[nodiscard]] auto value() const& -> const T& {
        throw_if_invalid();
        return _value;
    }

    [[nodiscard]] constexpr auto value_or(reference fallback) noexcept
      -> reference {
        if(has_value(_value)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    [[nodiscard]] constexpr auto value_or(
      const value_type& fallback) const noexcept -> const_reference {
        if(has_value(_value)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns default value.
    [[nodiscard]] constexpr auto or_default() const noexcept -> value_type {
        if(has_value(_value)) [[likely]] {
            return _value;
        }
        return value_type{};
    }

    [[nodiscard]] constexpr auto value_anyway() & noexcept -> T& {
        return _value;
    }

    [[nodiscard]] constexpr auto value_anyway() && noexcept -> T&& {
        return std::move(_value);
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() const& noexcept -> const T& {
        return _value;
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C, typename... Args>
    [[nodiscard]] auto construct(Args&&... args) noexcept(
      noexcept(T(std::declval<T&>())))
      -> basic_valid_if<C, valid_flag_policy, typename valid_flag_policy::do_log> {
        if(has_value()) {
            return {C{value_anyway(), std::forward<Args>(args)...}, true};
        }
        return {};
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
    auto and_then(F&& function) & noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), value_anyway());
        } else {
            return R{};
        }
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, T&&>>>
    auto and_then(F&& function) && noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&&>()))) -> R {
        if(has_value()) {
            return std::invoke(
              std::forward<F>(function), std::move(value_anyway()));
        } else {
            return R{};
        }
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see construct
    /// @see transform
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, const T&>>>
    auto and_then(F&& function) const& noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<const T&>()))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), value_anyway());
        } else {
            return R{};
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
        requires(std::is_void_v<R>)
    void and_then(F&& function) & noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), value_anyway());
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, T&&>>>
        requires(std::is_void_v<R>)
    void and_then(F&& function) && noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), std::move(value_anyway()));
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, const T&>>>
        requires(std::is_void_v<R>)
    auto and_then(F&& function) const& noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<const T&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), value_anyway());
        }
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto or_else(F&& function) & noexcept(
      noexcept(std::invoke(std::forward<F>(function)))) -> R {
        if(has_value()) {
            return R{value_anyway()};
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto or_else(F&& function) && noexcept(
      noexcept(std::invoke(std::forward<F>(function)))) -> R {
        if(has_value()) {
            return R{std::move(value_anyway())};
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    /// @brief Return the stored value or the result or function.
    /// @see and_then
    /// @see transform
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto or_else(F&& function) const& noexcept(
      noexcept(std::invoke(std::forward<F>(function)))) -> R {
        if(has_value()) {
            return R{value_anyway()};
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    template <typename F, typename R = std::invoke_result_t<F, T&&>>
        requires(not std::is_same_v<R, void>)
    [[nodiscard]] constexpr auto transform(F&& function) && noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&&>()) and
      std::is_nothrow_move_constructible_v<R>)) {
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using U = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<U>{std::invoke(
                  std::forward<F>(function), std::move(value_anyway()))};
            } else {
                return optional_reference<U>{nothing};
            }
        } else if constexpr(std::is_same_v<R, bool>) {
            if(has_value()) {
                return tribool{
                  std::invoke(
                    std::forward<F>(function), std::move(value_anyway())),
                  true};
            } else {
                return tribool{indeterminate};
            }
        } else {
            if(has_value()) {
                return basic_valid_if<
                  R,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{
                  std::invoke(
                    std::forward<F>(function), std::move(value_anyway())),
                  true};
            } else {
                return basic_valid_if<
                  R,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{};
            }
        }
    }

    /// @brief Calls the specified function if the stored value is valid.
    /// @param function the function to be called.
    /// @see and_then
    template <typename F, typename R = std::invoke_result_t<F, const T&>>
        requires(not std::is_same_v<R, void>)
    [[nodiscard]] constexpr auto transform(F&& function) const& noexcept(
      noexcept(
        std::invoke(std::forward<F>(function), std::declval<const T&>()) and
        std::is_nothrow_move_constructible_v<R>)) {
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using U = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<U>{
                  std::invoke(std::forward<F>(function), value_anyway())};
            } else {
                return optional_reference<U>{nothing};
            }
        } else if constexpr(std::is_same_v<R, bool>) {
            if(has_value()) {
                return tribool{
                  std::invoke(std::forward<F>(function), value_anyway()), true};
            } else {
                return tribool{indeterminate};
            }
        } else {
            if(has_value()) {
                return basic_valid_if<
                  R,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{
                  std::invoke(std::forward<F>(function), value_anyway()), true};
            } else {
                return basic_valid_if<
                  R,
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
            return optional_reference<M>{value_anyway().*ptr};
        } else {
            return optional_reference<M>{nothing};
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] auto member(M C::*ptr) const noexcept {
        if(has_value()) {
            return optional_reference<std::add_const_t<M>>{value_anyway().*ptr};
        } else {
            return optional_reference<std::add_const_t<M>>{nothing};
        }
    }

    /// @brief Calls a binary transforming function on {value, has_value()} pair.
    /// @param function the function to be called.
    template <typename F, typename R = std::invoke_result_t<F, const T&, bool>>
    [[nodiscard]] constexpr auto transformed(F function) const noexcept
      -> basic_valid_if<R, valid_flag_policy, typename valid_flag_policy::do_log> {
        const auto has_val{has_value(_value)};
        auto r{function(_value, has_val)};
        if constexpr(optional_like<R>) {
            return r;
        } else {
            return {std::move(r), has_val};
        }
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    [[nodiscard]] constexpr auto operator*() const noexcept -> const_reference {
        assert(has_value());
        return value_anyway();
    }

    /// @brief Returns pointer to the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    [[nodiscard]] constexpr auto operator->() const noexcept -> const_pointer {
        assert(has_value());
        return &value_anyway();
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<T>() const
      noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if(has_value()) {
            return {value_anyway()};
        }
        return {};
    }

private:
    T _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename DoLog>
class basic_valid_if<T&, Policy, DoLog> {
    static_assert(
      std::is_nothrow_default_constructible_v<Policy> or
      std::is_nothrow_move_constructible_v<Policy>);

    static constexpr auto _policy_with_value(bool has_val) noexcept {
        if constexpr(std::same_as<Policy, valid_flag_policy>) {
            return Policy{has_val};
        } else {
            return Policy{};
        }
    }

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
      : _value{ref}
      , _policy{_policy_with_value(true)} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T& ref, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<Policy>))
      : _value{ref}
      , _policy{std::move(plcy)} {}

    /// @brief Move constructor.
    constexpr basic_valid_if(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_constructible_v<value_type> &&
      std::is_nothrow_move_constructible_v<Policy>)
      : _value{that._value}
      , _policy{std::move(that._policy)} {}

    /// @brief Copy constructor.
    constexpr basic_valid_if(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<T> &&
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
    [[nodiscard]] constexpr auto has_value(const value_type& val) const noexcept
      -> bool {
        return _policy(val);
    }

    /// @brief Checks if the stored value is valid according to policy.
    [[nodiscard]] constexpr auto has_value() const noexcept {
        return _policy(_value);
    }

    /// @brief Indicates if the stored value is valid according to policy.
    /// @see has_value
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return _policy(_value);
    }

    /// @brief Equality comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator==(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() == that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator==(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() == v, has_value()};
    }

    /// @brief Non-equality comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator!=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() != that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Non-equality comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator!=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() != v, has_value()};
    }

    /// @brief Less-than comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator<(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() < that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Less-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() < v, has_value()};
    }

    /// @brief Greater-than comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator>(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() > that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Greater-than comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() > v, has_value()};
    }

    /// @brief Less-than or equal comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator<=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() <= that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Less-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator<=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() <= v, has_value()};
    }

    /// @brief Greater-than or equal comparison.
    template <typename U, typename Po2, typename Lo2>
    [[nodiscard]] constexpr auto operator>=(
      const basic_valid_if<U, Po2, Lo2>& that) const noexcept -> tribool {
        return {
          value_anyway() >= that.value_anyway(),
          has_value() and that.has_value()};
    }

    /// @brief Greater-equal comparison of the stored value with @p v.
    [[nodiscard]] constexpr auto operator>=(const value_type& v) const noexcept
      -> tribool {
        return {value_anyway() >= v, has_value()};
    }

    template <typename Log>
    constexpr void log_invalid(Log& log, const value_type& v) const {
        assert(not has_value(v));
        _do_log(log, v);
    }

    template <typename Log>
    constexpr void log_invalid(Log& log) const {
        log_invalid(log, _value);
    }

    /// @brief Throws an exception if the stored value is invalid.
    /// @throws std::runtime_error
    void throw_if_invalid() const {
        if(not has_value(_value)) {
            std::stringstream ss;
            log_invalid(ss);
            throw std::runtime_error(ss.str());
        }
    }

    /// @brief Returns the stored value if it is valid otherwise throws.
    /// @throws std::runtime_error
    [[nodiscard]] auto value() -> T& {
        throw_if_invalid();
        return _value;
    }

    /// @brief Returns the stored value if it is valid, otherwise throws.
    /// @throws std::runtime_error
    [[nodiscard]] auto value() const -> const T& {
        throw_if_invalid();
        return _value;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    [[nodiscard]] constexpr auto value_or(reference fallback) noexcept
      -> auto& {
        if(has_value(_value)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    [[nodiscard]] constexpr auto value_or(
      const value_type& fallback) const noexcept -> auto& {
        if(has_value(_value)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() const noexcept -> T& {
        return _value;
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C, typename... Args>
    [[nodiscard]] auto construct(Args&&... args) const noexcept(
      noexcept(T(std::declval<const T&>())))
      -> basic_valid_if<C, valid_flag_policy, typename valid_flag_policy::do_log> {
        if(has_value()) {
            return {C{value_anyway(), std::forward<Args>(args)...}, true};
        }
        return {};
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
    auto and_then(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), value_anyway());
        } else {
            return R{};
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
    void and_then(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), value_anyway());
        }
    }

    /// @brief Return the stored value or the result or function.
    /// @see and_then
    /// @see transform
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto or_else(F&& function) const
      noexcept(noexcept(std::invoke(std::forward<F>(function)))) -> R {
        if(has_value()) {
            return R{value_anyway()};
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    /// @brief Calls the specified function if the stored value is valid.
    /// @param function the function to be called.
    /// @see and_then
    /// @see or_else
    template <typename F, typename R = std::invoke_result_t<F, const T&>>
        requires(not std::is_same_v<std::invoke_result_t<F, const T&>, void>)
    [[nodiscard]] constexpr auto transform(F&& function) const
      noexcept(noexcept(
        std::invoke(std::forward<F>(function), std::declval<const T&>()) and
        std::is_nothrow_move_constructible_v<R>)) {
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using U = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<U>{
                  std::invoke(std::forward<F>(function), value_anyway())};
            } else {
                return optional_reference<U>{nothing};
            }
        } else if constexpr(std::is_same_v<R, bool>) {
            if(has_value()) {
                return tribool{
                  std::invoke(std::forward<F>(function), value_anyway()), true};
            } else {
                return tribool{indeterminate};
            }
        } else {
            if(has_value()) {
                return basic_valid_if<
                  R,
                  valid_flag_policy,
                  typename valid_flag_policy::do_log>{
                  std::invoke(std::forward<F>(function), value_anyway()), true};
            } else {
                return basic_valid_if<
                  R,
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
            return optional_reference<M>{value_anyway().*ptr};
        } else {
            return optional_reference<M>{nothing};
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] auto member(M C::*ptr) const noexcept {
        if(has_value()) {
            return optional_reference<std::add_const_t<M>>{value_anyway().*ptr};
        } else {
            return optional_reference<std::add_const_t<M>>{nothing};
        }
    }

    /// @brief Calls a binary transforming function on {value, has_value()} pair.
    /// @param function the function to be called.
    template <typename F, typename R = std::invoke_result_t<F, const T&, bool>>
    [[nodiscard]] constexpr auto transformed(F function) const noexcept
      -> basic_valid_if<R, valid_flag_policy, typename valid_flag_policy::do_log> {
        const auto has_val{has_value(_value)};
        auto r{function(_value, has_val)};
        if constexpr(optional_like<decltype(r)>) {
            return r;
        } else {
            return {std::move(r), has_val};
        }
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    [[nodiscard]] constexpr auto operator*() const -> const_reference {
        assert(has_value());
        return value_anyway();
    }

    /// @brief Returns pointer to the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    [[nodiscard]] constexpr auto operator->() const -> const_pointer {
        assert(has_value());
        return &value_anyway();
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<std::reference_wrapper<T>>()
      const noexcept {
        if(has_value()) {
            return {value_anyway()};
        }
        return {};
    }

private:
    T& _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
/// @brief Primary template for conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Policy>
using valid_if = basic_valid_if<T, Policy, typename Policy::do_log>;
//------------------------------------------------------------------------------
/// @brief Overload of extract for conditionally valid values.
/// @pre opt.has_value()
export template <typename T, typename P, typename L>
[[nodiscard]] constexpr auto extract(const basic_valid_if<T, P, L>& opt) noexcept
  -> const T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.has_value()
export template <typename T, typename P, typename L>
[[nodiscard]] constexpr auto extract(basic_valid_if<T, P, L>& opt) noexcept
  -> T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.has_value()
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
export template <typename F, typename T, typename P>
auto operator<<(std::ostream& out, const basic_valid_if<F, T, P>& vif)
  -> std::ostream& {
    assert(vif.has_value());
    return out << vif.value_anyway();
}

export template <typename F, typename T, typename P, typename L>
auto operator<<(std::ostream& out, const valid_if_or_fallback<F, T, P, L>& viof)
  -> std::ostream& {
    if(viof.has_value()) {
        out << viof.value_anyway();
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
export template <typename Dst, typename P, typename L, typename Src>
struct within_limits<basic_valid_if<Dst, P, L>, Src> {
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
