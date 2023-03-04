/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:substitution;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import :c_str;
import std;

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
  const variable_substitution_options opts) noexcept -> std::string& {
    do {
        if(const auto found{find_position(src, name)}) {
            const auto npos{extract(found)};
            if(
              (npos >= 2) and (safe_add_lt(npos, name.size(), src.size())) and
              (src[npos - 1] == opts.opening_bracket) and
              (src[npos - 2] == opts.leading_sign) and
              (src[npos + name.size()] == opts.closing_bracket)) {
                append_to(head(src, npos - 2), dst);
                append_to(value, dst);
                src = skip(src, safe_add(npos, name.size(), 1));
            } else {
                append_to(head(src, safe_add(npos, name.size())), dst);
                src = skip(src, safe_add(npos, name.size()));
            }
        } else {
            append_to(src, dst);
            src.reset();
        }
    } while(not src.empty());
    return dst;
}
//------------------------------------------------------------------------------
auto substitute_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options opts) noexcept -> std::string& {

    do {
        if(const auto lpos{find_element(src, opts.leading_sign)}) {
            append_to(head(src, lpos.value()), dst);
            src = skip(src, lpos.value());

            if(const auto inner{slice_inside_brackets(
                 src, opts.opening_bracket, opts.closing_bracket)}) {

                if(const auto translation{translate(inner)}) {
                    append_to(translation.value(), dst);
                } else {
                    if(find_element(inner, opts.leading_sign)) {
                        std::string temp;
                        substitute_variables_into(temp, inner, translate, opts);
                        if(const auto translation2 = translate(temp)) {
                            append_to(translation2.value(), dst);
                        } else if(opts.keep_untranslated) {
                            append_to(
                              head(src, safe_add(inner.size(), 3)), dst);
                        }
                    } else if(opts.keep_untranslated) {
                        append_to(head(src, safe_add(inner.size(), 3)), dst);
                    }
                }
                src = skip(src, safe_add(inner.size(), 3));
            } else {
                append_to(head(src, 3), dst);
                src = skip(src, 3);
            }

        } else {
            append_to(src, dst);
            src.reset();
        }
    } while(not src.empty());

    return dst;
}
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
  const variable_substitution_options opts = {}) noexcept -> std::string {
    const auto translate_func =
      [&strings](string_view key) -> std::optional<string_view> {
        char* e = nullptr; // NOLINT(hicpp-vararg)
        const span_size_t idx = span_size(std::strtol(c_str(key), &e, 10));
        if((0 < idx) and (idx <= strings.size())) {
            return {{strings[idx - 1]}};
        }
        return {};
    };
    return substitute_variables(str, {construct_from, translate_func}, opts);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto substitute_variables(
  const std::string& str,
  const std::map<std::string, std::string, str_view_less>& dictionary,
  const variable_substitution_options opts) noexcept -> std::string {
    const auto translate_func =
      [&dictionary](string_view key) -> std::optional<string_view> {
        if(not dictionary.empty()) {
            auto i = dictionary.find(key);
            if(i != dictionary.end()) {
                return {i->second};
            }
        }
        return {};
    };
    return substitute_variables(str, {construct_from, translate_func}, opts);
}
//------------------------------------------------------------------------------
} // namespace eagine
