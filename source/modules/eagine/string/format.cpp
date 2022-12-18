/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:format;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import :substitution;
import <array>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
class format_string_and_list_base {

protected:
    format_string_and_list_base(std::string fmt_str) noexcept
      : _fmt_str{std::move(fmt_str)} {}

    format_string_and_list_base(
      const construct_from_t,
      format_string_and_list_base& that) noexcept
      : _fmt_str{std::move(that._fmt_str)} {}

    auto _fmt(span<const std::string> values) const noexcept -> std::string {
        return substitute_variables(_fmt_str, values);
    }

private:
    std::string _fmt_str{};
};
//------------------------------------------------------------------------------
export template <span_size_t N>
class format_string_and_list;

export template <>
class format_string_and_list<0> : public format_string_and_list_base {
public:
    format_string_and_list(std::string& fmt_str) noexcept
      : format_string_and_list_base{std::move(fmt_str)} {}

    [[nodiscard]] operator std::string() const noexcept {
        return _fmt({});
    }
};
//------------------------------------------------------------------------------
/// @brief Helper class used in implementation of string variable substitution.
/// @ingroup string_utils
/// @see format
export template <span_size_t N>
class format_string_and_list : public format_string_and_list_base {

    template <std::size_t... I>
    format_string_and_list(
      format_string_and_list<N - 1>&& prev,
      std::string&& val,
      std::index_sequence<I...>) noexcept
      : format_string_and_list_base{construct_from, prev}
      , _list{{std::move(prev._list[I])..., std::move(val)}} {}

public:
    format_string_and_list(
      format_string_and_list<N - 1>&& prev,
      std::string&& val) noexcept
      : format_string_and_list{
          std::move(prev),
          std::move(val),
          std::make_index_sequence<N - 1>()} {}

    /// @brief Implicit conversion to a string with the variable substituted.
    [[nodiscard]] operator std::string() const noexcept {
        return _fmt(view(_list));
    }

public:
    std::array<std::string, N> _list{};
};
//------------------------------------------------------------------------------
export template <>
class format_string_and_list<1> : public format_string_and_list_base {

public:
    format_string_and_list(
      format_string_and_list<0>&& prev,
      std::string&& val) noexcept
      : format_string_and_list_base{construct_from, prev}
      , _list{{val}} {}

    [[nodiscard]] operator std::string() const noexcept {
        return _fmt(view(_list));
    }

public:
    std::array<std::string, 1> _list{};
};
//------------------------------------------------------------------------------
export template <span_size_t N>
auto operator<<(std::ostream& out, const format_string_and_list<N>& fsl)
  -> std::ostream& {
    return out << std::string(fsl);
}
//------------------------------------------------------------------------------
/// @brief Operator for adding variable name and value for string formatting.
/// @ingroup string_utils
/// @see format
export template <span_size_t N>
[[nodiscard]] auto operator%(
  format_string_and_list<N>&& fsal,
  std::string&& val) noexcept -> format_string_and_list<N + 1> {
    return {std::move(fsal), std::move(val)};
}
//------------------------------------------------------------------------------
/// @brief Takes a format string, returns an object for variable specification.
/// @ingroup string_utils
export [[nodiscard]] auto format(std::string&& fmt_str) noexcept
  -> format_string_and_list<0> {
    return {fmt_str};
}
//------------------------------------------------------------------------------
} // namespace eagine
