/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cassert>

export module eagine.core.types:optional_like;

import std;
import :concepts;
import :basic;
import :tribool;
import :limits;

namespace eagine {
//------------------------------------------------------------------------------
// optionally-valid
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
class basic_valid_if;

/// @brief Primary template for conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Policy>
using valid_if = basic_valid_if<T, Policy, typename Policy::do_log>;

/// @brief Specialization of valid_if with flag indicating validity.
/// @ingroup valid_if
/// @see valid_if_indicated
export template <typename T>
using optionally_valid = valid_if<T, valid_flag_policy>;
//------------------------------------------------------------------------------
// optional reference
//------------------------------------------------------------------------------
/// @brief Optional reference to an instance of type @p T.
/// @ingroup valid_if
/// @see valid_if
export template <typename T>
class optional_reference {

    template <typename V, typename... Args>
    [[nodiscard]] static constexpr auto
    _call_member(V* ptr, auto function, Args&&... args) noexcept(
      noexcept(std::invoke(function, *ptr, std::forward<Args>(args)...)));

public:
    /// @brief Type of the referenced value.
    using value_type = T;

    /// @brief Construction from a pointer to reference of type @p T.
    constexpr optional_reference(T* ptr) noexcept
      : _ptr{ptr} {}

    /// @brief Construction from a reference to reference of type @p T.
    constexpr optional_reference(T& ref) noexcept
      : _ptr{&ref} {}

    template <typename U>
        requires(std::is_convertible_v<U*, T*> and not std::is_same_v<U, T>)
    constexpr optional_reference(optional_reference<U> that) noexcept
      : _ptr{that.get()} {}

    /// @brief Construction from a unique_ptr to reference of type @p T.
    template <std::derived_from<T> U>
    constexpr optional_reference(const std::unique_ptr<U>& ptr) noexcept
      : _ptr{ptr.get()} {}

    /// @brief Construction from a shared_ptr to reference of type @p T.
    template <std::derived_from<T> U>
    constexpr optional_reference(const std::shared_ptr<U>& ptr) noexcept
      : _ptr{ptr.get()} {}

    /// @brief Move constructor.
    constexpr optional_reference(optional_reference&&) noexcept = default;

    /// @brief Copy constructor.
    constexpr optional_reference(const optional_reference&) = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(optional_reference&&) noexcept
      -> optional_reference& = default;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const optional_reference&)
      -> optional_reference& = default;

    ~optional_reference() noexcept = default;

    /// @brief Default construction
    /// @post not has_value()
    constexpr optional_reference() noexcept = default;

    /// @brief Construction from nothing.
    /// @post not has_value()
    constexpr optional_reference(nothing_t) noexcept {}

    /// @brief Construction from nullptr.
    /// @post not has_value()
    constexpr optional_reference(std::nullptr_t) noexcept {}

    /// @brief Indicates if this stores a valid reference.
    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return _ptr != nullptr;
    }

    /// @brief Resets the reference to invalid state.
    /// @post not has_value()
    constexpr auto reset() noexcept -> optional_reference& {
        _ptr = nullptr;
        return *this;
    }

    /// @brief Indicates in this reference specified to the specified object.
    [[nodiscard]] constexpr auto refers_to(const T& object) const noexcept
      -> tribool {
        return tribool{_ptr == &object, has_value()};
    }

    /// @brief Indicates in this reference the same object as @p that.
    template <std::derived_from<T> U>
    [[nodiscard]] constexpr auto refers_to(
      const optional_reference<U>& that) const noexcept -> tribool {
        if(that.has_value()) {
            return refers_to(*that);
        }
        return indeterminate;
    }

    /// @brief Indicates if this stores a valid reference.
    /// @see has_value
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> T& {
        assert(has_value());
        return *_ptr;
    }

    [[nodiscard]] constexpr auto operator->() const noexcept -> T* {
        assert(has_value());
        return _ptr;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> T* {
        return _ptr;
    }

    /// @brief Returns the stored reference.
    /// @pre has_value()
    [[nodiscard]] constexpr auto value() const noexcept -> T& {
        assert(has_value());
        return *_ptr;
    }

    /// @brief Returns the stored value if valid or @p fallback otherwise.
    /// @see has_value
    template <
      std::convertible_to<T> U,
      typename R = std::conditional_t<std::is_function_v<T>, T*, T>>
    [[nodiscard]] constexpr auto value_or(U&& fallback) const noexcept -> R {
        if(has_value()) {
            if constexpr(std::is_function_v<T>) {
                return _ptr;
            } else {
                return *_ptr;
            }
        }
        return R(std::forward<U>(fallback));
    }

    [[nodiscard]] constexpr auto value_or(T& fallback) const noexcept -> T& {
        if(has_value()) {
            return *_ptr;
        }
        return fallback;
    }

    [[nodiscard]] explicit constexpr operator T&() noexcept {
        return value();
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C, typename... Args>
    [[nodiscard]] constexpr auto construct(Args&&... args) noexcept(
      noexcept(T(std::declval<T&>()))) -> optionally_valid<C>;

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
    constexpr auto and_then(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), *_ptr);
        } else {
            return R{};
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
        requires(std::is_void_v<R>)
    constexpr void and_then(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), *_ptr);
        }
    }

    template <typename F>
    constexpr auto and_then_true(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> tribool {
        if(has_value()) {
            std::invoke(std::forward<F>(function), *_ptr);
            return true;
        } else {
            return indeterminate;
        }
    }

    /// @brief Return self if has value or the result of function.
    /// @see and_then
    /// @see transform
    template <typename F, typename R = std::invoke_result_t<F>>
        requires(
          std::same_as<R, optional_reference> or
          std::convertible_to<R, optional_reference>)
    constexpr auto or_else(F&& function) const
      noexcept(noexcept(std::invoke(std::forward<F>(function))))
        -> optional_reference {
        if(has_value()) {
            return *this;
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see and_then
    /// @see or_else
    template <typename F, typename R = std::invoke_result_t<F, T&>>
    [[nodiscard]] constexpr auto transform(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>())) and
      std::is_nothrow_move_constructible_v<std::remove_cvref_t<R>>);

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(M C::*ptr) const noexcept
        requires(not std::is_member_function_pointer_v<decltype(ptr)>)
    {
        if constexpr(optional_like<M>) {
            if(has_value()) {
                return value().*ptr;
            } else {
                return M{};
            }
        } else {
            using R =
              std::conditional_t<std::is_const_v<T>, std::add_const_t<M>, M>;
            if(has_value()) {
                return optional_reference<R>{value().*ptr};
            } else {
                return optional_reference<R>{nothing};
            }
        }
    }

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...),
      Args&&... args) const {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) noexcept,
      Args&&... args) const noexcept {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const,
      Args&&... args) const {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const noexcept,
      Args&&... args) const noexcept {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename... Args>
    [[nodiscard]] auto bind_member(auto* ptr, Args&&... args) const noexcept {
        return [this, ptr, ... args{std::forward<Args>(args)}] {
            return this->member(ptr, args...);
        };
    }

    template <std::derived_from<T> Derived>
    [[nodiscard]] auto as(std::type_identity<Derived> = {}) const noexcept
      -> optional_reference<Derived> {
        if(auto derived{dynamic_cast<Derived*>(_ptr)}) {
            return {derived};
        }
        return {};
    }

    /// @brief Returns the stored reference.
    /// @see value
    [[nodiscard]] explicit constexpr operator const T&() const noexcept {
        return value();
    }

    /// @brief Tri-state equality comparison of the referred instance with a value.
    [[nodiscard]] auto constexpr operator==(const T& r) const noexcept
      -> tribool {
        if(has_value()) {
            return value() == r;
        }
        return indeterminate;
    }

    /// @brief Tri-state nonequality comparison of the referred instance with a value.
    [[nodiscard]] constexpr auto operator!=(const T& r) const noexcept
      -> tribool {
        if(has_value()) {
            return value() != r;
        }
        return indeterminate;
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<std::reference_wrapper<T>>()
      const noexcept {
        if(has_value()) {
            return {value()};
        }
        return {};
    }

private:
    T* _ptr{nullptr};
};
//------------------------------------------------------------------------------
export template <typename T>
optional_reference(T* ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(T& ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(const std::unique_ptr<T>& ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(const std::shared_ptr<T>& ptr) -> optional_reference<T>;
//------------------------------------------------------------------------------
// basic valid-if
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

    template <optional_like O, typename U>
        requires(not std::same_as<Policy, valid_flag_policy>)
    constexpr basic_valid_if(O opt, U&& fallback) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : _value{opt ? *opt : T(std::forward<U>(fallback))} {}

    template <optional_like O>
        requires(not std::same_as<Policy, valid_flag_policy>)
    constexpr basic_valid_if(O opt, nothing_t) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : _value{opt ? *opt : T{}} {}

    template <optional_like O>
        requires(std::same_as<Policy, valid_flag_policy>)
    constexpr basic_valid_if(O opt) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : _value{opt ? *opt : T{}}
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
      noexcept(T(std::declval<T&>()))) -> optionally_valid<C> {
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
    [[nodiscard]] auto construct(Args&&... args) const
      noexcept(noexcept(T(std::declval<const T&>()))) -> optionally_valid<C> {
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
// implementation
//------------------------------------------------------------------------------
template <typename T>
template <typename V, typename... Args>
[[nodiscard]] constexpr auto optional_reference<T>::
  _call_member(V* ptr, auto function, Args&&... args) noexcept(
    noexcept(std::invoke(function, *ptr, std::forward<Args>(args)...))) {
    using R = std::invoke_result_t<
      decltype(function),
      std::conditional_t<std::is_const_v<V>, std::add_const_t<T>, T>&,
      Args...>;
    if constexpr(std::is_void_v<R>) {
        if(ptr) {
            std::invoke(function, *ptr, std::forward<Args>(args)...);
        }
    } else if constexpr(std::is_same_v<R, bool>) {
        if(ptr) {
            return tribool{
              std::invoke(function, *ptr, std::forward<Args>(args)...), true};
        } else {
            return tribool{indeterminate};
        }
    } else if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
        using P = std::conditional_t<
          std::is_reference_v<R>,
          std::remove_reference_t<R>,
          std::remove_pointer_t<R>>;
        if(ptr) {
            return optional_reference<P>{
              std::invoke(function, *ptr, std::forward<Args>(args)...)};
        } else {
            return optional_reference<P>{nothing};
        }
    } else {
        if constexpr(optional_like<R>) {
            if(ptr) {
                return std::invoke(function, *ptr, std::forward<Args>(args)...);
            } else {
                return R{};
            }
        } else {
            if(ptr) {
                return optionally_valid<R>{
                  std::invoke(function, *ptr, std::forward<Args>(args)...),
                  true};
            } else {
                return optionally_valid<R>{};
            }
        }
    }
}
//------------------------------------------------------------------------------
template <typename T>
template <typename C, typename... Args>
[[nodiscard]] constexpr auto optional_reference<T>::construct(
  Args&&... args) noexcept(noexcept(T(std::declval<T&>())))
  -> optionally_valid<C> {
    if(has_value()) {
        return {C(value(), std::forward<Args>(args)...), true};
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename T>
template <typename F, typename R>
[[nodiscard]] constexpr auto optional_reference<T>::transform(F&& function) const
  noexcept(
    noexcept(std::invoke(std::forward<F>(function), std::declval<T&>())) and
    std::is_nothrow_move_constructible_v<std::remove_cvref_t<R>>) {
    if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
        using P = std::conditional_t<
          std::is_reference_v<R>,
          std::remove_reference_t<R>,
          std::remove_pointer_t<R>>;
        if(_ptr) {
            return optional_reference<P>{
              std::invoke(std::forward<F>(function), *_ptr)};
        } else {
            return optional_reference<P>{nothing};
        }
    } else if constexpr(std::is_same_v<R, bool>) {
        if(_ptr) {
            return tribool{std::invoke(std::forward<F>(function), *_ptr), true};
        } else {
            return tribool{indeterminate};
        }
    } else {
        if(_ptr) {
            return optionally_valid<R>{
              std::invoke(std::forward<F>(function), *_ptr), true};
        } else {
            return optionally_valid<R>{};
        }
    }
}
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
/// @brief Policy for values valid when non-boolean Indicator has the specified value.
/// @ingroup valid_if
export template <typename Indicator, typename Comparable, Comparable value>
struct valid_if_indicated_policy {
    Indicator _indicator{};

    constexpr valid_if_indicated_policy() noexcept = default;

    constexpr valid_if_indicated_policy(Indicator indicator) noexcept
      : _indicator{std::move(indicator)} {}

    /// @brief Indicates value validity, true if indicator == value.
    template <typename T>
    constexpr auto operator()(const T&) const noexcept {
        return Comparable(_indicator) == value;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T&) const {
            log << "indicator is " << value;
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
  Comparable value = true>
using valid_if_indicated =
  valid_if<T, valid_if_indicated_policy<Indicator, Comparable, value>>;
//------------------------------------------------------------------------------
// optional_like_tuple
//------------------------------------------------------------------------------
export template <optional_like... O>
class optional_like_tuple {
    using _i_s = std::make_index_sequence<sizeof...(O)>;

    template <std::size_t I>
    using _co_t = const std::tuple_element_t<I, std::tuple<O...>>&;

    template <std::size_t I>
    using _ce_t = decltype(*std::declval<_co_t<I>>());

public:
    constexpr optional_like_tuple() noexcept = default;

    constexpr optional_like_tuple(O&&... opts) noexcept
      : _opts{std::move(opts)...} {}

    constexpr auto has_value() const noexcept -> bool {
        return std::apply(
          [](auto&... e) { return (... and e.has_value()); }, _opts);
    }

    constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    constexpr auto value() const noexcept {
        return _value(_i_s{});
    }

    template <typename... V>
        requires(sizeof...(O) == sizeof...(V))
    constexpr auto value_or(V&&... fallback) const noexcept {
        return _value_or(std::tie(fallback...), _i_s{});
    }

    template <typename Func>
    constexpr auto and_then(Func&& func) const {
        return _and_then(std::forward<Func>(func), _i_s{});
    }

    template <typename Func, std::size_t... I>
    constexpr auto _transform(Func&& func, std::index_sequence<I...>) const {
        using T = std::invoke_result_t<Func, _ce_t<I>...>;
        if constexpr(std::is_pointer_v<T> or std::is_reference_v<T>) {
            using R = optional_reference<T>;
            if(has_value()) {
                return R{std::apply(
                  [&func](auto&... o) { return func(*o...); }, _opts)};
            }
            return R{};
        } else if constexpr(std::is_same_v<T, bool>) {
            using R = tribool;
            if(has_value()) {
                return R{
                  std::apply(
                    [&func](auto&... o) { return func(*o...); }, _opts),
                  true};
            }
            return R{};
        } else {
            using R = optionally_valid<T>;
            if(has_value()) {
                return R{
                  std::apply(
                    [&func](auto&... o) { return func(*o...); }, _opts),
                  true};
            }
            return R{};
        }
    }

    template <typename Func>
    constexpr auto transform(Func&& func) const {
        return _transform(std::forward<Func>(func), _i_s{});
    }

private:
    template <std::size_t... I>
    constexpr auto _value(std::index_sequence<I...>) const noexcept
      -> std::tuple<decltype(std::declval<_co_t<I>>().value())...> {
        return {std::get<I>(_opts).value()...};
    }

    template <typename Tup, std::size_t... I>
    constexpr auto _value_or(Tup fallback, std::index_sequence<I...>) const
      -> std::tuple<
        decltype(std::declval<_co_t<I>>().value_or(std::get<I>(fallback)))...> {
        return {std::get<I>(_opts).value_or(std::get<I>(fallback))...};
    }

    template <typename Func, std::size_t... I>
    constexpr auto _and_then(Func&& func, std::index_sequence<I...>) const
      -> std::invoke_result_t<Func, _ce_t<I>...>
        requires(optional_like<std::invoke_result_t<Func, _ce_t<I>...>>)
    {
        if(has_value()) {
            return std::apply(
              [&func](auto&... o) { return func(*o...); }, _opts);
        }
        return {};
    }

    template <typename Func, std::size_t... I>
    constexpr auto _and_then(Func&& func, std::index_sequence<I...>) const
      -> std::invoke_result_t<Func, _ce_t<I>...>
        requires(std::is_void_v<std::invoke_result_t<Func, _ce_t<I>...>>)
    {
        if(has_value()) {
            std::apply([&func](auto&... o) { func(*o...); }, _opts);
        }
    }

    std::tuple<O...> _opts{};
};
//------------------------------------------------------------------------------
export template <optional_like... O>
constexpr auto meld(O&&... o) noexcept -> optional_like_tuple<O...> {
    return {std::move(o)...};
}
//------------------------------------------------------------------------------
} // namespace eagine
