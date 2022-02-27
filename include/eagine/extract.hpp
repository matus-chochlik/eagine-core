/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_EXTRACT_HPP
#define EAGINE_EXTRACT_HPP

#include "assert.hpp"
#include "nothing.hpp"
#include <memory>
#include <optional>
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
template <typename T>
struct extract_traits;

/// @brief Returns the value type of an extractable.
/// @ingroup utilities
/// @see extract
template <typename T>
using extracted_type_t = std::remove_cv_t<typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::value_type>;

template <typename T>
using extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::result_type;

template <typename T>
using const_extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::const_result_type;

template <typename E, typename V>
static constinit const auto has_value_type_v =
  std::is_convertible_v<extracted_type_t<E>, V>;
//------------------------------------------------------------------------------
// pointers
template <typename T>
struct extract_traits<T*> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
template <typename T>
static constexpr auto extract(T* ptr) noexcept -> T& {
    return EAGINE_CONSTEXPR_ASSERT(bool(ptr), *ptr);
}
//------------------------------------------------------------------------------
template <typename T>
struct extract_traits<std::shared_ptr<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
template <typename T>
static constexpr auto extract(std::shared_ptr<T>& ptr) noexcept -> auto& {
    return EAGINE_CONSTEXPR_ASSERT(bool(ptr), *ptr);
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
template <typename T>
static constexpr auto extract(const std::shared_ptr<T>& ptr) noexcept -> auto& {
    return EAGINE_CONSTEXPR_ASSERT(bool(ptr), *ptr);
}
//------------------------------------------------------------------------------
template <typename T, typename D>
struct extract_traits<std::unique_ptr<T, D>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
template <typename T, typename D>
static constexpr auto extract(std::unique_ptr<T, D>& ptr) noexcept -> T& {
    return EAGINE_CONSTEXPR_ASSERT(bool(ptr), *ptr);
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
template <typename T, typename D>
static constexpr auto extract(const std::unique_ptr<T, D>& ptr) noexcept
  -> const T& {
    return EAGINE_CONSTEXPR_ASSERT(bool(ptr), *ptr);
}
//------------------------------------------------------------------------------
template <typename T>
struct extract_traits<std::optional<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(opt)
template <typename T>
static constexpr auto extract(std::optional<T>& opt) noexcept -> auto& {
    return EAGINE_CONSTEXPR_ASSERT(bool(opt), *opt);
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(opt)
template <typename T>
static constexpr auto extract(const std::optional<T>& opt) noexcept -> auto& {
    return EAGINE_CONSTEXPR_ASSERT(bool(opt), *opt);
}
//------------------------------------------------------------------------------
/// @brief Traits used for customization of class ok for the specified Outcome.
/// @see ok
template <typename Outcome>
struct ok_traits {
    /// @brief Returns additional info type for values that are not-OK.
    static constexpr auto nok_info(const Outcome&) noexcept -> nothing_t {
        return {};
    }
};
//------------------------------------------------------------------------------
/// @brief Value typically wrapping function call result and success indicator.
/// @tparam Outcome the actual function call result type.
/// @see ok_traits
/// @see valid_if
///
/// Instances of this class are used to store the result of a function call
/// and store the actual result together with some ingication whether the call
/// was actually successful. This means that the stored result value is
/// only optionally valid as indicated by the conversion-to-boolean operator.
///
/// The ok class can, depending on specializations of ok_traits, provide
/// additional information about why the associated function call failed,
/// for example an error code or error message string, etc.
///
/// This class is typically used to store and retrieve C-API function call
/// results where the success is indicated by @c errno or some such global
/// variable.
template <typename Outcome>
class ok {
private:
    Outcome _outcome{};

    using _traits = ok_traits<Outcome>;

public:
    /// @brief Construction from a function call @p outcome object.
    constexpr ok(Outcome&& outcome) noexcept(noexcept(std::declval<Outcome&&>()))
      : _outcome{std::move(outcome)} {}

    /// @brief Indicates if the stored outcome contains valid result value.
    explicit constexpr operator bool() noexcept(
      noexcept(bool(std::declval<Outcome&>()))) {
        return bool(_outcome);
    }

    /// @brief Indicates if the stored outcome contains valid result value.
    explicit constexpr operator bool() const
      noexcept(noexcept(bool(std::declval<const Outcome&>()))) {
        return bool(_outcome);
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    constexpr auto get() noexcept(noexcept(extract(_outcome)))
      -> decltype(extract(_outcome)) {
        return extract(_outcome);
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    constexpr auto get() const noexcept(noexcept(extract(_outcome)))
      -> decltype(extract(_outcome)) {
        return extract(_outcome);
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    constexpr operator decltype(extract(std::declval<Outcome&>()))() noexcept(
      noexcept(extract(_outcome))) {
        return extract(_outcome);
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    constexpr operator decltype(extract(std::declval<const Outcome&>()))() const
      noexcept(noexcept(extract(_outcome))) {
        return extract(_outcome);
    }

    constexpr auto nok() const noexcept
      -> decltype(_traits::nok_info(_outcome)) {
        return _traits::nok_info(_outcome);
    }

    constexpr auto operator!() const noexcept
      -> decltype(_traits::nok_info(_outcome)) {
        return _traits::nok_info(_outcome);
    }
};

template <typename Outcome>
ok(Outcome&& outcome) -> ok<Outcome>;
//------------------------------------------------------------------------------
/// @brief Overload of extract for instantiations of the ok template.
/// @relates ok
template <typename Outcome>
auto extract(const ok<Outcome>& x) noexcept -> const auto& {
    return x.get();
}
//------------------------------------------------------------------------------
/// @brief Overload of begin for instantiations of the ok template.
/// @relates ok
template <typename Outcome>
auto begin(
  const ok<Outcome>& x,
  decltype(std::declval<const ok<Outcome>&>().get().begin())* = nullptr) {
    return x.get().begin();
}
//------------------------------------------------------------------------------
/// @brief Overload of begin for instantiations of the ok template.
/// @relates ok
template <typename Outcome>
auto end(
  const ok<Outcome>& x,
  decltype(std::declval<const ok<Outcome>&>().get().end())* = nullptr) {
    return x.get().end();
}
//------------------------------------------------------------------------------
// clang-format off
template <typename T>
concept basic_extractable = requires(T v) {
	{ std::declval<eagine::extracted_type_t<T>>() };
	{ std::declval<eagine::extract_result_type_t<T>>() };
	extract(v);
};

template <typename T>
concept extractable = basic_extractable<T> && requires(T v) {
    { has_value(v) } -> std::convertible_to<bool>;
};
// clang-format on
//------------------------------------------------------------------------------
template <basic_extractable T>
static constexpr auto has_value(const T& v) noexcept {
    return bool(v);
}

template <extractable T>
static constexpr auto extract_or(
  T& opt_val,
  extract_result_type_t<T> fallback) noexcept -> extract_result_type_t<T> {
    return has_value(opt_val) ? extract(opt_val) : fallback;
}

template <extractable T>
static constexpr auto extract_or(
  const T& opt_val,
  const_extract_result_type_t<T> fallback) noexcept
  -> const_extract_result_type_t<T> {
    return has_value(opt_val) ? extract(opt_val) : fallback;
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_EXTRACT_HPP
