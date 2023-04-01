/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:outcome;
import std;
import :basic;
import :extract;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Traits used for customization of class ok for the specified Outcome.
/// @see ok
export template <typename Outcome>
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
export template <typename Outcome>
class ok {
private:
    Outcome _outcome{};

    using _traits = ok_traits<Outcome>;

public:
    /// @brief Construction from a function call @p outcome object.
    constexpr ok(Outcome&& outcome) noexcept(noexcept(std::declval<Outcome&&>()))
      : _outcome{std::move(outcome)} {}

    /// @brief Indicates if the stored outcome contains valid result value.
    [[nodiscard]] explicit constexpr operator bool() noexcept(
      noexcept(bool(std::declval<Outcome&>()))) {
        return bool(_outcome);
    }

    /// @brief Indicates if the stored outcome contains valid result value.
    [[nodiscard]] explicit constexpr operator bool() const
      noexcept(noexcept(bool(std::declval<const Outcome&>()))) {
        return bool(_outcome);
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr auto get() noexcept(noexcept(extract(_outcome)))
      -> decltype(extract(_outcome)) {
        return extract(_outcome);
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr auto get() const
      noexcept(noexcept(extract(_outcome))) -> decltype(extract(_outcome)) {
        return extract(_outcome);
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr operator decltype(extract(
      std::declval<Outcome&>()))() noexcept(noexcept(extract(_outcome))) {
        return extract(_outcome);
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr operator decltype(
      extract(std::declval<const Outcome&>()))() const
      noexcept(noexcept(extract(_outcome))) {
        return extract(_outcome);
    }

    [[nodiscard]] constexpr auto nok() const noexcept
      -> decltype(_traits::nok_info(_outcome)) {
        return _traits::nok_info(_outcome);
    }

    [[nodiscard]] constexpr auto operator!() const noexcept
      -> decltype(_traits::nok_info(_outcome)) {
        return _traits::nok_info(_outcome);
    }
};

export template <typename Outcome>
ok(Outcome&& outcome) -> ok<Outcome>;
//------------------------------------------------------------------------------
/// @brief Overload of extract for instantiations of the ok template.
/// @relates ok
export template <typename Outcome>
[[nodiscard]] auto extract(const ok<Outcome>& x) noexcept -> const auto& {
    return x.get();
}

/// @brief Overload of extract for instantiations of the ok template.
/// @relates ok
export template <typename Outcome>
[[nodiscard]] auto extract(ok<Outcome>& x) noexcept -> auto& {
    return x.get();
}
//------------------------------------------------------------------------------
/// @brief Overload of begin for instantiations of the ok template.
/// @relates ok
export template <typename Outcome>
[[nodiscard]] auto begin(
  const ok<Outcome>& x,
  decltype(std::declval<const ok<Outcome>&>().get().begin())* = nullptr) {
    return x.get().begin();
}
//------------------------------------------------------------------------------
/// @brief Overload of begin for instantiations of the ok template.
/// @relates ok
export template <typename Outcome>
[[nodiscard]] auto end(
  const ok<Outcome>& x,
  decltype(std::declval<const ok<Outcome>&>().get().end())* = nullptr) {
    return x.get().end();
}
//------------------------------------------------------------------------------
} // namespace eagine
