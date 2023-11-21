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
        return to_string(t.count()) + "us";
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
auto operator<<(std::ostream& output, const string_padded_to& s) noexcept
  -> std::ostream& {
    output << std::string(std_size(s.before), ' ');
    output << s.str;
    output << std::string(std_size(s.after), ' ');
    return output;
}
//------------------------------------------------------------------------------
auto padded_to(std::size_t l, const string_view s) noexcept
  -> string_padded_to {
    const auto sl{span_size(l)};
    const auto ss{s.size()}; // TODO: count "glyphs" not chars here
    if(sl < ss) {
        return {.str = head(s, sl)};
    }
    if(sl > s.size()) {
        const auto a{(sl - ss) / 2};
        const auto b{sl - ss - a};
        return {.str = s, .before = b, .after = a};
    }
    return {.str = s};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
