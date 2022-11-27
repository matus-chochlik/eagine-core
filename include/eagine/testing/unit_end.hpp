/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
import <format>;
import <limits>;
import <iostream>;

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
    if constexpr(std::is_integral_v<T>) {
        return std::uniform_int_distribution<T>(min, max)(_re);
    }
    // TODO
    return {};
}
//------------------------------------------------------------------------------
template <typename T>
auto random_generator::get_any(std::type_identity<T>) noexcept -> T {
    if constexpr(std::is_integral_v<T>) {
        return std::uniform_int_distribution<T>(
          std::numeric_limits<T>::min(), std::numeric_limits<T>::max())(_re);
    }
    // TODO
    return {};
}
//------------------------------------------------------------------------------
// test suite
//------------------------------------------------------------------------------
suite::suite(int argc, const char** argv, std::string_view name) noexcept
  : _rand_gen{argc, argv}
  , _name{name} {
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
auto suite::once(void (*func)(eagitest::suite&)) -> suite& {
    try {
        func(*this);
    } catch(const abort_test_case&) {
    } catch(...) {
    }
    return *this;
}
//------------------------------------------------------------------------------
auto suite::repeat(unsigned count, void (*func)(unsigned, eagitest::suite&))
  -> suite& {
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
auto suite::exit_code() const noexcept -> int {
    return _checks_failed ? 1 : 0;
}
//------------------------------------------------------------------------------
// test case
//------------------------------------------------------------------------------
case_::case_(suite& parent, std::string_view name) noexcept
  : _parent{parent}
  , _name{name} {
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
} // namespace eagitest
