/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.log_server;

import std;
import eagine.core;

import :utilities;

namespace eagine::logs {
//------------------------------------------------------------------------------
// format reltime
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
auto format_reltime_s(float_seconds t) noexcept -> std::string {
    return format_reltime(
      std::chrono::duration_cast<std::chrono::milliseconds>(t));
}
//------------------------------------------------------------------------------
// format instance
//------------------------------------------------------------------------------
auto format_instance(std::uint64_t i, std::string& s) noexcept -> std::string& {
    i = integer_hash<std::uint64_t, std::uint64_t>(i);
    return base64_encode(as_bytes(view_one(i)), s).value_or(s);
}
//------------------------------------------------------------------------------
// standalone format functions
//------------------------------------------------------------------------------
inline auto unformatted(const message_info::arg_info& info) -> std::string {
    return get_if<std::string>(info.value).value_or("-");
}
//------------------------------------------------------------------------------
auto format_bool(const message_info::arg_info& i) noexcept -> std::string {
    if(const auto val{get_if<bool>(i.value)}; val.has_value()) {
        if(*val) {
            return "True";
        } else {
            return "False";
        }
    }
    return unformatted(i);
}
//------------------------------------------------------------------------------
auto format_duration(const message_info::arg_info& i, bool) noexcept
  -> std::string {
    if(const auto seconds{get_if<float>(i.value)}) {
        return format_reltime_s(float_seconds(*seconds));
    }
    if(const auto seconds{get_if<float_seconds>(i.value)}) {
        return format_reltime_s(*seconds);
    }
    return unformatted(i);
}
//------------------------------------------------------------------------------
auto format_progress(const message_info::arg_info& i, bool) -> std::string {
    if(const auto val{get_if<float>(i.value)}; val and i.min and i.max) {
        if(*(i.min) < *(i.max)) {
            const auto done{(*val - *(i.min)) / (*(i.max) - *(i.min))};
            return std::format("{:.1f}%", 100.F * done);
        }
    }
    return unformatted(i);
}
//------------------------------------------------------------------------------
auto format_main_progress(
  const message_info::arg_info& i,
  float_seconds done_dur,
  bool) -> std::string {
    if(const auto val{get_if<float>(i.value)}; val and i.min and i.max) {
        if(*(i.min) < *(i.max)) {
            const auto done{(*val - *(i.min)) / (*(i.max) - *(i.min))};
            const auto estimate{[&] -> std::string {
                if(done > 0.01F and done < 1.F) {
                    return format_reltime_s(done_dur * (1.F - done) / done) +
                           " remaining";
                }
                return "estimating";
            }};
            return std::format("{:.1f}% ({})", 100.F * done, estimate());
        }
    }
    return unformatted(i);
}
//------------------------------------------------------------------------------
// message formatter
//------------------------------------------------------------------------------
auto message_formatter::format(
  const message_info::arg_info& info,
  bool short_value) noexcept -> std::string {
    if(const auto seconds{get_if<float_seconds>(info.value)}) {
        return format_reltime_s(*seconds);
    }
    switch(info.tag.value()) {
        case id_v("duration"):
            return format_duration(info, short_value);
        case id_v("MainPrgrss"):
            if(not _main_prgrs_started) {
                _main_prgrs_start = _curr_msg_time;
                _main_prgrs_started = true;
            }
            return format_main_progress(
              info, _curr_msg_time - _main_prgrs_start, short_value);
        case id_v("Progress"):
            return format_progress(info, short_value);
        case id_v("bool"):
            return format_bool(info);
        default:
            break;
    }
    return unformatted(info);
}
//------------------------------------------------------------------------------
auto message_formatter::format(const message_info& info) noexcept
  -> std::string {
    const auto translate{[&](string_view name) -> std::optional<std::string> {
        for(auto& arg : info.args) {
            if(arg.name.matches(name)) {
                return {format(arg, true)};
            }
        }
        return {};
    }};

    _curr_msg_time = info.offset;
    std::string result;
    return substitute_str_variables_into(
      result, info.format, {construct_from, translate});
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
