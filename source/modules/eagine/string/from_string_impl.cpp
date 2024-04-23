/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cerrno>
#if __has_include(<boost/spirit/home/x3.hpp>)
#include <boost/spirit/home/x3.hpp>
#include <cassert>
#ifndef EAGINE_USE_BOOST_SPIRIT
#define EAGINE_USE_BOOST_SPIRIT 1
#endif
#else
#ifndef EAGINE_USE_BOOST_SPIRIT
#define EAGINE_USE_BOOST_SPIRIT 0
#endif
#endif

module eagine.core.string;
import std;
import eagine.core.types;
import eagine.core.memory;

#if EAGINE_USE_BOOST_SPIRIT
import eagine.core.math;
#endif

namespace eagine {
//------------------------------------------------------------------------------
auto string_traits<std::string>::from(const string_view src) noexcept
  -> always_valid<std::string> {
    return to_string(src);
}
//------------------------------------------------------------------------------
auto string_traits<char>::from(const string_view src) noexcept
  -> optionally_valid<char> {
    if(src.has_value()) {
        return {*src, true};
    }
    return {};
}
//------------------------------------------------------------------------------
auto string_traits<bool>::from(const string_view src) noexcept
  -> optionally_valid<bool> {
    const string_view true_strs[] = {{"true"}, {"True"}, {"1"}, {"t"}, {"T"}};
    if(find_element(view(true_strs), src)) {
        return {true, true};
    }

    const string_view false_strs[] = {
      {"false"}, {"False"}, {"0"}, {"f"}, {"F"}};
    if(find_element(view(false_strs), src)) {
        return {false, true};
    }

    return {};
}
//------------------------------------------------------------------------------
auto string_traits<tribool>::from(const string_view src) noexcept
  -> optionally_valid<tribool> {
    if(const auto val{string_traits<bool>{}.from(src)}) {
        return {*val, true};
    }

    const string_view unknown_strs[] = {
      {"indeterminate"}, {"Indeterminate"}, {"unknown"}};
    if(find_element(view(unknown_strs), src)) {
        return {indeterminate, true};
    }

    return {};
}
//------------------------------------------------------------------------------
template <typename T, typename N>
constexpr auto multiply_and_convert_if_fits(const N n, string_view t) noexcept
  -> optionally_valid<T> {
    if(t.empty()) {
        return convert_if_fits<T>(n);
    } else if(t.has_single_value()) {
        if(t.back() == 'k') {
            return convert_if_fits<T>(n * 1000);
        }
        if(t.back() == 'M') {
            return convert_if_fits<T>(n * 1000000);
        }
        if(t.back() == 'G') {
            return convert_if_fits<T>(n * 1000000000);
        }
    } else if(t.size() == 2) {
        if(t.back() == 'i') {
            if(t.front() == 'K') {
                return convert_if_fits<T>(n * 1024);
            }
            if(t.front() == 'M') {
                return convert_if_fits<T>(n * 1024 * 1024);
            }
            if(t.front() == 'G') {
                return convert_if_fits<T>(n * 1024 * 1024 * 1024);
            }
        }
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename T, typename N>
auto convert_from_string_with(
  N (*converter)(const char*, char**),
  const string_view src,
  const std::type_identity<T> tid) noexcept -> optionally_valid<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result{converter(cstr, &end)};
    if((errno != ERANGE) and (end != cstr) and (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), cstr.position_of(end)))}) {
            return converted;
        }
    }

    return parse_from_string(src, tid);
}
//------------------------------------------------------------------------------
template <typename T, typename N>
auto convert_from_string_with(
  N (*converter)(const char*, char**, int),
  const int base,
  const string_view src,
  const std::type_identity<T> tid) noexcept -> optionally_valid<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result = converter(cstr, &end, base);
    if((errno != ERANGE) and (end != cstr) and (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), cstr.position_of(end)))}) {
            return converted;
        }
    }
    return parse_from_string(src, tid);
}
//------------------------------------------------------------------------------
auto string_traits<short>::from(const string_view src, const int base) noexcept
  -> optionally_valid<short> {
    return convert_from_string_with(
      &std::strtol, base, src, std::type_identity<short>{});
}
//------------------------------------------------------------------------------
auto string_traits<int>::from(const string_view src, const int base) noexcept
  -> optionally_valid<int> {
    return convert_from_string_with(
      &std::strtol, base, src, std::type_identity<int>{});
}
//------------------------------------------------------------------------------
auto string_traits<long>::from(const string_view src, const int base) noexcept
  -> optionally_valid<long> {
    return convert_from_string_with(
      &std::strtol, base, src, std::type_identity<long>{});
}
//------------------------------------------------------------------------------
auto string_traits<long long>::from(
  const string_view src,
  const int base) noexcept -> optionally_valid<long long> {
    return convert_from_string_with(
      &std::strtoll, base, src, std::type_identity<long long>{});
}
//------------------------------------------------------------------------------
auto string_traits<unsigned short>::from(
  const string_view src,
  const int base) noexcept -> optionally_valid<unsigned short> {
    return convert_from_string_with(
      &std::strtoul, base, src, std::type_identity<unsigned short>{});
}
//------------------------------------------------------------------------------
auto string_traits<unsigned int>::from(
  const string_view src,
  const int base) noexcept -> optionally_valid<unsigned int> {
    return convert_from_string_with(
      &std::strtoul, base, src, std::type_identity<unsigned int>{});
}
//------------------------------------------------------------------------------
auto string_traits<unsigned long>::from(
  const string_view src,
  const int base) noexcept -> optionally_valid<unsigned long> {
    return convert_from_string_with(
      &std::strtoul, base, src, std::type_identity<unsigned long>{});
}
//------------------------------------------------------------------------------
auto string_traits<unsigned long long>::from(
  const string_view src,
  const int base) noexcept -> optionally_valid<unsigned long long> {
    return convert_from_string_with(
      &std::strtoull, base, src, std::type_identity<unsigned long long>{});
}
//------------------------------------------------------------------------------
auto string_traits<byte>::from(const string_view src) noexcept
  -> optionally_valid<byte> {
    if(starts_with(src, string_view("0x"))) {
        if(const auto opt_val{
             string_traits<unsigned>{}.from(skip(src, 2), 16)}) {
            return {static_cast<byte>(*opt_val), opt_val <= 255U};
        }
    }
    if(starts_with(src, string_view("0"))) {
        if(const auto opt_val{
             string_traits<unsigned>{}.from(skip(src, 1), 8)}) {
            return {static_cast<byte>(*opt_val), opt_val <= 255U};
        }
    }
    if(const auto opt_val{string_traits<unsigned>{}.from(src, 10)}) {
        return {static_cast<byte>(*opt_val), opt_val <= 255U};
    }
    return {};
}
//------------------------------------------------------------------------------
auto string_traits<float>::from(const string_view src) noexcept
  -> optionally_valid<float> {
    return convert_from_string_with(
      &std::strtof, src, std::type_identity<float>{});
}

