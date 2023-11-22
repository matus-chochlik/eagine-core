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

namespace eagine::logs {
//------------------------------------------------------------------------------
// standalone format functions
//------------------------------------------------------------------------------
inline auto unformatted(const message_info::arg_info& info) -> std::string {
    return get_if<std::string>(info.value).value_or("-");
}
//------------------------------------------------------------------------------
auto format_progress(const message_info::arg_info& i, bool) -> std::string {
    if(const auto val{get_if<float>(i.value)}; val and i.min and i.max) {
        if(*(i.min) < *(i.max)) {
            return std::format(
              "{:.1f}%", 100.F * (*val - *(i.min)) / (*(i.max) - *(i.min)));
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
    switch(info.tag.value()) {
        case id_v("MainPrgrss"):
        case id_v("Progress"):
            return format_progress(info, short_value);
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

    std::string result;
    return substitute_str_variables_into(
      result, info.format, {construct_from, translate});
}
//------------------------------------------------------------------------------
// factory functions
//------------------------------------------------------------------------------
auto make_sink_factory(main_ctx& ctx) noexcept
  -> shared_holder<stream_sink_factory> {
    return make_ostream_sink_factory(ctx);
}
//------------------------------------------------------------------------------
auto make_data_parser(main_ctx& ctx, shared_holder<stream_sink> sink) noexcept
  -> parser_input {
    return make_json_parser(ctx, std::move(sink));
}
//------------------------------------------------------------------------------
auto make_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory) noexcept
  -> unique_holder<reader> {
    if(const auto arg{ctx.args().find("--local")}) {
        if(arg.next().starts_with("-")) {
            return make_asio_local_reader(ctx, factory, {});
        }
        return make_asio_local_reader(ctx, factory, arg.next().get());
    }

    if(const auto arg{ctx.args().find("--network")}) {
        if(arg.next().starts_with("-")) {
            return make_asio_tcp_ipv4_reader(ctx, factory, {});
        }
        return make_asio_tcp_ipv4_reader(ctx, factory, arg.next().get());
    }

    return make_cin_reader(ctx, std::move(factory));
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
