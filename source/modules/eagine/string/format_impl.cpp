/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.string;
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
auto substitute_variable_into(
  std::string& dst,
  string_view src,
  const string_view name,
  const string_view value,
  const variable_substitution_options opts) noexcept -> std::string& {
    do {
        if(const auto found{find_position(src, name)}) {
            const auto npos{*found};
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
template <typename Arg>
auto do_substitute_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<Arg>(const string_view) noexcept>& translate,
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
                        do_substitute_variables_into<Arg>(
                          temp, inner, translate, opts);
                        if(const auto translation2{translate(temp)}) {
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
auto substitute_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<string_view>(const string_view) noexcept>&
    translate,
  const variable_substitution_options opts) noexcept -> std::string& {
    return do_substitute_variables_into<string_view>(dst, src, translate, opts);
}
//------------------------------------------------------------------------------
auto substitute_str_variables_into(
  std::string& dst,
  string_view src,
  const callable_ref<std::optional<std::string>(const string_view) noexcept>&
    translate,
  const variable_substitution_options opts) noexcept -> std::string& {
    return do_substitute_variables_into<std::string>(dst, src, translate, opts);
}
//------------------------------------------------------------------------------
auto substitute_variables(
  const std::string& str,
  const span<const std::string> strings,
  const variable_substitution_options opts) noexcept -> std::string {
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
auto substitute_variables_into(
  std::string& dest,
  const string_view str,
  const span<const std::string> strings,
  const variable_substitution_options opts) noexcept -> std::string& {
    const auto translate_func =
      [&strings](string_view key) -> std::optional<string_view> {
        char* e = nullptr; // NOLINT(hicpp-vararg)
        const span_size_t idx = span_size(std::strtol(c_str(key), &e, 10));
        if((0 < idx) and (idx <= strings.size())) {
            return {{strings[idx - 1]}};
        }
        return {};
    };
    return substitute_variables_into(
      dest, str, {construct_from, translate_func}, opts);
}
//------------------------------------------------------------------------------
auto substitute_variables(
  const std::string& str,
  const std::map<std::string, std::string, str_view_less>& dictionary,
  const variable_substitution_options opts) noexcept -> std::string {
    const auto translate_func{[&dictionary](string_view key) {
        return find(dictionary, key)
          .and_then([](const auto& found) -> std::optional<std::string_view> {
              return {found};
          });
    }};
    return substitute_variables(str, {construct_from, translate_func}, opts);
}
//------------------------------------------------------------------------------
} // namespace eagine
