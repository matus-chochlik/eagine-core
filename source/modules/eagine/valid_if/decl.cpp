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

import eagine.core.types;
import eagine.core.concepts;
import <sstream>;
import <stdexcept>;
import <type_traits>;
import <utility>;

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
    auto operator()(const T&) const noexcept -> bool {
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
      std::is_nothrow_default_constructible_v<Policy> ||
      std::is_nothrow_move_constructible_v<Policy>);

public:
    using value_type = std::remove_cv_t<std::remove_reference_t<T>>;
    using reference = std::conditional_t<std::is_reference_v<T>, T, T&>;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /// @brief Returns a reference to this object's policy.
    [[nodiscard]] auto policy() noexcept -> Policy& {
        return _policy;
    }

    /// @brief Returns a const reference to this object's policy.
    [[nodiscard]] auto policy() const noexcept -> const Policy& {
        return _policy;
    }

    /// @brief Default constructor.
    constexpr basic_valid_if() noexcept(
      std::is_nothrow_default_constructible_v<T>) = default;

    /// @brief Constructor initializing the stored value by @p val.
    constexpr basic_valid_if(T val) noexcept(
      std::is_nothrow_move_constructible_v<T>)
        requires(!std::is_reference_v<T>)
    : _value{std::move(val)} {}

    constexpr basic_valid_if(T val) noexcept
        requires(std::is_reference_v<T>)
    : _value{val} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T val, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<value_type>&&
                 std::is_nothrow_move_constructible_v<Policy>))
        requires(!std::is_reference_v<T>)
    : _value{std::move(val)}
    , _policy{std::move(plcy)} {}

    constexpr basic_valid_if(T val, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<Policy>))
        requires(std::is_reference_v<T>)
    : _value{val}
    , _policy{std::move(plcy)} {}

    /// @brief Move constructor.
    basic_valid_if(basic_valid_if&& that) noexcept(
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
    auto operator=(basic_valid_if&& that) noexcept(
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
    auto operator=(const basic_valid_if& that) noexcept(
      std::is_nothrow_copy_constructible_v<value_type>&&
        std::is_nothrow_copy_constructible_v<Policy>) -> auto& {
        if(this != std::addressof(that)) {
            basic_valid_if temp{that};
            *this = std::move(temp);
        }
        return *this;
    }

    /// @brief Copies @p value into this instance.
    auto operator=(const value_type& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = value;
        return *this;
    }

    auto operator=(value_type&& value) noexcept(
      std::is_nothrow_copy_assignable_v<value_type>) -> auto& {
        _value = std::move(value);
        return *this;
    }

    /// @brief The destructor.
    ~basic_valid_if() noexcept = default;

    /// @brief Checks if @p val is valid according to this object's policy.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto is_valid(const_reference val, P... p) const noexcept
      -> bool {
        return _policy(val, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto is_valid(P... p) const noexcept {
        return is_valid(_value, p...);
    }

    /// @brief Checks if the stored value is valid according to policy.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto has_value(P... p) const noexcept {
        return is_valid(p...);
    }

    /// @brief Indicates if the stored value is valid according to policy.
    /// @see is_valid
    explicit operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Equality comparison.
    constexpr friend auto operator==(
      const basic_valid_if& a,
      const basic_valid_if& b) noexcept -> bool {
        return (a._value == b._value) && a.is_valid() && b.is_valid();
    }

    /// @brief Equality comparison of the stored value with @p v.
    constexpr auto operator==(const value_type& v) const -> tribool {
        return {this->value_anyway() == v, !is_valid()};
    }

    /// @brief Non-equality comparison of the stored value with @p v.
    constexpr auto operator!=(const value_type& v) const -> tribool {
        return {this->value_anyway() != v, !is_valid()};
    }

    /// @brief Less-than comparison of the stored value with @p v.
    constexpr auto operator<(const value_type& v) const -> tribool {
        return {this->value_anyway() < v, !is_valid()};
    }

    /// @brief Greater-than comparison of the stored value with @p v.
    constexpr auto operator>(const value_type& v) const -> tribool {
        return {this->value_anyway() > v, !is_valid()};
    }

    /// @brief Less-equal comparison of the stored value with @p v.
    constexpr auto operator<=(const value_type& v) const -> tribool {
        return {this->value_anyway() <= v, !is_valid()};
    }

    /// @brief Greater-equal comparison of the stored value with @p v.
    constexpr auto operator>=(const value_type& v) const -> tribool {
        return {this->value_anyway() >= v, !is_valid()};
    }

    template <typename Log>
    void log_invalid(Log& log, const value_type& v, P... p) const {
        assert(!is_valid(v, p...));
        _do_log(log, v, p...);
    }

    template <typename Log>
    void log_invalid(Log& log, P... p) const {
        log_invalid(log, _value, p...);
    }

    /// @brief Calls the specified function if the stored value is invalid.
    /// @param func the function to be called.
    /// @param p additional parameters for the policy validity check function.
    template <typename Func>
    auto call_if_invalid(Func func, P... p) -> auto& {
        if(!is_valid(p...)) {
            func(_do_log, _value, p...);
        }
        return *this;
    }

    /// @brief Throws an exception if the stored value is invalid.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    void throw_if_invalid(P... p) const {
        if(!is_valid(p...)) {
            std::stringstream ss;
            log_invalid(ss, p...);
            throw std::runtime_error(ss.str());
        }
    }

    /// @brief Returns the stored value if it is valid otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    auto value(P... p) -> T& {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if it is valid, otherwise throws.
    /// @param p additional parameters for the policy validity check function.
    /// @throws std::runtime_error
    auto value(P... p) const -> const T& {
        throw_if_invalid(p...);
        return _value;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    auto value_or(reference fallback, P... p) noexcept -> auto& {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto value_or(const_reference fallback, P... p) const noexcept
      -> auto& {
        if(is_valid(p...)) [[likely]] {
            return _value;
        }
        return fallback;
    }

    /// @brief Returns the stored value regardless of its validity.
    constexpr auto value_anyway() const noexcept -> const auto& {
        return _value;
    }

    /// @brief Returns the stored value regardless of its validity.
    constexpr auto value_anyway() noexcept -> auto& {
        return _value;
    }

    /// @brief Calls the specified function if the stored value is valid.
    /// @param func the function to be called.
    template <typename Func>
    auto then(const Func& func) const -> basic_valid_if<
      std::invoke_result_t<Func, T>,
      valid_flag_policy,
      typename valid_flag_policy::do_log>
        requires(!std::is_same_v<std::invoke_result_t<Func, T>, void>)
    {
        if(is_valid()) {
            return {func(this->value_anyway()), true};
        }
        return {};
    }

    /// @brief Calls the specified function if the stored valus is valid.
    /// @param func the function to call.
    /// @see then
    template <typename Func>
    auto operator|(const Func& func) const {
        return then(func);
    }

    /// @brief Calls a binary transforming function on {value, is_valid()} pair.
    /// @param func the function to be called.
    template <typename Func>
    constexpr auto transformed(Func func, P... p) const noexcept
      -> basic_valid_if<
        std::invoke_result_t<Func, T, bool>,
        valid_flag_policy,
        typename valid_flag_policy::do_log> {
        const auto v{is_valid(p...)};
        auto r{func(_value, v)};
        if constexpr(extractable<decltype(r)>) {
            return r;
        } else {
            return {std::move(r), v};
        }
    }

    /// @brief Returns the stored value if valid, returns fallback otherwise.
    /// @see basic_valid_if::value_or
    auto operator/(const_reference fallback) const noexcept -> const_reference {
        return value_or(fallback);
    }

    /// @brief Returns the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    auto operator*() const noexcept -> const_reference {
        return value();
    }

    /// @brief Returns pointer to the stored value, throws if it is invalid.
    /// @see basic_valid_if::value
    /// @throws std::runtime_error
    auto operator->() const noexcept -> const_pointer {
        return &value();
    }

private:
    T _value{};
    [[no_unique_address]] Policy _policy;
    [[no_unique_address]] DoLog _do_log{_policy};
};
//------------------------------------------------------------------------------
export template <typename T, typename P, typename L, typename... A>
constexpr auto has_value(const basic_valid_if<T, P, L>& v, A&&... a) noexcept
  -> bool {
    return v.has_value(std::forward<A>(a)...);
}

/// @brief Equality comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator==(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() == v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}

/// @brief Non-equality comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator!=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() != v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}

/// @brief Less-than comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator<(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() < v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}

/// @brief Greater-than comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator>(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() > v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}

/// @brief Less-equal comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator<=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() <= v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}

/// @brief Greater-equal comparison of two conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Po1, typename Po2, typename L1, typename L2>
constexpr auto operator>=(
  const basic_valid_if<T, Po1, L1>& v1,
  const basic_valid_if<T, Po2, L2>& v2) noexcept -> tribool {
    return {
      (v1.value_anyway() >= v2.value_anyway()),
      (!v1.is_valid() || !v2.is_valid())};
}
//------------------------------------------------------------------------------
/// @brief Primary template for conditionally valid values.
/// @ingroup valid_if
export template <typename T, typename Policy>
using valid_if = basic_valid_if<T, Policy, typename Policy::do_log>;
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits;

export template <typename T, typename P, typename L>
struct extract_traits<basic_valid_if<T, P, L>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
constexpr auto extract(const basic_valid_if<T, P, L>& opt) noexcept
  -> const T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
constexpr auto extract(basic_valid_if<T, P, L>& opt) noexcept -> T& {
    assert(opt);
    return opt.value_anyway();
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
export template <typename T, typename P, typename L>
constexpr auto extract(basic_valid_if<T, P, L>&& opt) noexcept -> T&& {
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
    valid_if_or_fallback(basic_valid_if<T, P, L> vi, F fallback) noexcept(
      noexcept(basic_valid_if<T, P, L>(
        std::declval<
          basic_valid_if<T, P, L>&&>())) && noexcept(F(std::declval<F&&>())))
      : basic_valid_if<T, P, L>{std::move(vi)}
      , _fallback{std::move(fallback)} {}

    /// @brief Returns the stored fallback value.
    auto fallback() const noexcept -> const F& {
        return _fallback;
    }

    /// @brief Returns the stored fallback value.
    auto fallback() noexcept -> F& {
        return _fallback;
    }

private:
    F _fallback{};
};
//------------------------------------------------------------------------------
/// @brief Constructor function for valid_if_or_fallback.
/// @ingroup valid_if
export template <typename F, typename T, typename P, typename L>
auto either_or(basic_valid_if<T, P, L> vi, F f) noexcept(
  noexcept(basic_valid_if<T, P, L>(
    std::declval<
      basic_valid_if<T, P, L>&&>())) && noexcept(F(std::declval<F&&>())))
  -> valid_if_or_fallback<F, T, P, L> {
    return {std::move(vi), std::move(f)};
}
//------------------------------------------------------------------------------
export template <typename F, typename T, typename P, typename L>
auto operator<<(std::ostream& out, const valid_if_or_fallback<F, T, P, L>& viof)
  -> std::ostream& {
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
} // namespace eagine
