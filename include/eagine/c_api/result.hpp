/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_RESULT_HPP
#define EAGINE_C_API_RESULT_HPP

#include "../assert.hpp"
#include "../extract.hpp"
#include "../nothing.hpp"
#include "../unreachable_reference.hpp"
#include <stdexcept>
#include <type_traits>

namespace eagine {
namespace c_api {
//------------------------------------------------------------------------------
/// @brief Exception wrapping information about failed C-API function call result.
/// @ingroup c_api_wrap
template <typename Info>
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
enum class result_validity {
    /// @brief Result is always valid.
    always,
    /// @brief Result is conditionally valid.
    maybe,
    /// @brief Result is never valid.
    never
};
//------------------------------------------------------------------------------
template <typename Result, result_validity>
class result_value;

/// @brief Class wrapping the result of a C-API function call.
/// @ingroup c_api_wrap
template <typename Result, typename Info, result_validity = result_validity::always>
class result;

/// @brief Alias for conditionally-valid result of a C-API function call.
/// @ingroup c_api_wrap
///
/// Such result may be returned from wrappers of C-API functions that have not
/// been successfully dynamically-loaded  in the current process.
template <typename Result, typename Info>
using opt_result = result<Result, Info, result_validity::maybe>;

/// @brief Alias for always-invalid result of a C-API function call.
/// @ingroup c_api_wrap
///
/// Such result may be returned from wrappers of C-API functions that are not
/// available in the current build. For example API extension functions.
template <typename Result, typename Info>
using no_result = result<Result, Info, result_validity::never>;
//------------------------------------------------------------------------------
// no result
//------------------------------------------------------------------------------
template <typename Result>
class result_value<Result, result_validity::never> {
public:
    constexpr result_value() noexcept = default;

protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<Result, Info, result_validity::never>& src,
      std::type_identity<T>) const {
        result<T, Info, result_validity::never> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<Result, Info, result_validity::never>& src,
      Transform& transform) const {
        using T = decltype(transform(std::declval<Result>(), false));
        result<T, Info, result_validity::never> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<Result, Info, result_validity::never>& src,
      Check&,
      IfFalse&) const {
        result<void, Info, result_validity::never> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }
};

template <>
class result_value<void, result_validity::never> {
public:
protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<void, Info, result_validity::never>& src,
      const std::type_identity<T>) const {
        result<T, Info, result_validity::never> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<void, Info, result_validity::never>& src,
      Transform& transform) const {
        using T = decltype(transform(nothing, false));
        result<T, Info, result_validity::never> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<void, Info, result_validity::never>& src,
      Check&,
      IfFalse&) const -> auto& {
        return src;
    }
};
//------------------------------------------------------------------------------
template <typename Result, typename Info>
class result<Result, Info, result_validity::never>
  : public Info
  , public result_value<Result, result_validity::never> {
    using base = result_value<Result, result_validity::never>;

public:
    using base::base;

    explicit constexpr operator bool() const noexcept {
        return false;
    }

    template <typename T>
    auto replaced_with(const T&) const {
        result<T, Info, result_validity::never> res{};
        static_cast<Info&>(res) = static_cast<const Info&>(*this);
        return res;
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return this->_cast_to(*this, tid);
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename Transform>
    auto transformed(Transform transform) const {
        return this->_transformed(*this, transform);
    }

    template <typename Check, typename IfFalse>
    auto collapsed(Check check, IfFalse if_false) const {
        return this->_collapsed(*this, check, if_false);
    }
};
//------------------------------------------------------------------------------
// result
//------------------------------------------------------------------------------
/// @brief Specialization for always-valid result value.
/// @ingroup c_api_wrap
template <typename Result>
class result_value<Result, result_validity::always> {
public:
    constexpr result_value() noexcept(noexcept(Result{})) = default;

    constexpr result_value(Result value) noexcept
      : _value{std::move(value)} {}

    /// @brief Indicates if the result value is valid.
    /// @returns true
    constexpr auto is_valid() const noexcept {
        return true;
    }

protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<Result, Info, result_validity::always>& src,
      const std::type_identity<T>) const {
        result<T, Info, result_validity::always> result{T(_value)};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<Result, Info, result_validity::always>& src,
      Transform& transform) const {
        using T = decltype(transform(std::declval<Result>(), true));
        result<T, Info, result_validity::always> result{
          transform(_value, true)};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<Result, Info, result_validity::always>& src,
      Check& check,
      IfFalse& if_false) const {
        result<void, Info, result_validity::always> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        if(!check(_value)) [[unlikely]] {
            if_false(static_cast<Info&>(result));
        }
        return result;
    }

public:
    friend class result_value<Result, result_validity::maybe>;
    Result _value{};
};
//------------------------------------------------------------------------------
template <>
class result_value<void, result_validity::always> {
public:
    constexpr auto is_valid() const noexcept {
        return true;
    }

protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<void, Info, result_validity::always>& src,
      const std::type_identity<T>) const {
        result<T, Info, result_validity::always> result{};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<void, Info, result_validity::always>& src,
      Transform& transform) const {
        using T = decltype(transform(nothing, true));
        result<T, Info, result_validity::always> result{
          transform(nothing, true)};
        static_cast<Info&>(result) = static_cast<const Info&>(src);
        return result;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<void, Info, result_validity::always>& src,
      Check&,
      IfFalse&) const -> auto& {
        return src;
    }
};
//------------------------------------------------------------------------------
/// @brief Specialization of result for always-valid result values.
/// @ingroup c_api_wrap
///
/// The other specializations (@c maybe, @c never) has the same API as this one.
template <typename Result, typename Info>
class result<Result, Info, result_validity::always>
  : public Info
  , public result_value<Result, result_validity::always> {
    using base = result_value<Result, result_validity::always>;

public:
    using base::base;

    /// @brief Tests if this result is valid and contains an extractable value.
    explicit constexpr operator bool() const noexcept {
        return bool(*static_cast<const Info*>(this));
    }

    /// @brief Returns a transformed result with a new stored value.
    template <typename T>
    auto replaced_with(T value) const {
        result<T, Info, result_validity::always> res{std::move(value)};
        static_cast<Info&>(res) = static_cast<const Info&>(*this);
        return res;
    }

    /// @brief Returns an result with the stored value cast to different type.
    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return this->_cast_to(*this, tid);
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    /// @brief Returns an result with the value transformed by the specified function.
    template <typename Transform>
    auto transformed(Transform transform) const {
        return this->_transformed(*this, transform);
    }

    template <typename Check, typename IfFalse>
    auto collapsed(Check check, IfFalse if_false) const {
        return this->_collapsed(*this, check, if_false);
    }
};
//------------------------------------------------------------------------------
/// @brief Specialization for conditionally-valid result value.
/// @ingroup c_api_wrap
template <typename Result>
class result_value<Result, result_validity::maybe> {
public:
    constexpr result_value() noexcept = default;

