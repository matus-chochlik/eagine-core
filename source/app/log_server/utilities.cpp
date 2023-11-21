/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "utilities.hpp"

namespace eagine::logs {
//------------------------------------------------------------------------------
auto format_reltime(std::chrono::microseconds t) noexcept -> std::string {
    using namespace std::chrono;
    using std::to_string;
    if(t == microseconds{0}) {
        return "0";
    }
    if(t < milliseconds{10}) {
        return to_string(t.count()) + "μs";
    }
    if(t < seconds{10}) {
        return to_string(duration_cast<milliseconds>(t).count()) + "ms";
    }
    if(t < minutes{2}) {
        return to_string(duration_cast<seconds>(t).count()) + "s";
    }
    if(t < minutes{90}) {
        return to_string(duration_cast<minutes>(t).count()) + "m " +
               to_string(duration_cast<seconds>(t).count() % 60) + "s";
    }
    if(t < hours{36}) {
        return to_string(duration_cast<hours>(t).count()) + "h " +
               to_string(duration_cast<minutes>(t).count() % 60) + "m";
    }
    if(t < days{7}) {
        return to_string(duration_cast<days>(t).count()) + "d " +
               to_string(duration_cast<hours>(t).count() % 24) + "h";
    }
    return to_string(duration_cast<weeks>(t).count()) + "w " +
           to_string(duration_cast<days>(t).count() % 7) + "d";
}
//------------------------------------------------------------------------------
auto padded_to(std::size_t l, const std::string& s) noexcept -> std::string {
    if(l < s.size()) {
        return s.substr(l);
    }
    if(l > s.size()) {
        const auto a{(l - s.size()) / 2};
        const auto b{l - a - s.size()};
        return std::string(b, ' ') + s + std::string(a, ' ');
    }
    return s;
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
