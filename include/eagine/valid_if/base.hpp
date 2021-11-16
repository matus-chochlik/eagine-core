/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALID_IF_BASE_HPP
#define EAGINE_VALID_IF_BASE_HPP

#include "../assert.hpp"
#include "../branch_predict.hpp"
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace eagine {

/// @brief Basic template for conditionally-valid values.
/// @ingroup valid_if
/// @tparam T type of the stored, conditionally valid value.
/// @tparam Policy indicates under what conditions is the stored value valid.
template <typename T, typename Policy, typename DoLog, typename... P>
class basic_valid_if {
    static_assert(
      std::is_nothrow_default_constructible_v<Policy> ||
      std::is_nothrow_move_constructible_v<Policy>);

public:
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
      : _value{std::move(val)} {}

    /// @brief Constructor initializing the stored value and policy.
    constexpr basic_valid_if(T val, Policy plcy) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<T>&&
                 std::is_nothrow_move_constructible_v<Policy>))
      : _value{std::move(val)}
      , _policy{std::move(plcy)} {}

    /// @brief Move constructor.
    basic_valid_if(basic_valid_if&& that) noexcept(
      std::is_nothrow_move_constructible_v<T>&&
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
      std::is_nothrow_move_assignable_v<T>&&
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
      std::is_nothrow_copy_constructible_v<T>&&
        std::is_nothrow_copy_constructible_v<Policy>) -> auto& {
        if(this != std::addressof(that)) {
            basic_valid_if temp{that};
            *this = std::move(temp);
        }
        return *this;
    }

    /// @brief Copies @p value into this instance.
    auto operator=(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
      -> auto& {
        _value = value;
        return *this;
    }

    auto operator=(T&& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
      -> auto& {
        _value = std::move(value);
        return *this;
    }

    /// @brief The destructor.
    ~basic_valid_if() noexcept = default;

    /// @brief Checks if @p val is valid according to this object's policy.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto is_valid(const T& val, P... p) const noexcept -> bool {
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

    /// @brief Equality comparison.
    constexpr friend auto operator==(
      const basic_valid_if& a,
      const basic_valid_if& b) noexcept -> bool {
        return (a._value == b._value) && a.is_valid() && b.is_valid();
    }

    template <typename Log>
    void log_invalid(Log& log, const T& v, P... p) const {
        EAGINE_ASSERT(!is_valid(v, p...));
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
    auto value_or(T& fallback, P... p) noexcept -> auto& {
        return EAGINE_LIKELY(is_valid(p...)) ? _value : fallback;
    }

    /// @brief Returns the stored value if valid, otherwise returns fallback.
    /// @param p additional parameters for the policy validity check function.
    constexpr auto value_or(const T& fallback, P... p) const noexcept -> auto& {
        return EAGINE_LIKELY(is_valid(p...)) ? _value : fallback;
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
    /// @param p additional parameters for the policy validity check function.
    template <typename Func>
    auto then(const Func& func, P... p) const
      -> std::enable_if_t<std::is_same_v<std::result_of_t<Func(T)>, void>> {
        if(EAGINE_LIKELY(is_valid(p...))) {
            func(value(p...));
        }
    }

    /// @brief Calls a binary transforming function on {value, is_valid()} pair.
    /// @param func the function to be called.
    /// @param p additional parameters for the policy validity check function.
    template <typename Func>
    constexpr auto transformed(Func func, P... p) const noexcept {
        return func(_value, is_valid(p...));
    }

private:
    T _value{};
    [[no_unique_address]] Policy _policy{};
    [[no_unique_address]] DoLog _do_log{_policy};
};

} // namespace eagine

#endif // EAGINE_VALID_IF_BASE_HPP