    constexpr result_value(Result value, const bool valid) noexcept
      : _value{std::move(value)}
      , _valid{valid} {}

    constexpr result_value(
      const result_value<Result, result_validity::never>&) noexcept {}

    constexpr result_value(
      result_value<Result, result_validity::always> that) noexcept
      : _value{std::move(that._value)}
      , _valid{true} {}

    /// @brief Indicates if the result value is valid.
    constexpr auto is_valid() const noexcept {
        return _valid;
    }

protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<Result, Info, result_validity::maybe>& src,
      const std::type_identity<T>) const {
        result<T, Info, result_validity::maybe> res{T(_value), src.is_valid()};
        static_cast<Info&>(res) = static_cast<const Info&>(src);
        return res;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<Result, Info, result_validity::maybe>& src,
      Transform& transform) const {
        using T = decltype(transform(std::declval<Result>(), true));
        result<T, Info, result_validity::maybe> res{
          transform(_value, src.is_valid()), src.is_valid()};
        static_cast<Info&>(res) = static_cast<const Info&>(src);
        return res;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<Result, Info, result_validity::maybe>& src,
      Check& check,
      IfFalse& if_false) const {
        result<void, Info, result_validity::maybe> res{};
        static_cast<Info&>(res) = static_cast<const Info&>(src);
        if(src.is_valid() && !check(_value)) [[unlikely]] {
            if_false(static_cast<Info&>(res));
        }
        return res;
    }

public:
    Result _value{};
    bool _valid{false};
};
//------------------------------------------------------------------------------
template <>
class result_value<void, result_validity::maybe> {
public:
    constexpr result_value() noexcept = default;

    constexpr result_value(const bool valid) noexcept
      : _valid{valid} {}

    constexpr result_value(
      const result_value<void, result_validity::never>&) noexcept {}

    constexpr result_value(
      const result_value<void, result_validity::always>&) noexcept
      : _valid{true} {}

