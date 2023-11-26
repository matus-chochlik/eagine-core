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
auto format_default(const message_info::arg_info& i) -> std::string {
    if(const auto seconds{get_if<float_seconds>(i.value)}) {
        return format_reltime_s(*seconds);
    }
    if(const auto val{get_if<float>(i.value)}) {
        return std::format("{:.3}", *val);
    }
    if(const auto val{get_if<std::int64_t>(i.value)}) {
        return std::format("{}", *val);
    }
    if(const auto val{get_if<std::uint64_t>(i.value)}) {
        return std::format("{}", *val);
    }
    if(const auto val{get_if<bool>(i.value)}; val.has_value()) {
        if(*val) {
            return "True";
        } else {
            return "False";
        }
    }
    if(const auto val{get_if<identifier>(i.value)}; val.has_value()) {
        return val->name().str();
    }
    return get_if<std::string>(i.value).value_or("-");
}
//------------------------------------------------------------------------------
auto get_float_value(const message_info::arg_info& i) noexcept
  -> optionally_valid<float> {
    if(const auto val{get_if<float>(i.value)}) {
        return *val;
    }
    if(const auto val{get_if<std::int64_t>(i.value)}) {
        return float(*val);
    }
    if(const auto val{get_if<std::uint64_t>(i.value)}) {
        return float(*val);
    }
    return {};
}
//------------------------------------------------------------------------------
auto format_duration(const message_info::arg_info& i, bool) noexcept
  -> std::string {
    if(const auto seconds{get_if<float_seconds>(i.value)}) {
        return format_reltime_s(*seconds);
    }
    if(const auto val{get_float_value(i)}) {
        return format_reltime_s(float_seconds(*val));
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_endpoint_id(const message_info::arg_info& i, bool) noexcept
  -> std::string {
    if(const auto val{get_if<std::uint64_t>(i.value)}) {
        std::string s;
        return format_instance(*val, s);
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_bytesize(const message_info::arg_info& i, bool) noexcept
  -> std::string {
    if(const auto opt{get_float_value(i)}) {
        const auto val{*opt};
        if(val > 8192.F * 1024.F * 1024.F) {
            return std::format("{:.1f}GiB", val / (1024.F * 1024.F * 1024.F));
        }
        if(val > 8192.F * 1024.F) {
            return std::format("{:.1f}MiB", val / (1024.F * 1024.F));
        }
        if(val > 8192.F) {
            return std::format("{:.1f}kiB", val / 1024.F);
        }
        return std::format("{:.0f}B", val);
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_rate_per_sec(const message_info::arg_info& i, bool) noexcept
  -> std::string {
    if(const auto opt{get_float_value(i)}) {
        const auto val{*opt};
        if(val < 0.1F) {
            return std::format("{:.1f} per hour", val * 3600.F);
        }
        if(val < 10.F) {
            return std::format("{:.1f} per min", val * 60.F);
        }
        if(val > 10'000'000.F) {
            return std::format("{:.1f}M per sec", val / 1000000.F);
        }
        if(val > 10'000.F) {
            return std::format("{:.1f}k per sec", val / 1000.F);
        }
        return std::format("{:.1f} per sec", val);
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_ratio(const message_info::arg_info& i, bool) -> std::string {
    if(const auto val{get_float_value(i)}) {
        return std::format("{:.1f}%", 100.F * *val);
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_progress(const message_info::arg_info& i, bool header_value)
  -> std::string {
    if(const auto val{get_float_value(i)}; val and i.min and i.max) {
        if(*(i.min) < *(i.max)) {
            const auto done{(*val - *(i.min)) / (*(i.max) - *(i.min))};
            if(header_value) {
                return std::format("{:.1f}%", 100.F * done);
            } else {
                // TODO: progress bar
                return std::format("{:.1f}%", 100.F * done);
            }
        }
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
auto format_main_progress(
  const message_info::arg_info& i,
  float_seconds done_dur,
  bool header_value) -> std::string {
    if(const auto val{get_float_value(i)}; val and i.min and i.max) {
        if(*(i.min) < *(i.max)) {
            const auto done{(*val - *(i.min)) / (*(i.max) - *(i.min))};
            const auto estimate{[&] -> std::string {
                if(done > 0.0005F) {
                    if(done < 1.F) {
                        return std::format(
                          " ({} remaining) ",
                          format_reltime_s(done_dur * (1.F - done) / done));
                    } else {
                        return {};
                    }
                }
                return "[estimating...] ";
            }};
            if(header_value) {
                return std::format("{}{:.1f}%", estimate(), 100.F * done);
            } else {
                // TODO: progress bar
                return std::format("{:.1f}%", 100.F * done);
            }
        }
    }
    return format_default(i);
}
//------------------------------------------------------------------------------
// message formatter
//------------------------------------------------------------------------------
auto message_formatter::format(
  const message_info::arg_info& info,
  bool header_value) noexcept -> std::string {
    switch(info.tag.value()) {
        case id_v("duration"):
            return format_duration(info, header_value);
        case id_v("MsgBusEpId"):
            return format_endpoint_id(info, header_value);
        case id_v("ByteSize"):
            return format_bytesize(info, header_value);
        case id_v("RatePerSec"):
            return format_rate_per_sec(info, header_value);
        case id_v("Ratio"):
            return format_ratio(info, header_value);
        case id_v("Progress"):
            return format_progress(info, header_value);
        case id_v("MainPrgrss"):
            if(not _main_prgrs_started) {
                _main_prgrs_start = _curr_msg_time;
                _main_prgrs_started = true;
            }
            return format_main_progress(
              info, _curr_msg_time - _main_prgrs_start, header_value);
        default:
            break;
    }
    return format_default(info);
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
// string padded to length
//------------------------------------------------------------------------------
auto padded_to(std::size_t l, std::string_view s) noexcept -> std::string {
    return std::format("{1: ^{0}}", l, s);
}
//------------------------------------------------------------------------------
auto padded_to(std::size_t l, identifier id) noexcept -> std::string {
    return std::format("{1: ^{0}}", l, std::string_view{id.name()});
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