auto string_traits<double>::from(const string_view src) noexcept
  -> optionally_valid<double> {
    return convert_from_string_with(
      &std::strtod, src, std::type_identity<double>{});
}

auto string_traits<long double>::from(const string_view src) noexcept
  -> optionally_valid<long double> {
    return convert_from_string_with(
      &std::strtold, src, std::type_identity<long double>{});
}
//------------------------------------------------------------------------------
#if EAGINE_USE_BOOST_SPIRIT
namespace numexpr {
//------------------------------------------------------------------------------
namespace bs = boost::spirit::x3;
namespace bf = boost::fusion;
using bf::at_c;
using bs::_attr;
using bs::_val;
using parsed_number_t = std::complex<long double>;
//------------------------------------------------------------------------------
struct symbolic_constants : bs::symbols<parsed_number_t> {
    symbolic_constants() noexcept {
        add("pi", parsed_number_t(math::pi))("i", parsed_number_t(0, 1));
    }
};
//------------------------------------------------------------------------------
const symbolic_constants symbolic_constant{};

struct numeric_expression_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<numeric_expression_tag, parsed_number_t> numeric_expression =
  "numeric-expression";

struct function_call_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<function_call_tag, parsed_number_t> function_call =
  "function-call";

struct numeric_literal_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<numeric_literal_tag, parsed_number_t> numeric_literal =
  "numeric-literal";
// NOLINTNEXTLINE(cert-err58-cpp,hicpp-signed-bitwise)
const auto numeric_literal_def = bs::double_ | bs::int_;

//------------------------------------------------------------------------------
struct juxtaposition_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<juxtaposition_tag, parsed_number_t> juxtaposition =
  "juxtaposition";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto juxtaposition_def = (numeric_literal >> symbolic_constant)[(
  [](auto& c) { _val(c) = at_c<0>(_attr(c)) * at_c<1>(_attr(c)); })];
//------------------------------------------------------------------------------
struct conversion_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<conversion_tag, parsed_number_t> conversion = "conversion";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto conversion_def =
  (numeric_literal >> "deg")[([](auto& c) {
      _val(c) = ((parsed_number_t(math::pi) * _attr(c)) / parsed_number_t(180));
  })] |
  (numeric_literal >>
   "turn")[([](auto& c) { _val(c) = (parsed_number_t(math::tau) * _attr(c)); })];
//------------------------------------------------------------------------------
struct numeric_constant_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<numeric_constant_tag, parsed_number_t> numeric_constant =
  "numeric-constant";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto numeric_constant_def =
  (juxtaposition | conversion | numeric_literal | symbolic_constant);
//------------------------------------------------------------------------------
struct coersion_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<coersion_tag, parsed_number_t> coersion = "coersion";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto coersion_def =
  (numeric_constant)[([](auto& c) { _val(c) = _attr(c); })] |
  ("-" >> numeric_constant)[([](auto& c) { _val(c) = -_attr(c); })] |
  (function_call)[([](auto& c) { _val(c) = _attr(c); })] |
  ("-" >> function_call)[([](auto& c) { _val(c) = -_attr(c); })] |
  ("(" >> numeric_expression >> ")")[([](auto& c) { _val(c) = _attr(c); })] |
  ("-(" >> numeric_expression >> ")")[([](auto& c) { _val(c) = -_attr(c); })];
//------------------------------------------------------------------------------
struct numeric_factor_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<numeric_factor_tag, parsed_number_t> numeric_factor =
  "numeric-factor";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto numeric_factor_def = (coersion >> *('^' >> coersion))[([](auto& c) {
    auto l{at_c<0>(_attr(c))};
    const auto& ve{at_c<1>(_attr(c))};
    if(ve.empty()) {
        _val(c) = l;
    } else {
        parsed_number_t e{1};
        for(auto i = ve.rbegin(); i != ve.rend(); ++i) {
            e = std::pow(*i, e);
        }
        _val(c) = std::pow(l, e);
    }
})];
//------------------------------------------------------------------------------
struct numeric_term_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<numeric_term_tag, parsed_number_t> numeric_term = "numeric-term";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto numeric_term_def =
  (numeric_factor >> *(bs::char_("*/") >> numeric_factor))[([](auto& c) {
      auto l{at_c<0>(_attr(c))};
      for(const auto& p : at_c<1>(_attr(c))) {
          const auto o{at_c<0>(p)};
          const auto r{at_c<1>(p)};
          l = (o == '*') ? (l * r) : (l / r);
      }
      _val(c) = l;
  })];
//------------------------------------------------------------------------------
struct arith_expression_tag;
// NOLINTNEXTLINE(cert-err58-cpp)
const bs::rule<arith_expression_tag, parsed_number_t> arith_expression =
  "arithmetic-expression";
// NOLINTNEXTLINE(cert-err58-cpp)
const auto arith_expression_def =
  (numeric_term >> *(bs::char_("+-") >> numeric_term))[([](auto& c) {
      auto l{at_c<0>(_attr(c))};
      for(const auto& p : at_c<1>(_attr(c))) {
          const auto o{at_c<0>(p)};
          const auto r{at_c<1>(p)};
          l = (o == '+') ? (l + r) : (l - r);
      }
      _val(c) = l;
  })];
//------------------------------------------------------------------------------
// NOLINTNEXTLINE(cert-err58-cpp)
const auto function_call_def =
  (bs::lit("sqrt") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::sqrt(_attr(c)); })] |
  (bs::lit("sin") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::sin(_attr(c)); })] |
  (bs::lit("cos") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::cos(_attr(c)); })] |
  (bs::lit("tg") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::tan(_attr(c)); })] |
  (bs::lit("ln") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::log(_attr(c)); })] |
  (bs::lit("exp") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = std::exp(_attr(c)); })] |
  (bs::lit("sin01") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = math::sine_wave01(_attr(c)); })] |
  (bs::lit("cos01") >> "(" >> numeric_expression >>
   ")")[([](auto& c) { _val(c) = math::cosine_wave01(_attr(c)); })] |
  (bs::lit("sigmoid") >> "(" >> numeric_expression >> "," >>
   numeric_expression >> ")")[([](auto& c) {
      _val(c) =
        math::sigmoid01(at_c<0>(_attr(c)).real(), at_c<1>(_attr(c)).real());
  })] |
  (bs::lit("min") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   ")")[([](auto& c) {
      _val(c) =
        math::minimum(at_c<0>(_attr(c)).real(), at_c<1>(_attr(c)).real());
  })] |
  (bs::lit("max") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   ")")[([](auto& c) {
      _val(c) =
        math::maximum(at_c<0>(_attr(c)).real(), at_c<1>(_attr(c)).real());
  })] |
  (bs::lit("clamp") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   "," >> numeric_expression >> ")")[([](auto& c) {
      _val(c) = math::clamp(
        at_c<0>(_attr(c)).real(),
        at_c<1>(_attr(c)).real(),
        at_c<2>(_attr(c)).real());
  })] |
  (bs::lit("ramp") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   "," >> numeric_expression >> ")")[([](auto& c) {
      _val(c) = math::ramp(
        at_c<0>(_attr(c)).real(),
        at_c<1>(_attr(c)).real(),
        at_c<2>(_attr(c)).real());
  })] |
  (bs::lit("lerp") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   "," >> numeric_expression >> ")")[([](auto& c) {
      _val(c) = math::lerp(
        at_c<0>(_attr(c)).real(),
        at_c<1>(_attr(c)).real(),
        at_c<2>(_attr(c)).real());
  })] |
  (bs::lit("slerp") >> "(" >> numeric_expression >> "," >> numeric_expression >>
   "," >> numeric_expression >> ")")[([](auto& c) {
      _val(c) = math::smooth_lerp(
        at_c<0>(_attr(c)).real(),
        at_c<1>(_attr(c)).real(),
        at_c<2>(_attr(c)).real());
  })] |
  (bs::lit("oscillate") >> "(" >> numeric_expression >> "," >>
   numeric_expression >> ")")[([](auto& c) {
      _val(c) = math::smooth_oscillate(
        at_c<0>(_attr(c)).real(), at_c<1>(_attr(c)).real());
  })] |
  (bs::lit("e") >> "^" >>
   numeric_term)[([](auto& c) { _val(c) = std::exp(_attr(c)); })];