    constexpr auto is_valid() const noexcept {
        return _valid;
    }

protected:
    template <typename Info, typename T>
    auto _cast_to(
      const result<void, Info, result_validity::maybe>& src,
      const std::type_identity<T>) const {
        result<T, Info, result_validity::maybe> res{T{}, src.is_valid()};
        static_cast<Info&>(res) = static_cast<const Info&>(src);
        return res;
    }

    template <typename Info, typename Transform>
    auto _transformed(
      const result<void, Info, result_validity::maybe>& src,
      Transform& transform) const {
        using T = decltype(transform(nothing, true));
        result<T, Info, result_validity::maybe> res{
          transform(nothing, src.is_valid()), src.is_valid()};
        static_cast<Info&>(res) = static_cast<const Info&>(src);
        return res;
    }

    template <typename Info, typename Check, typename IfFalse>
    auto _collapsed(
      const result<void, Info, result_validity::maybe>& src,
      Check&,
      IfFalse&) const -> auto& {
        return src;
    }

public:
    bool _valid{false};
};
//------------------------------------------------------------------------------
template <typename Result, typename Info>
class result<Result, Info, result_validity::maybe>
  : public Info
  , public result_value<Result, result_validity::maybe> {
    using base = result_value<Result, result_validity::maybe>;

public:
    using base::base;

    explicit constexpr operator bool() const noexcept {
        return this->is_valid() && bool(*static_cast<const Info*>(this));
    }

    template <typename T>
    auto replaced_with(T value) const {
        result<T, Info, result_validity::maybe> res{
          std::move(value), this->is_valid()};
        static_cast<Info&>(res) = static_cast<const Info&>(*this);
        return res;
    }

    template <typename T>
    auto cast_to(const std::type_identity<T> tid) const {
        return this->_cast_to(*this, tid);
    }

    auto cast_to(const std::type_identity<void>) const noexcept -> auto& {
        return *this;
    }

    auto cast_to(const std::type_identity<nothing_t>) const noexcept -> auto& {
        return *this;
    }

    template <typename Transform>
    auto transformed(Transform transform) const {
        return this->_transformed(*this, transform);
    }

    template <typename Check, typename IfFalse>
    auto collapsed(Check check, IfFalse if_false) const {
        return this->_collapsed(*this, check, if_false);
    }
};
//------------------------------------------------------------------------------
/// @brief Overload of extract for result_value.
/// @ingroup c_api_wrap
template <typename Result>
static constexpr auto extract(
  result_value<Result, result_validity::never>&) noexcept -> Result& {
    return unreachable_reference(std::type_identity<Result>{});
}

template <typename Result>
static constexpr auto extract(
  const result_value<Result, result_validity::never>&) noexcept
  -> const Result& {
    return unreachable_reference(std::type_identity<Result>{});
}

static constexpr auto extract(
  const result_value<void, result_validity::never>&) noexcept -> nothing_t {
    return {};
}

template <typename Result, typename Info>
inline auto operator>>(
  result<Result, Info, result_validity::never> res,
  Result& dest) -> Result& {
    throw bad_result<Info>(static_cast<Info&&>(res));
    return dest;
}
//------------------------------------------------------------------------------
// combined_result
//------------------------------------------------------------------------------
template <typename Result, typename Info>
class combined_result : public result<Result, Info, result_validity::maybe> {
    using base = result<Result, Info, result_validity::maybe>;

public:
    combined_result(result<Result, Info, result_validity::never> src)
      : base{} {
        static_cast<Info&>(*this) = static_cast<Info&&>(src);
    }

    combined_result(result<Result, Info> src)
      : base{
          extract(
            static_cast<result_value<Result, result_validity::always>&&>(src)),
          src.is_valid()} {
        static_cast<Info&>(*this) = static_cast<Info&&>(src);
    }

    combined_result(result<Result, Info, result_validity::maybe> src)
      : base{std::move(src)} {}
};
//------------------------------------------------------------------------------
template <typename Info>
class combined_result<void, Info>
  : public result<void, Info, result_validity::maybe> {
    using base = result<void, Info, result_validity::maybe>;

public:
    template <typename R>
    combined_result(const result<R, Info, result_validity::never>& src)
      : base{} {
        static_cast<Info&>(*this) = static_cast<const Info&>(src);
    }

    template <typename R>
    combined_result(const result<R, Info>& src)
      : base{src.is_valid()} {
        static_cast<Info&>(*this) = static_cast<const Info&>(src);
    }

    template <typename R>
    combined_result(const result<R, Info, result_validity::maybe>& src)
      : base{src.is_valid()} {
        static_cast<Info&>(*this) = static_cast<const Info&>(src);
    }
};
//------------------------------------------------------------------------------
template <typename Result, typename Info, result_validity validity>
static constexpr auto has_value(
  const result<Result, Info, validity>& v) noexcept {
    return bool(v);
}

