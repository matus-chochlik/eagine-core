/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.c_api:result;

import std;
import eagine.core.types;
import eagine.core.utility;

namespace eagine {
namespace c_api {
//------------------------------------------------------------------------------
/// @brief Exception wrapping information about failed C-API function call result.
/// @ingroup c_api_wrap
export template <typename Info>
class bad_result
  : public std::runtime_error
  , public Info {
public:
    bad_result(Info&& info) noexcept
      : std::runtime_error("bad operation result")
      , Info(std::move(info)) {}
};
//------------------------------------------------------------------------------
/// @brief Enumeration of C-API function call result validity.
/// @ingroup c_api_wrap
/// @see result
export enum class result_validity {
    /// @brief Result is always valid.
    always,
    /// @brief Result is conditionally valid.
    maybe,
    /// @brief Result is never valid.
    never
};

/// @brief Class wrapping the result of a C-API function call.
/// @ingroup c_api_wrap
export template <
  typename Result,
  typename Info,
  result_validity = result_validity::always>
class result;

/// @brief Alias for conditionally-valid result of a C-API function call.
/// @ingroup c_api_wrap
///
/// Such result may be returned from wrappers of C-API functions that have not
/// been successfully dynamically-loaded  in the current process.
export template <typename Result, typename Info>
using opt_result = result<Result, Info, result_validity::maybe>;

/// @brief Alias for always-invalid result of a C-API function call.
/// @ingroup c_api_wrap
///
/// Such result may be returned from wrappers of C-API functions that are not
/// available in the current build. For example API extension functions.
export template <typename Result, typename Info>
using no_result = result<Result, Info, result_validity::never>;
//------------------------------------------------------------------------------
// no result
//------------------------------------------------------------------------------
export template <typename Result, typename Info>
class result<Result, Info, result_validity::never> : public Info {
public:
    using value_type = Result;

    constexpr result() noexcept = default;

    constexpr result(Info info) noexcept
      : Info{std::move(info)} {}

    constexpr auto has_value() const noexcept -> bool {
        return false;
    }

    explicit constexpr operator bool() const noexcept {
        return false;
    }

    template <std::derived_from<Result> Dest>
    auto operator>>(Dest& dest) -> Dest& {
        throw bad_result<Info>(std::move(static_cast<Info*>(this)));
        return dest;
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> Result {
        assert(has_value());
        return {};
    }

    [[nodiscard]] constexpr auto value() const noexcept -> Result {
        assert(has_value());
        return {};
    }

    template <std::convertible_to<Result> U>
    [[nodiscard]] auto value_or(U&& fallback) const noexcept -> Result {
        return Result(std::forward<U>(fallback));
    }

    [[nodiscard]] auto or_default() const noexcept -> Result {
        return Result{};
    }

    [[nodiscard]] auto or_true() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        return true;
    }

    [[nodiscard]] auto or_false() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        return false;
    }

    template <
      typename F,
      optional_like R =
        std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>
    auto and_then(F&&) const& -> R {
        return R{};
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, Result&&>>>
    auto and_then(F&&) && -> R {
        return R{};
    }

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>)
    void and_then(F&&) const& {}

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, Result &&>>>)
    void and_then(F&&) && {}

