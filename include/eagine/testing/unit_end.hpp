/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
import <cmath>;
import <format>;
import <limits>;
import <iostream>;

#include <cassert>
namespace eagitest {
//------------------------------------------------------------------------------
// test random generator
//------------------------------------------------------------------------------
random_generator::random_generator(int, const char**) noexcept
  : _re{std::random_device{}()} {}
//------------------------------------------------------------------------------
template <typename T>
auto random_generator::get_between(T min, T max, std::type_identity<T>) noexcept
  -> T {
    using U = std::remove_const_t<T>;
    if constexpr(std::is_same_v<U, bool>) {
        return bool(std::uniform_int_distribution<int>(0, 1)(_re));
    } else if constexpr(std::is_same_v<U, char>) {
        if constexpr(std::is_signed_v<char>) {
            return std::uniform_int_distribution<signed char>(min, max)(_re);
        } else {
            return std::uniform_int_distribution<unsigned char>(min, max)(_re);
        }
    } else if constexpr(std::is_integral_v<T>) {
        return std::uniform_int_distribution<T>(min, max)(_re);
    } else if constexpr(std::is_floating_point_v<T>) {
        return std::uniform_real_distribution<T>(min, max)(_re);
    }
    // TODO
    return {};
}
//------------------------------------------------------------------------------
template <typename T>
auto random_generator::get_any(std::type_identity<T>) noexcept -> T {
    return get_between(
      std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
}
//------------------------------------------------------------------------------
auto random_generator::get_string_made_of(
  std::size_t min,
  std::size_t max,
  std::string_view chars) noexcept -> std::string {
    std::string result(get_std_size(min, max), '\0');
    for(char& c : result) {
        c = get_char_from(chars);
    }
    return result;
}
//------------------------------------------------------------------------------
auto random_generator::get_string(std::size_t min, std::size_t max) noexcept
  -> std::string {
    return get_string_made_of(
      min,
      max,
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789`~!@#$%^&*(){}_");
}
//------------------------------------------------------------------------------
// test suite
//------------------------------------------------------------------------------
suite::suite(
  int argc,
  const char** argv,
  std::string_view name,
  suite_case_t cases) noexcept
  : _rand_gen{argc, argv}
  , _name{name}
  , _expected_cases{cases} {
    if(_is_verbose) {
        std::clog << "test suite '" << _name << "' started" << std::endl;
    }
}
//------------------------------------------------------------------------------
suite::~suite() noexcept {
    if(_is_verbose) {
        std::clog << "test suite '" << _name << "' finished" << std::endl;
    }
}
//------------------------------------------------------------------------------
auto suite::random() noexcept -> random_generator& {
    return _rand_gen;
}
//------------------------------------------------------------------------------
auto suite::once(void (*func)(eagitest::suite&)) noexcept -> suite& {
    try {
        func(*this);
    } catch(const abort_test_case&) {
    } catch(...) {
    }
    return *this;
}
//------------------------------------------------------------------------------
auto suite::repeat(
  unsigned count,
  void (*func)(unsigned, eagitest::suite&)) noexcept -> suite& {
    for(unsigned i = 0; i < count; ++i) {
        try {
            func(i, *this);
        } catch(const abort_test_case&) {
        } catch(...) {
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
auto suite::tested_case(suite_case_t case_idx) noexcept -> suite& {
    if(_expected_cases != 0U) {
        assert(case_idx != static_cast<suite_case_t>(0));
        _tested_cases =
          _tested_cases | (static_cast<suite_case_t>(1) << (case_idx - 1U));
    }
    return *this;
}
//------------------------------------------------------------------------------
auto suite::exit_code() const noexcept -> int {
    bool failed{_checks_failed};
    for(suite_case_t c = 0U; c < _expected_cases; ++c) {
        if(
          static_cast<suite_case_t>(0) ==
          (_tested_cases & (static_cast<suite_case_t>(1) << c))) {
            std::clog << "  suite '" << this->_name << "' missed case ("
                      << (c + 1) << "/" << _expected_cases << ")";
            std::clog << std::endl;
            failed = true;
        }
    }
    return failed ? 1 : 0;
}
//------------------------------------------------------------------------------
// test case
//------------------------------------------------------------------------------
case_::case_(suite& parent, suite_case_t case_idx, std::string_view name) noexcept
  : _parent{parent}
  , _name{name} {
    _parent.tested_case(case_idx);
    if(_parent._is_verbose) {
        std::clog << " test case '" << _parent._name << "/" << this->_name
                  << "' started" << std::endl;
    }
}
//------------------------------------------------------------------------------
case_::~case_() noexcept {
    if(_parent._is_verbose) {
        std::clog << " test case '" << _parent._name << "/" << this->_name
                  << "' finished" << std::endl;
    }
}
//------------------------------------------------------------------------------
auto case_::repeats(unsigned count) noexcept -> unsigned {
    // TODO: decrease if running on valgrind, etc.
    return count;
}
//------------------------------------------------------------------------------
auto case_::parameter(const auto& value, std::string_view name) noexcept
  -> case_& {
    if(_parent._is_verbose) {
        std::clog << "  parameter '" << _parent._name << "/" << this->_name
                  << "/" << name << "': " << value << std::endl;
    }
    return *this;
}
//------------------------------------------------------------------------------
auto case_::constructed(const auto&, std::string_view name) noexcept -> case_& {
    if(_parent._is_verbose) {
        std::clog << "  object '" << _parent._name << "/" << this->_name << "/"
                  << name << "' constructed" << std::endl;
    }
    return *this;
}
//------------------------------------------------------------------------------
auto case_::checkpoint(
  std::string_view label
#if __cpp_lib_source_location >= 201907L
  ,
  const std::source_location
#endif
  ) noexcept -> case_& {
    std::clog << "  checkpoint '" << _parent._name << "/" << this->_name << "/"
              << label << "' reached" << std::endl;

    return *this;
}
//------------------------------------------------------------------------------
auto case_::ensure(bool condition, std::string_view label, const auto&...)
  -> case_& {
    if(!condition) {
        std::clog << "  check '" << _parent._name << "/" << this->_name << "/"
                  << label << "' failed";
        std::clog << std::endl;
        _parent._checks_failed = true;
        throw abort_test_case{};
    }
    return *this;
}
//------------------------------------------------------------------------------
auto case_::fail(std::string_view label, const auto&...) -> case_& {
    std::clog << "  failure: '" << _parent._name << "/" << this->_name << "/"
              << label << "' failed";
    std::clog << std::endl;
    _parent._checks_failed = true;
    return *this;
}
//------------------------------------------------------------------------------
auto case_::check(
  bool condition,
  std::string_view label,
  const auto&...) noexcept -> case_& {
    if(!condition) {
        std::clog << "  check '" << _parent._name << "/" << this->_name << "/"
                  << label << "' failed";
        std::clog << std::endl;
        _parent._checks_failed = true;
    }
    return *this;
}
//------------------------------------------------------------------------------
template <typename L, typename R>
auto case_::check_equal(const L& l, const R& r, std::string_view label) noexcept
  -> case_& {
    const auto _eq = [&]() {
        if constexpr(std::is_floating_point_v<L> || std::is_floating_point_v<R>) {
            using C = std::common_type_t<L, R>;
            return std::fabs(C(l) - C(r)) <= std::numeric_limits<C>::epsilon();
        } else {
            return (l == r);
        }
    };
    if(!_eq()) {
        std::clog << "  check '" << _parent._name << "/" << this->_name << "/"
                  << label << "' " << l << " == " << r << " failed";
        std::clog << std::endl;
        _parent._checks_failed = true;
    }
    return *this;
}
//------------------------------------------------------------------------------
template <typename L, typename R>
auto case_::check_close(
  const L& l,
  const R& r,
  const std::common_type_t<L, R>& eps,
  std::string_view label) noexcept -> case_& {
    const auto _close = [&]() {
        using std::fabs;
        if constexpr(std::is_floating_point_v<L> && std::is_floating_point_v<R>) {
            const auto cll{std::fpclassify(l)};
            const auto clr{std::fpclassify(r)};
            if(cll == clr) {
                int exl = 0;
                int exr = 0;
                const auto frl{std::frexp(l, &exl)};
                const auto frr{std::frexp(r, &exr)};

                using C = std::common_type_t<L, R>;
                if(exl == exr) {
                    return fabs(C(frl) - C(frr)) <= eps;
                } else if(exl == exr + 1) {
                    return fabs(C(l) - C(r)) <= (C(l) + C(r)) * eps;
                } else if(exl + 1 == exr) {
                    return fabs(C(l) - C(r)) <= (C(l) + C(r)) * eps;
                }
            } else if(cll == FP_ZERO) {
                return fabs(r) <= eps;
            } else if(clr == FP_ZERO) {
                return fabs(l) <= eps;
            }
            return false;
        } else {
            return (l == r);
        }
    };
    if(!_close()) {
        std::clog << "  check '" << _parent._name << "/" << this->_name << "/"
                  << label << "' " << l << " ~= " << r << " failed";
        std::clog << std::endl;
        _parent._checks_failed = true;
    }
    return *this;
}
//------------------------------------------------------------------------------
template <typename L, typename R>
auto case_::check_close(const L& l, const R& r, std::string_view label) noexcept
  -> case_& {
    using C = std::common_type_t<L, R>;
    return check_close(
      l, r, C(1) / C(std::is_same_v<C, float> ? 100 : 1000), label);
}
//------------------------------------------------------------------------------
// test track
//------------------------------------------------------------------------------
track::track(
  case_& parent,
  std::string_view name,
  std::size_t expected_points,
  track_part_t expected_parts) noexcept
  : _parent{parent}
  , _name{name}
  , _expected_points{expected_points}
  , _expected_parts{expected_parts} {}
//------------------------------------------------------------------------------
track::~track() noexcept {
    if(_passed_points < _expected_points) {
        std::clog << "  track '" << _parent._parent._name << "/"
                  << _parent._name << "/" << this->_name << "' not completed ("
                  << _passed_points << "/" << _expected_points << ")";
        std::clog << std::endl;
        _parent._parent._checks_failed = true;
    }
    for(track_part_t p = 0U; p < _expected_parts; ++p) {
        if(
          static_cast<track_part_t>(0) ==
          (_passed_parts & (static_cast<track_part_t>(1) << p))) {
            std::clog << "  track '" << _parent._parent._name << "/"
                      << _parent._name << "/" << this->_name
                      << "' missed part (" << (p + 1) << "/" << _expected_parts
                      << ")";
            std::clog << std::endl;
            _parent._parent._checks_failed = true;
        }
    }
}
//------------------------------------------------------------------------------
auto track::passed_part(track_part_t part_idx) noexcept -> track& {
    ++_passed_points;
    if(_expected_parts != 0U) {
        assert(part_idx != 0U);
        _passed_parts =
          _passed_parts | (static_cast<track_part_t>(1) << (part_idx - 1U));
    }
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagitest
