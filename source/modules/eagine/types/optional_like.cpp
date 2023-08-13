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

export template <typename T>
class optional_reference;

/// @brief Specialization of valid_if with flag indicating validity.
/// @ingroup valid_if
/// @see valid_if_indicated
export template <typename T>
using optionally_valid = valid_if<T, valid_flag_policy>;
//------------------------------------------------------------------------------
// optional-like API
//------------------------------------------------------------------------------
/// @brief Implements the common interface for optional-like types.
/// @ingroup optional_like
template <typename Derived, typename T>
class optional_like_crtp {
    template <typename Self, typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto _member(Self&& self, M C::*ptr) const noexcept
        requires(not std::is_member_function_pointer_v<decltype(ptr)>)
    {
        if constexpr(optional_like<M>) {
            if(self.has_value()) {
                return (self.get()).*ptr;
            } else {
                return M{};
            }
        } else {
            using R =
              std::conditional_t<std::is_const_v<T>, std::add_const_t<M>, M>;
            if(self.has_value()) {
                return optional_reference<R>{(*self).*ptr};
            } else {
                return optional_reference<R>{nothing};
            }
        }
    }

    template <typename V, typename... Args>
    [[nodiscard]] static constexpr auto
    _call_member(V* ptr, auto function, Args&&... args) noexcept(
      noexcept(std::invoke(function, *ptr, std::forward<Args>(args)...)));

public:
    /// @brief Type of the referenced value.
    using value_type = T;

    constexpr auto derived() && noexcept -> Derived&& {
        return static_cast<Derived&&>(*this);
    }

    constexpr auto derived() & noexcept -> Derived& {
        return static_cast<Derived&>(*this);
    }

    constexpr auto derived() const& noexcept -> const Derived& {
        return static_cast<const Derived&>(*this);
    }