    template <typename T>
    auto replaced_with(const T&) const
      -> result<T, Info, result_validity::never> {
        return {*this};
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const
      -> result<T, Info, result_validity::never> {
        return {*this};
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename F>
    auto transform(const F&) const& -> result<
      std::invoke_result_t<F, const Result&>,
      Info,
      result_validity::never> {
        return {*this};
    }

    template <typename F>
    auto transform(const F&) && -> result<
      std::invoke_result_t<F, Result&&>,
      Info,
      result_validity::never> {
        return {*this};
    }

    template <typename F>
    auto transform_if(const F&) const -> result<
      std::invoke_result_t<F, const Result&, bool>,
      Info,
      result_validity::never> {
        return {*this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(const Check&, IfFalse if_false) noexcept
      -> result<void, Info, result_validity::never> {
        if_false(*static_cast<Info*>(this));
        return {*this};
    }
};
//------------------------------------------------------------------------------
export template <typename Info>
class result<void, Info, result_validity::never> : public Info {
public:
    using value_type = nothing_t;

    constexpr result() noexcept = default;

    constexpr result(Info info) noexcept
      : Info{std::move(info)} {}

    constexpr auto has_value() const noexcept -> bool {
        return false;
    }

    explicit constexpr operator bool() const noexcept {
        return false;
    }

    constexpr auto operator*() const noexcept -> const nothing_t& {
        assert(has_value());
        return nothing;
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto and_then(F&&) const -> R {
        return R{};
    }

    template <typename F>
        requires(std::is_void_v<std::remove_cvref_t<std::invoke_result_t<F>>>)
    void and_then(F&&) const {}

    template <typename T>
    auto replaced_with(const T&) const noexcept
      -> result<T, Info, result_validity::never> {
        return {*this};
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const noexcept
      -> result<T, Info, result_validity::never> {
        return {*this};
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename F>
    auto transform(const F&) const
      -> result<std::invoke_result_t<F, nothing_t>, Info, result_validity::never> {
        return {*this};
    }

    template <typename F>
    auto transform_if(const F&) const -> result<
      std::invoke_result_t<F, nothing_t, bool>,
      Info,
      result_validity::never> {
        return {*this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(const Check&, IfFalse if_false) noexcept -> auto& {
        if_false(*static_cast<Info*>(this));
        return *this;
    }
};
//------------------------------------------------------------------------------
// result
//------------------------------------------------------------------------------
/// @brief Specialization of result for always-valid result values.
/// @ingroup c_api_wrap
///
/// The other specializations (@c maybe, @c never) have the same API as this one.
export template <typename Result, typename Info>
class result<Result, Info, result_validity::always> : public Info {

public:
    using value_type = Result;

    constexpr result() noexcept = default;

    constexpr result(Result value) noexcept
      : _value{std::move(value)} {}

    constexpr result(Result value, Info info) noexcept
      : Info{std::move(info)}
      , _value{std::move(value)} {}

    /// @brief Indicates if the result value is valid.
    constexpr auto has_value() const noexcept -> bool {
        return bool(*static_cast<const Info*>(this));
    }

    /// @brief Tests if this result is valid and contains a value.
    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    /// @brief Extraction operator for result value.
    /// @ingroup c_api_wrap
    ///
    /// Extracts the value from this result
    template <std::derived_from<Result> Dest>
    auto operator>>(Dest& dest) -> Dest& {
        return dest = std::move(_value);
    }

    /// @brief Returns a reference the stored value.
    /// @pre has_value()
    [[nodiscard]] constexpr auto operator*() & noexcept -> Result& {
        return _value;
    }

    /// @brief Returns a rvalue reference the stored value.
    /// @pre has_value()
    [[nodiscard]] constexpr auto operator*() && noexcept -> Result&& {
        return std::move(_value);
    }

    /// @brief Returns a const reference the stored value.
    /// @pre has_value()
    [[nodiscard]] constexpr auto operator*() const& noexcept -> const Result& {
        return _value;
    }

    /// @brief Returns the stored value.
    /// @pre has_value()
    /// @see value_or
    [[nodiscard]] constexpr auto value() const noexcept -> const Result& {
        return _value;
    }

    /// @brief Returns the stored value or a fallback.
    /// @see value
    /// @see or_default
    template <std::convertible_to<Result> U>
    [[nodiscard]] auto value_or(U&&) const& noexcept -> Result {
        return _value;
    }

    template <std::convertible_to<Result> U>
    [[nodiscard]] auto value_or(U&&) && noexcept -> Result {
        return std::move(_value);
    }

    /// @brief Returns the stored value or a default constructed value.
    /// @see value
    /// @see value_or
    [[nodiscard]] auto or_default() const& noexcept -> Result {
        return _value;
    }

    [[nodiscard]] auto or_default() && noexcept -> Result {
        return std::move(_value);
    }

    /// @brief Converts the stored value to boolean or returns true.
    /// @see or_false
    /// @see or_default
    [[nodiscard]] auto or_true() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        return bool(_value);
    }

    /// @brief Converts the stored value to boolean or returns false.
    /// @see or_true
    /// @see or_default
    [[nodiscard]] auto or_false() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        return bool(_value);
    }

    template <
      typename F,
      optional_like R =
        std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>
    auto and_then(F&& function) const& -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), _value);
        } else {
            return R{};
        }
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, Result&&>>>
    auto and_then(F&& function) && -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), std::move(_value));
        } else {
            return R{};
        }
    }

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>)
    void and_then(F&& function) const& {
        if(has_value()) {
            std::invoke(std::forward<F>(function), _value);
        }
    }

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, Result &&>>>)
    void and_then(F&& function) && {
        if(has_value()) {
            std::invoke(std::forward<F>(function), std::move(_value));
        }
    }

    /// @brief Returns a transformed result with a new stored value.
    template <typename T>
    auto replaced_with(T value) const
      -> result<T, Info, result_validity::always> {
        return {std::move(value), *this};
    }

    /// @brief Returns an result with the stored value cast to different type.
    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return replaced_with(T(_value));
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    /// @brief Returns an result with the value transformed by the specified function.
    template <typename F>
    auto transform(F&& function) const& -> result<
      std::invoke_result_t<F, const Result&>,
      Info,
      result_validity::always> {
        return {std::invoke(std::forward<F>(function), _value), *this};
    }

    template <typename F>
    auto transform(F&& function) && -> result<
      std::invoke_result_t<F, Result&&>,
      Info,
      result_validity::always> {
        return {
          std::invoke(std::forward<F>(function), std::move(_value)), *this};
    }

    /// @brief Returns an result with the value transformed by the specified function.
    template <typename F>
    auto transform_if(F&& function) const -> result<
      std::invoke_result_t<F, const Result&, bool>,
      Info,
      result_validity::always> {
        return {std::invoke(std::forward<F>(function), _value, true), *this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(Check check, IfFalse if_false)
      -> result<void, Info, result_validity::always> {
        if(not check(_value)) [[unlikely]] {
            if_false(*static_cast<Info*>(this));
        }
        return {*this};
    }

public:
    Result _value{};
};
//------------------------------------------------------------------------------
export template <typename Info>
class result<void, Info, result_validity::always> : public Info {

public:
    using value_type = nothing_t;

    constexpr result() noexcept = default;

    constexpr result(Info info) noexcept
      : Info{std::move(info)} {}

    constexpr auto has_value() const noexcept -> bool {
        return bool(*static_cast<const Info*>(this));
    }

    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    constexpr auto operator*() const noexcept -> const nothing_t& {
        assert(has_value());
        return nothing;
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto and_then(F&& function) const -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function));
        } else {
            return R{};
        }
    }

    template <typename F>
        requires(std::is_void_v<std::remove_cvref_t<std::invoke_result_t<F>>>)
    void and_then(F&& function) const {
        if(has_value()) {
            std::invoke(std::forward<F>(function));
        }
    }

    template <typename T>
    auto replaced_with(T value) const
      -> result<T, Info, result_validity::always> {
        return {std::move(value), *this};
    }

    /// @brief Returns an result with the stored value cast to different type.
    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return replaced_with(T{});
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename F>
    auto transform(F&& function) const
      -> result<std::invoke_result_t<F, nothing_t>, Info, result_validity::always> {
        return {std::invoke(std::forward<F>(function), nothing), *this};
    }

    template <typename F>
    auto transform_if(F&& function) const -> result<
      std::invoke_result_t<F, nothing_t, bool>,
      Info,
      result_validity::always> {
        return {std::invoke(std::forward<F>(function), nothing, true), *this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(const Check& check, const IfFalse&) const -> auto& {
        return *this;
    }
};
//------------------------------------------------------------------------------
// optional result
//------------------------------------------------------------------------------
export template <typename Result, typename Info>
class result<Result, Info, result_validity::maybe> : public Info {

public:
    using value_type = Result;

    constexpr result() noexcept = default;

    constexpr result(Result value, const bool valid) noexcept
      : _value{std::move(value)}
      , _valid{valid} {}

    constexpr result(Result value, const bool valid, Info info) noexcept
      : Info{std::move(info)}
      , _value{std::move(value)}
      , _valid{valid} {}

    constexpr auto has_value() const noexcept -> bool {
        return _valid and bool(*static_cast<const Info*>(this));
    }

    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    template <std::derived_from<Result> Dest>
    auto operator>>(Dest& dest) -> Dest& {
        if(not has_value()) {
            throw bad_result<Info>(static_cast<Info&&>(*this));
        }
        return dest = std::move(_value);
    }

    [[nodiscard]] constexpr auto operator*() & noexcept -> Result& {
        assert(has_value());
        return _value;
    }

    [[nodiscard]] constexpr auto operator*() && noexcept -> Result&& {
        assert(has_value());
        return std::move(_value);
    }

    [[nodiscard]] constexpr auto operator*() const& noexcept -> const Result& {
        assert(has_value());
        return _value;
    }

    [[nodiscard]] constexpr auto value() const noexcept -> const Result& {
        assert(has_value());
        return _value;
    }

    template <std::convertible_to<Result> U>
    [[nodiscard]] auto value_or(U&& fallback) const& noexcept -> Result {
        if(has_value()) {
            return _value;
        }
        return Result(std::forward<U>(fallback));
    }

    template <std::convertible_to<Result> U>
    [[nodiscard]] auto value_or(U&& fallback) && noexcept -> Result {
        if(has_value()) {
            return std::move(_value);
        }
        return Result(std::forward<U>(fallback));
    }

    [[nodiscard]] auto or_default() const& noexcept -> Result {
        if(has_value()) {
            return _value;
        }
        return Result{};
    }

    [[nodiscard]] auto or_default() && noexcept -> Result {
        if(has_value()) {
            return std::move(_value);
        }
        return Result{};
    }

    [[nodiscard]] auto or_true() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        if(has_value()) {
            return bool(_value);
        }
        return true;
    }

    [[nodiscard]] auto or_false() const noexcept -> bool
        requires(std::convertible_to<Result, bool>)
    {
        if(has_value()) {
            return bool(_value);
        }
        return false;
    }

    template <
      typename F,
      optional_like R =
        std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>
    auto and_then(F&& function) const& -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), _value);
        } else {
            return R{};
        }
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, Result&&>>>
    auto and_then(F&& function) && -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), std::move(_value));
        } else {
            return R{};
        }
    }

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, const Result&>>>)
    void and_then(F&& function) const& {
        if(has_value()) {
            std::invoke(std::forward<F>(function), _value);
        }
    }

    template <typename F>
        requires(std::is_void_v<
                 std::remove_cvref_t<std::invoke_result_t<F, Result &&>>>)
    void and_then(F&& function) && {
        if(has_value()) {
            std::invoke(std::forward<F>(function), std::move(_value));
        }
    }

    template <typename T>
    auto replaced_with(T value) const
      -> result<T, Info, result_validity::maybe> {
        return {std::move(value), has_value(), *this};
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return replaced_with(T(_value));
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename F>
    auto transform(F&& function) const& -> result<
      std::invoke_result_t<F, const Result&>,
      Info,
      result_validity::maybe> {
        if(has_value()) {
            return {
              std::invoke(std::forward<F>(function), _value), true, *this};
        }
        return {};
    }

    template <typename F>
    auto transform(F&& function) && -> result<
      std::invoke_result_t<F, Result&&>,
      Info,
      result_validity::maybe> {
        if(has_value()) {
            return {
              std::invoke(std::forward<F>(function), std::move(_value)),
              true,
              *this};
        }
        return {};
    }

    template <typename F>
    auto transform_if(F&& function) const -> result<
      std::invoke_result_t<F, const Result&, bool>,
      Info,
      result_validity::maybe> {
        return {
          std::invoke(std::forward<F>(function), _value, has_value()),
          has_value(),
          *this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(Check check, IfFalse if_false)
      -> result<void, Info, result_validity::always> {
        if(not check(_value)) [[unlikely]] {
            if_false(*static_cast<Info*>(this));
        }
        return {*this};
    }

public: // TODO
    Result _value{};
    bool _valid{false};
};
//------------------------------------------------------------------------------
export template <typename Info>
class result<void, Info, result_validity::maybe> : public Info {

public:
    using value_type = nothing_t;

    constexpr result() noexcept = default;

    constexpr result(const bool valid) noexcept
      : _valid{valid} {}

    constexpr result(const bool valid, Info info) noexcept
      : Info{std::move(info)}
      , _valid{valid} {}

    constexpr auto has_value() const noexcept -> bool {
        return _valid;
    }

    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    constexpr auto operator*() const noexcept -> const nothing_t& {
        assert(has_value());
        return nothing;
    }

    [[nodiscard]] constexpr auto value() const noexcept -> nothing_t {
        assert(has_value());
        return {};
    }

    template <typename U>
    [[nodiscard]] auto value_or(U value) const noexcept -> U {
        return value;
    }

    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F>>>
    auto and_then(F&& function) const -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function));
        } else {
            return R{};
        }
    }

    template <typename F>
        requires(std::is_void_v<std::remove_cvref_t<std::invoke_result_t<F>>>)
    void and_then(F&& function) const {
        if(has_value()) {
            std::invoke(std::forward<F>(function));
        }
    }

    template <typename T>
    auto replaced_with(T value) const
      -> result<T, Info, result_validity::maybe> {
        return {std::move(value), has_value(), *this};
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return replaced_with(T{});
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename F>
    auto transform(F&& function) const
      -> result<std::invoke_result_t<F, nothing_t>, Info, result_validity::maybe> {
        if(has_value()) {
            return {
              std::invoke(std::forward<F>(function), nothing), true, *this};
        }
        return {};
    }

    template <typename F>
    auto transform_if(F&& function) const -> result<
      std::invoke_result_t<F, nothing_t, bool>,
      Info,
      result_validity::maybe> {
        return {
          std::invoke(std::forward<F>(function), nothing, has_value()),
          has_value(),
          *this};
    }

    template <typename Check, typename IfFalse>
    auto collapsed(const Check&, const IfFalse&) const -> auto& {
        return *this;
    }

private:
    bool _valid{false};
};
//------------------------------------------------------------------------------
// combined_result
//------------------------------------------------------------------------------
export template <typename Result, typename Info>
class combined_result : public result<Result, Info, result_validity::maybe> {
    using base = result<Result, Info, result_validity::maybe>;

public:
    using value_type = typename base::value_type;

    constexpr combined_result() noexcept = default;

    template <typename SrcInfo, result_validity validity>
    combined_result(result<Result, SrcInfo, validity> src)
      : base{std::move(src)} {}
};
//------------------------------------------------------------------------------
} // namespace c_api

export template <typename Result, typename Info, c_api::result_validity validity>
struct ok_traits<c_api::result<Result, Info, validity>> {
    static constexpr auto nok_info(
      const c_api::result<Result, Info, validity>& r) noexcept -> const Info& {
        return r;
    }
};

export template <typename Result, typename Info>
struct ok_traits<c_api::combined_result<Result, Info>> {
    static constexpr auto nok_info(
      const c_api::combined_result<Result, Info>& r) noexcept -> const Info& {
        return r;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