//------------------------------------------------------------------------------
// NOLINTNEXTLINE(cert-err58-cpp)
const auto numeric_expression_def = arith_expression | function_call;

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
BOOST_SPIRIT_DEFINE(
  numeric_literal,
  juxtaposition,
  numeric_constant,
  conversion,
  coersion,
  numeric_factor,
  numeric_term,
  arith_expression,
  function_call,
  numeric_expression)
//------------------------------------------------------------------------------
template <typename Iter>
static inline auto do_parse(Iter i, Iter e, parsed_number_t& n) {
    if(bs::phrase_parse(i, e, numeric_expression, bs::ascii::space, n)) {
        return i == e;
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace numexpr
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long double& parsed) noexcept
  -> bool {
    numexpr::parsed_number_t temp{};
    // Spirit does not support non-ASCII7 characters
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    if(memory::all_of(src, [](char c) { return (c & ~(0x7f * 2 + 1)) == 0; })) {
        if(numexpr::do_parse(src.begin(), src.end(), temp)) {
            parsed = temp.real();
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long long int& parsed) noexcept
  -> bool {
    long double temp{};
    if(_parse_from_string(src, temp)) {
        const auto rnd{std::round(temp)};
        const auto eps{std::numeric_limits<long double>::epsilon()};
        if(std::abs(temp - rnd) <= eps) {
            parsed = static_cast<long long int>(rnd);
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
#else
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view, long double&) noexcept -> bool {
    return false;
}
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view, long long int&) noexcept -> bool {
    return false;
}
//------------------------------------------------------------------------------
#endif
} // namespace eagine