    /// @brief Indicates if this has a valid value.
    /// @see has_value
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return derived().has_value();
    }

    [[nodiscard]] constexpr auto operator->() const noexcept -> auto* {
        assert(derived().has_value());
        return derived().get();
    }

    /// @brief Tri-state equality comparison of the stored object with a value.
    [[nodiscard]] auto constexpr operator==(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() == that, true};
        }
        return indeterminate;
    }

    /// @brief Tri-state equality comparison of the stored object with an optional-like.
    template <typename D>
    [[nodiscard]] auto constexpr operator==(
      const optional_like_crtp<D, T>& that) const noexcept -> tribool {
        if(derived().has_value() and that.derived()) {
            return {*derived() == *(that.derived()), true};
        }
        return indeterminate;
    }

    /// @brief Tri-state non-equality comparison of the stored object with a value.
    [[nodiscard]] constexpr auto operator!=(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() != that, true};
        }
        return indeterminate;
    }

    /// @brief Tri-state non-equality comparison of the stored object with an optional-like.
    template <typename D>
    [[nodiscard]] auto constexpr operator!=(
      const optional_like_crtp<D, T>& that) const noexcept -> tribool {
        if(derived().has_value() and that) {
            return {*derived() != *(that.derived()), true};
        }
        return indeterminate;
    }

    /// @brief Tri-state less-equal comparison of the stored object with a value.
    [[nodiscard]] constexpr auto operator<=(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() <= that, true};
        }
        return indeterminate;
    }

    /// @brief Tri-state greater-equal comparison of the stored object with a value.
    [[nodiscard]] constexpr auto operator>=(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() >= that, true};
        }
        return indeterminate;
    }

    /// @brief Tri-state less-than comparison of the stored object with a value.
    [[nodiscard]] constexpr auto operator<(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() < that, true};
        }
        return indeterminate;
    }

    /// @brief Tri-state less-than comparison of the stored object with an optional-like.
    template <typename D>
    [[nodiscard]] auto constexpr operator<(
      const optional_like_crtp<D, T>& that) const noexcept -> tribool {
        if(derived().has_value() and that) {
            return {*derived() < *(that.derived()), true};
        }
        return indeterminate;
    }

    /// @brief Tri-state greater-than comparison of the stored object with a value.
    [[nodiscard]] constexpr auto operator>(const T& that) const noexcept
      -> tribool {
        if(derived().has_value()) {
            return {*derived() > that, true};
        }
        return indeterminate;
    }

    /// @brief Implicit conversion to std::optional<std::reference_wrapper>
    [[nodiscard]] constexpr operator std::optional<std::reference_wrapper<T>>()
      const noexcept {
        if(derived().has_value()) {
            return {*derived()};
        }
        return {};
    }

    /// @brief Conversion to std::optional
    template <std::constructible_from<T> U>
    [[nodiscard]] constexpr operator std::optional<U>() const
      noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if(derived().has_value()) {
            return {*derived()};
        }
        return {};
    }

    [[nodiscard]] constexpr auto or_default() const noexcept
      -> std::conditional_t<std::is_function_v<T>, void, T> {
        if constexpr(not std::is_function_v<T>) {
            if(derived().has_value()) {
                return *derived();
            }
            return {};
        }
    }

    /// @brief Returns optional_reference to the held type.
    [[nodiscard]] auto ref() const noexcept -> optional_reference<T> {
        return {derived().get()};
    }

    /// @brief Indicates if this refers to the specified object.
    /// @returns tribool
    [[nodiscard]] constexpr auto refers_to(const T& object) const noexcept {
        return tribool{derived().get() == &object, derived().has_value()};
    }

    /// @brief Indicates if this refers to the same object as that.
    template <typename B>
    [[nodiscard]] constexpr auto refers_to(
      const optional_like_crtp<B, T>& that) const noexcept -> tribool {
        if(that) {
            return that.refers_to(*derived());
        }
        return indeterminate;
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(M C::*ptr) noexcept
        requires(not std::is_member_function_pointer_v<decltype(ptr)>)
    {
        return _member(derived(), ptr);
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(M C::*ptr) const noexcept
        requires(not std::is_member_function_pointer_v<decltype(ptr)>)
    {
        return _member(derived(), ptr);
    }

    /// @brief Returns the stored value if valid or @p fallback otherwise.
    /// @see has_value
    template <
      std::convertible_to<T> U,
      typename R = std::conditional_t<std::is_function_v<T>, T*, T>>
    [[nodiscard]] constexpr auto value_or(U&& fallback) const noexcept -> R {
        if(derived().has_value()) {
            if constexpr(std::is_function_v<T>) {
                return derived().get();
            } else {
                return *derived();
            }
        }
        return R(std::forward<U>(fallback));
    }

    [[nodiscard]] constexpr auto value_or(T& fallback) const noexcept -> auto& {
        return derived().has_value() ? *derived() : fallback;
    }

    /// @brief Constructs value of type C from the stored value or an empty optional-like.
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
        if(derived().has_value()) {
            return std::invoke(std::forward<F>(function), *derived());
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
        if(derived().has_value()) {
            std::invoke(std::forward<F>(function), *derived());
        }
    }

    template <typename F>
    constexpr auto and_then_true(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> tribool {
        if(derived().has_value()) {
            std::invoke(std::forward<F>(function), *derived());
            return true;
        } else {
            return indeterminate;
        }
    }

    /// @brief Return self if has value or the result of function.
    /// @see and_then
    /// @see transform
    template <typename F, typename R = std::invoke_result_t<F>>
        requires(std::same_as<Derived, R> or std::convertible_to<Derived, R>)
    constexpr auto or_else(F&& function) const
      noexcept(noexcept(std::invoke(std::forward<F>(function)))) -> R {
        if(derived().has_value()) {
            return derived();
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

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...),
      Args&&... args) const {
        return _call_member(
          derived().get(), function, std::forward<Args>(args)...);
    }

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) noexcept,
      Args&&... args) const noexcept {
        return _call_member(
          derived().get(), function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const,
      Args&&... args) const {
        return _call_member(
          derived().get(), function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const noexcept,
      Args&&... args) const noexcept {
        return _call_member(
          derived().get(), function, std::forward<Args>(args)...);
    }

    template <typename... Args>
    [[nodiscard]] auto bind_member(auto* ptr, Args&&... args) const noexcept {
        return [this, ptr, ... args{std::forward<Args>(args)}] {
            return this->member(ptr, args...);
        };
    }
};
//------------------------------------------------------------------------------
// optional reference
//------------------------------------------------------------------------------
export template <typename Base, typename T>
class basic_holder;

/// @brief Optional reference to an instance of type @p T.
/// @ingroup valid_if
/// @see valid_if
export template <typename T>
class optional_reference : public optional_like_crtp<optional_reference<T>, T> {

public:
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

    /// @brief Construction from a basic_holder to reference of type @p T.
    template <typename Base, std::derived_from<T> U>
    constexpr optional_reference(const basic_holder<Base, U>& holder) noexcept
      : _ptr{holder.get()} {}

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

    [[nodiscard]] constexpr auto operator*() const noexcept -> T& {
        return *_ptr;
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

    /// @brief Returns an optional reference to a derived type That.
    template <std::derived_from<T> That>
    [[nodiscard]] auto as(std::type_identity<That> = {}) const noexcept
      -> optional_reference<That> {
        if(auto that{dynamic_cast<That*>(_ptr)}) {
            return {that};
        }
        return {};
    }

    [[nodiscard]] explicit constexpr operator T&() noexcept {
        return value();
    }

    /// @brief Returns the stored reference.
    /// @see value
    [[nodiscard]] explicit constexpr operator const T&() const noexcept {
        return value();
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
class basic_valid_if
  : public optional_like_crtp<basic_valid_if<T, Policy, DoLog>, T> {

    friend class optional_like_crtp<basic_valid_if<T, Policy, DoLog>, T>;

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
    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return _policy(_value);
    }

    [[nodiscard]] constexpr auto operator*() && noexcept -> T&& {
        return std::move(_value);
    }

    [[nodiscard]] constexpr auto operator*() & noexcept -> T& {
        return _value;
    }

    [[nodiscard]] constexpr auto operator*() const& noexcept -> const T& {
        return _value;
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

private:
    constexpr auto get() const noexcept -> const T* {
        return &_value;
    }

    T _value{};
    Policy _policy;
    DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename DoLog>
class basic_valid_if<T&, Policy, DoLog>
  : public optional_like_crtp<basic_valid_if<T&, Policy, DoLog>, T> {
    friend class optional_like_crtp<basic_valid_if<T&, Policy, DoLog>, T>;

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

    /// @brief Returns the stored value regardless of its validity.
    [[nodiscard]] constexpr auto value_anyway() const noexcept -> T& {
        return _value;
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    [[nodiscard]] constexpr auto operator*() const noexcept -> T& {
        assert(has_value());
        return value_anyway();
    }

private:
    constexpr auto get() const noexcept -> T* {
        return &_value;
    }

    T& _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
// implementation
//------------------------------------------------------------------------------
template <typename Derived, typename T>
template <typename V, typename... Args>
[[nodiscard]] constexpr auto optional_like_crtp<Derived, T>::
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
template <typename Derived, typename T>
template <typename C, typename... Args>
[[nodiscard]] constexpr auto optional_like_crtp<Derived, T>::construct(
  Args&&... args) noexcept(noexcept(T(std::declval<T&>())))
  -> optionally_valid<C> {
    if(derived().has_value()) {
        return {C(*derived(), std::forward<Args>(args)...), true};
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename Derived, typename T>
template <typename F, typename R>
[[nodiscard]] constexpr auto optional_like_crtp<Derived, T>::transform(
  F&& function) const
  noexcept(
    noexcept(std::invoke(std::forward<F>(function), std::declval<T&>())) and
    std::is_nothrow_move_constructible_v<std::remove_cvref_t<R>>) {
    if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
        using P = std::conditional_t<
          std::is_reference_v<R>,
          std::remove_reference_t<R>,
          std::remove_pointer_t<R>>;
        if(derived().has_value()) {
            return optional_reference<P>{
              std::invoke(std::forward<F>(function), *derived())};
        } else {
            return optional_reference<P>{nothing};
        }
    } else if constexpr(std::is_same_v<R, bool>) {
        if(derived().has_value()) {
            return tribool{
              std::invoke(std::forward<F>(function), *derived()), true};
        } else {
            return tribool{indeterminate};
        }
    } else {
        if(derived().has_value()) {
            return optionally_valid<R>{
              std::invoke(std::forward<F>(function), *derived()), true};
        } else {
            return optionally_valid<R>{};
        }
    }
}
//------------------------------------------------------------------------------
// Wrappers for valid_if
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
// iterator, sentinel pair
//------------------------------------------------------------------------------
template <typename Iterator, typename Sentinel>
class optional_iterator_base {
public:
    constexpr optional_iterator_base() noexcept = default;

    constexpr optional_iterator_base(Iterator pos, Sentinel end) noexcept
      : _pos{pos}
      , _end{end} {}

    constexpr auto operator=(Iterator pos) noexcept -> optional_iterator_base& {
        _pos = pos;
        return *this;
    }

    constexpr auto has_value() const noexcept -> bool {
        return _pos != _end;
    }

    constexpr operator Iterator() const noexcept {
        return _pos;
    }

protected:
    constexpr auto _get() const noexcept -> Iterator {
        return _pos;
    }

private:
    Iterator _pos{};
    Sentinel _end{};
};
//------------------------------------------------------------------------------
export template <
  typename Iterator,
  typename Sentinel = Iterator,
  typename T = typename std::iterator_traits<Iterator>::value_type>
class optional_iterator
  : public optional_iterator_base<Iterator, Sentinel>
  , public optional_like_crtp<optional_iterator<Iterator, Sentinel, T>, T> {
    using base = optional_iterator_base<Iterator, Sentinel>;

public:
    using base::base;

    constexpr auto operator*() const noexcept -> auto& {
        assert(this->has_value());
        return *(this->_get());
    }

    constexpr auto get() const noexcept -> auto* {
        assert(this->has_value());
        return &(*(this->_get()));
    }

protected:
    Iterator _pos{};
    Sentinel _end{};
};
//------------------------------------------------------------------------------
export template <typename Iterator, typename Sentinel, typename K, typename V>
class optional_iterator<Iterator, Sentinel, std::pair<K, V>>
  : public optional_iterator_base<Iterator, Sentinel>
  , public optional_like_crtp<
      optional_iterator<Iterator, Sentinel, std::pair<K, V>>,
      V> {
    using base = optional_iterator_base<Iterator, Sentinel>;

public:
    using base::base;

    constexpr auto operator*() const noexcept -> auto& {
        assert(this->has_value());
        return this->_get()->second;
    }

    constexpr auto get() const noexcept -> auto* {
        assert(this->has_value());
        return &(this->_get()->second);
    }
};
//------------------------------------------------------------------------------
export template <typename Iterator, typename Sentinel, typename K, typename V>
class optional_iterator<Iterator, Sentinel, const std::pair<K, V>>
  : public optional_iterator_base<Iterator, Sentinel>
  , public optional_like_crtp<
      optional_iterator<Iterator, Sentinel, const std::pair<K, V>>,
      std::add_const_t<V>> {
    using base = optional_iterator_base<Iterator, Sentinel>;

public:
    using base::base;

    constexpr auto operator*() const noexcept -> auto& {
        assert(this->has_value());
        return this->_get()->second;
    }

    constexpr auto get() const noexcept -> auto* {
        assert(this->has_value());
        return &(this->_get()->second);
    }
};
//------------------------------------------------------------------------------
export template <typename W, typename K, typename T, typename C, typename A>
constexpr auto find(std::map<K, T, C, A>& m, W&& what) noexcept
  -> optional_iterator<
    typename std::map<K, T, C, A>::iterator,
    typename std::map<K, T, C, A>::iterator,
    typename std::map<K, T, C, A>::value_type> {
    return {m.find(std::forward<W>(what)), m.end()};
}

export template <typename W, typename K, typename T, typename C, typename A>
constexpr auto find(const std::map<K, T, C, A>& m, W&& what) noexcept
  -> optional_iterator<
    typename std::map<K, T, C, A>::const_iterator,
    typename std::map<K, T, C, A>::const_iterator,
    const typename std::map<K, T, C, A>::value_type> {
    return {m.find(std::forward<W>(what)), m.end()};
}
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

    template <typename X>
    struct _get_value_type;

    template <std::size_t... I>
    struct _get_value_type<std::index_sequence<I...>>
      : std::type_identity<
          std::tuple<decltype(std::declval<_co_t<I>>().value())...>> {};

public:
    using value_type = typename _get_value_type<_i_s>::type;

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

    constexpr auto operator*() const noexcept {
        return _value(_i_s{});
    }

    constexpr auto value() const noexcept {
        return _value(_i_s{});
    }

    template <typename... V>
        requires(sizeof...(O) == sizeof...(V))
    constexpr auto value_or(const std::tuple<V...>& fallback) const noexcept {
        return _value_or(fallback, _i_s{});
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
            return R{indeterminate};
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
};
//------------------------------------------------------------------------------
export template <optional_like... O>
constexpr auto meld(O&&... o) noexcept -> optional_like_tuple<O...> {
    return {std::move(o)...};
}
//------------------------------------------------------------------------------
// utility functions
//------------------------------------------------------------------------------
export template <typename T, typename... A>
constexpr auto get_if(std::variant<A...>& v) noexcept -> optional_reference<T> {
    return {std::get_if<T>(&v)};
}

export template <typename T, typename... A>
constexpr auto get_if(const std::variant<A...>& v) noexcept
  -> optional_reference<const T> {
    return {std::get_if<T>(&v)};
}
} // namespace eagine
