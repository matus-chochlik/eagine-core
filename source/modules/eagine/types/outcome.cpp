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
import :concepts;
import :limits;

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

    using _elref_t = decltype(*std::declval<Outcome&>());
    using _erref_t = decltype(*std::declval<Outcome&&>());
    using _ecref_t = decltype(*std::declval<const Outcome&>());

public:
    /// @brief Construction from a function call @p outcome object.
    constexpr ok(Outcome outcome) noexcept(
      std::is_nothrow_move_constructible_v<Outcome>)
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
    [[nodiscard]] constexpr auto get() & noexcept(noexcept(*_outcome))
      -> _elref_t {
        return *_outcome;
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr auto get() && noexcept(noexcept(*_outcome))
      -> _erref_t {
        return std::move(*_outcome);
    }

    /// @brief Extracts the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr auto get() const& noexcept(noexcept(*_outcome))
      -> _ecref_t {
        return *_outcome;
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr operator _elref_t() & noexcept(noexcept(*_outcome)) {
        return *_outcome;
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr operator _erref_t() && noexcept(
      noexcept(*_outcome)) {
        return std::move(*_outcome);
    }

    /// @brief Implicit conversion to the stored outcome value.
    /// @pre bool(*this)
    [[nodiscard]] constexpr operator _ecref_t() const& noexcept(
      noexcept(*_outcome)) {
        return *_outcome;
    }

    [[nodiscard]] constexpr operator Outcome&() & noexcept {
        return _outcome;
    }

    [[nodiscard]] constexpr operator Outcome&&() && noexcept {
        return std::move(_outcome);
    }

    [[nodiscard]] constexpr operator const Outcome&() const& noexcept {
        return _outcome;
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
ok(Outcome&& outcome) -> ok<std::remove_cvref_t<Outcome>>;
//------------------------------------------------------------------------------
export template <typename Dst, typename E>
[[nodiscard]] constexpr auto limit_cast(const ok<E>& value) noexcept -> Dst {
    return limit_cast<Dst>(value.get());
}

export template <typename T>
[[nodiscard]] constexpr auto span_size(const ok<T>& v) noexcept {
    return span_size(v.get());
}

export template <typename T>
[[nodiscard]] constexpr auto std_size(const ok<T>& v) noexcept {
    return std_size(v.get());
}
//------------------------------------------------------------------------------
export template <typename Outcome>
auto operator<<(std::ostream& out, const ok<Outcome>& x) -> std::ostream& {
    return out << x.get();
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
