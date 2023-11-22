/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:format;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import :c_str;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Options for string variable substitution customization.
/// @ingroup string_utils
export struct variable_substitution_options {
    /// @brief Keep the untranslated variable references in the format string.
    bool keep_untranslated{false};

    /// @brief The variable reference leading sign.
    char leading_sign{'$'};

    /// @brief The variable reference opening (left) bracket.
    char opening_bracket{'{'};

    /// @brief The variable reference closing (right) bracket.
    char closing_bracket{'}'};
};
//------------------------------------------------------------------------------
/// @brief Substitutes variable value from src into dst.
/// @ingroup string_utils
/// @see substitute_variables
///
/// This function takes the format string @p src, finds variable reference
/// using substitution options and substitutes the variable with the specified
/// @p value into @p dst.
export auto substitute_variable_into(
  std::string& dst,
  string_view src,
  const string_view name,
  const string_view value,
  const variable_substitution_options = {}) noexcept -> std::string&;
//------------------------------------------------------------------------------
/// @brief Substitutes variable values by using translate, from src into dst.
/// @ingroup string_utils
/// @see substitute_variables
///
/// This function takes the format string @p src, finds variable references
/// using substitution options and substitutes the variables with their values
/// using the translation function into @p dst.
export auto substitute_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options = {}) noexcept -> std::string&;

export auto substitute_str_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<std::string>(const string_view) noexcept>&
    translate,
  const variable_substitution_options = {}) noexcept -> std::string&;
//------------------------------------------------------------------------------
/// @brief Substitutes variable values by using translate, from src.
/// @ingroup string_utils
/// @see substitute_variables_into
///
/// This function takes the format string @p src, finds variable references
/// using substitution options and substitutes the variables with their values
/// using the translation function and returns the resulting string.
export [[nodiscard]] auto substitute_variables(
  const string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options = {}) noexcept -> std::string;
//------------------------------------------------------------------------------
/// @brief Substitutes variable values by using translate, from src.
/// @ingroup string_utils
/// @see substitute_variables_into
///
/// This function takes the format string @p src, finds variable references
/// using substitution options and substitutes the variables with their values
/// using the dictionary map and returns the resulting string.
export [[nodiscard]] auto substitute_variables(
  const std::string& str,
  const std::map<std::string, std::string, str_view_less>& dictionary,
  const variable_substitution_options = {}) noexcept -> std::string;
//------------------------------------------------------------------------------
/// @brief Class storing a map of variable names to values, doing string substitution.
/// @ingroup string_utils
export class string_variable_map {

public:
    /// @brief Set the @p value of a variable with @p name.
    auto set(std::string name, std::string value) noexcept
      -> string_variable_map& {
        _dict.emplace(std::move(name), std::move(value));
        return *this;
    }

    /// @brief Uses the stored variables to do substitution in the given string.
    auto subst_variables(const std::string& str) const noexcept -> std::string {
        return substitute_variables(str, _dict);
    }

    /// @brief Uses the stored variables to do substitution in the given string.
    [[nodiscard]] auto operator()(const std::string& str) const noexcept
      -> std::string {
        return substitute_variables(str, _dict);
    }

private:
    std::map<std::string, std::string, str_view_less> _dict{};
};
//------------------------------------------------------------------------------
auto substitute_variable_into(
  std::string& dst,
  string_view src,
  const string_view name,
  const string_view value,
  const variable_substitution_options opts) noexcept -> std::string&;
//------------------------------------------------------------------------------
auto substitute_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options opts) noexcept -> std::string&;
//------------------------------------------------------------------------------
export [[nodiscard]] auto substitute_variables(
  const string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options opts) noexcept -> std::string {
    std::string result;
    return std::move(substitute_variables_into(result, src, translate, opts));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto substitute_variables(
  const std::string& str,
  const span<const std::string> strings,
  const variable_substitution_options opts = {}) noexcept -> std::string;
//------------------------------------------------------------------------------
export [[nodiscard]] auto substitute_variables_into(
  std::string& dest,
  const string_view str,
  const span<const std::string> strings,
  const variable_substitution_options opts = {}) noexcept -> std::string&;
//------------------------------------------------------------------------------
export [[nodiscard]] auto substitute_variables(
  const std::string& str,
  const std::map<std::string, std::string, str_view_less>& dictionary,
  const variable_substitution_options opts) noexcept -> std::string;
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

    auto _fmt_into(std::string& dest, span<const std::string> values)
      const noexcept -> std::string& {
        return substitute_variables_into(dest, _fmt_str, values);
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

    auto into(std::string& dest) const noexcept -> std::string& {
        return _fmt_into(dest, {});
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

    /// @brief Formats the string with the variables substituted into a string.
    auto into(std::string& dest) const noexcept -> std::string& {
        return _fmt_into(dest, view(_list));
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

    auto into(std::string& dest) const noexcept -> std::string& {
        return _fmt_into(dest, view(_list));
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