template <typename Result>
static constexpr auto extract(
  result_value<Result, result_validity::always>&& res) noexcept -> Result {
    return std::move(res._value);
}

template <typename Result>
static constexpr auto extract(
  result_value<Result, result_validity::always>& res) noexcept -> Result& {
    return res._value;
}

template <typename Result>
static constexpr auto extract(
  const result_value<Result, result_validity::always>& res) noexcept
  -> const Result& {
    return res._value;
}

template <typename Result>
inline auto operator>>(
  result_value<Result, result_validity::always> res,
  Result& dest) noexcept -> Result& {
    return dest = std::move(res._value);
}

static constexpr auto extract(
  const result_value<void, result_validity::always>&) noexcept -> nothing_t {
    return {};
}

/// @brief Specialization of extract for result_value.
/// @ingroup c_api_wrap
template <typename Result>
static constexpr auto extract(
  result_value<Result, result_validity::maybe>&& res) noexcept -> Result {
    return EAGINE_CONSTEXPR_ASSERT(res._valid, std::move(res._value));
}

/// @brief Specialization of extract for result_value.
/// @ingroup c_api_wrap
template <typename Result>
static constexpr auto extract(
  result_value<Result, result_validity::maybe>& res) noexcept -> Result& {
    return EAGINE_CONSTEXPR_ASSERT(res._valid, res._value);
}

/// @brief Specialization of extract for result_value.
/// @ingroup c_api_wrap
template <typename Result>
static constexpr auto extract(
  const result_value<Result, result_validity::maybe>& res) noexcept
  -> const Result& {
    return EAGINE_CONSTEXPR_ASSERT(res._valid, res._value);
}

/// @brief Extraction operator for result_value.
/// @ingroup c_api_wrap
/// @throws bad_result<Info>
///
/// Extracts the value from an result, if the value is valid, throws otherwise.
template <typename Result, typename Info>
static inline auto operator>>(
  result<Result, Info, result_validity::maybe> res,
  Result& dest) -> Result& {
    if(!res._valid) {
        throw bad_result<Info>(static_cast<Info&&>(res));
    }
    return dest = std::move(res._value);
}

static constexpr auto extract(
  const result_value<void, result_validity::maybe>&) noexcept -> nothing_t {
    return {};
}
//------------------------------------------------------------------------------
} // namespace c_api

template <typename Result, c_api::result_validity validity>
struct extract_traits<c_api::result_value<Result, validity>> {
    using value_type = Result;
    using result_type = Result&;
    using const_result_type = std::add_const_t<Result>&;
};

template <c_api::result_validity validity>
struct extract_traits<c_api::result_value<void, validity>> {
    using value_type = void;
    using result_type = void;
    using const_result_type = void;
};

template <typename Result, typename Info, c_api::result_validity validity>
struct extract_traits<c_api::result<Result, Info, validity>> {
    using value_type = Result;
    using result_type = Result&;
    using const_result_type = std::add_const_t<Result>&;
};

template <typename Info, c_api::result_validity validity>
struct extract_traits<c_api::result<void, Info, validity>> {
    using value_type = void;
    using result_type = void;
    using const_result_type = void;
};

template <typename Result, typename Info>
struct ok_traits<c_api::result<Result, Info, c_api::result_validity::never>> {
    static constexpr auto nok_info(
      const c_api::result<Result, Info, c_api::result_validity::never>&
        r) noexcept -> const Info& {
        return r;
    }
};

template <typename Result, typename Info>
struct ok_traits<c_api::result<Result, Info, c_api::result_validity::always>> {
    static constexpr auto nok_info(
      const c_api::result<Result, Info, c_api::result_validity::always>&
        r) noexcept -> const Info& {
        return r;
    }
};

template <typename Result, typename Info>
struct ok_traits<c_api::result<Result, Info, c_api::result_validity::maybe>> {
    static constexpr auto nok_info(
      const c_api::result<Result, Info, c_api::result_validity::maybe>&
        r) noexcept -> const Info& {
        return r;
    }
};

template <typename Result, typename Info>
struct ok_traits<c_api::combined_result<Result, Info>> {
    static constexpr auto nok_info(
      const c_api::combined_result<Result, Info>& r) noexcept -> const Info& {
        return r;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif
