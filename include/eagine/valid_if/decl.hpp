/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALID_IF_DECL_HPP
#define EAGINE_VALID_IF_DECL_HPP

#include "../assert.hpp"
#include "base.hpp"

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Primary template for conditionally valid values.
/// @ingroup valid_if
template <typename T, typename Policy, typename DoLog = typename Policy::do_log>
using valid_if = basic_valid_if<T, Policy, DoLog>;
//------------------------------------------------------------------------------
/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
template <typename T, typename P, typename L>
static constexpr auto extract(const basic_valid_if<T, P, L>& opt) noexcept
  -> const T& {
    return EAGINE_CONSTEXPR_ASSERT(bool(opt), opt.value_anyway());
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
template <typename T, typename P, typename L>
static constexpr auto extract(basic_valid_if<T, P, L>& opt) noexcept -> T& {
    return EAGINE_CONSTEXPR_ASSERT(bool(opt), opt.value_anyway());
}

/// @brief Overload of extract for conditionally valid values.
/// @pre opt.is_valid()
template <typename T, typename P, typename L>
static constexpr auto extract(basic_valid_if<T, P, L>&& opt) noexcept -> T&& {
    return EAGINE_CONSTEXPR_ASSERT(bool(opt), std::move(opt.value_anyway()));
}
//------------------------------------------------------------------------------
/// @brief Overload of extract_or for conditionally valid values.
template <typename T, typename P, typename L, typename F>
static constexpr auto extract_or(
  const basic_valid_if<T, P, L>& opt,
  F&& fallback) noexcept -> std::enable_if_t<std::is_convertible_v<F, T>, T> {
    if(bool(opt)) {
        return opt.value_anyway();
    }
    return T{std::forward<F>(fallback)};
}
//------------------------------------------------------------------------------
/// @brief Overload of extract_or for conditionally valid values.
template <typename T, typename P, typename L, typename F>
static constexpr auto extract_or(
  basic_valid_if<T, P, L>& opt,
  T& fallback) noexcept -> T& {
    if(bool(opt)) {
        return opt.value_anyway();
    }
    return fallback;
}
//------------------------------------------------------------------------------
/// @brief Helper class storing both conditionally valid value and fallback.
template <typename F, typename T, typename P, typename L>
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
template <typename F, typename T, typename P, typename L>
static inline auto either_or(basic_valid_if<T, P, L> vi, F f) noexcept(
  noexcept(basic_valid_if<T, P, L>(
    std::declval<
      basic_valid_if<T, P, L>&&>())) && noexcept(F(std::declval<F&&>())))
  -> valid_if_or_fallback<F, T, P, L> {
    return {std::move(vi), std::move(f)};
}
//------------------------------------------------------------------------------
template <typename F, typename T, typename P, typename L>
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
template <typename T>
using optionally_valid = valid_if<T, valid_flag_policy>;
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_VALID_IF_DECL_HPP
