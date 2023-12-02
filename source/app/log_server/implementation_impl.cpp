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
// arg info
//------------------------------------------------------------------------------
auto message_info::arg_info::value_bool() const noexcept -> tribool {
    if(const auto val{get_if<bool>(value)}; val.has_value()) {
        return {*val};
    }
    if(const auto val{get_if<std::int64_t>(value)}) {
        return {*val == true};
    }
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return {*val == true};
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_int() const noexcept
  -> optionally_valid<int> {
    if(const auto val{get_if<std::int64_t>(value)}) {
        return convert_if_fits<int>(*val);
    }
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return convert_if_fits<int>(*val);
    }
    return {};
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_int64() const noexcept
  -> optionally_valid<std::int64_t> {
    if(const auto val{get_if<std::int64_t>(value)}) {
        return *val;
    }
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return convert_if_fits<std::int64_t>(*val);
    }
    return {};
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_uint64() const noexcept
  -> optionally_valid<std::uint64_t> {
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return *val;
    }
    if(const auto val{get_if<std::int64_t>(value)}) {
        return convert_if_fits<std::uint64_t>(*val);
    }
    return {};
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_float() const noexcept
  -> optionally_valid<float> {
    if(const auto val{get_if<float>(value)}) {
        return *val;
    }
    if(const auto val{get_if<std::int64_t>(value)}) {
        return float(*val);
    }
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return float(*val);
    }
    return {};
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_duration() const noexcept
  -> optionally_valid<float_seconds> {
    if(const auto val{get_if<float_seconds>(value)}) {
        return *val;
    }
    if(const auto val{get_if<float>(value)}) {
        return float_seconds{*val};
    }
    if(const auto val{get_if<std::int64_t>(value)}) {
        return float_seconds{float(*val)};
    }
    if(const auto val{get_if<std::uint64_t>(value)}) {
        return float_seconds{float(*val)};
    }
    return {};
}
//------------------------------------------------------------------------------
auto message_info::arg_info::value_string() const noexcept
  -> optionally_valid<string_view> {
    if(const auto val{get_if<std::string>(value)}) {
        return string_view{*val};
    }
    return {};
}
//------------------------------------------------------------------------------
// message info
//------------------------------------------------------------------------------
auto message_info::find_arg(identifier name) const noexcept
  -> optional_reference<const arg_info> {
    for(auto& arg : args) {
        if(arg.name == name) {
            return {arg};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
// factory functions
//------------------------------------------------------------------------------
auto make_text_output(main_ctx& ctx) -> unique_holder<text_output> {
    std::vector<unique_holder<text_output>> outputs;
    if(const auto arg{ctx.args().find("--netcat")}) {
        string_view address;
        if(not arg.next().starts_with("-")) {
            address = arg.next().get();
        }
        outputs.emplace_back(make_asio_tcp_ipv4_text_output(ctx, address));
    }

    if(const auto arg{ctx.args().find("--socat")}) {
        string_view address;
        if(not arg.next().starts_with("-")) {
            address = arg.next().get();
        }
        outputs.emplace_back(make_asio_local_text_output(ctx, address));
    }

    if(ctx.args().find("--ostream") or outputs.empty()) {
        outputs.emplace_back(make_ostream_text_output(ctx));
    }

    if(outputs.size() == 1) {
        return std::move(outputs.front());
    }

    return make_combined_text_output(std::move(outputs));
}
//------------------------------------------------------------------------------
auto make_sink_factory(main_ctx& ctx) noexcept
  -> shared_holder<stream_sink_factory> {
    std::vector<shared_holder<stream_sink_factory>> factories;
    if(const auto arg{ctx.args().find("--libpq")}) {
        string_view address;
        if(not arg.next().starts_with("-")) {
            address = arg.next().get();
        }
        factories.emplace_back(make_libpq_sink_factory(ctx, address));
    }
    if(ctx.args().find("--ostream") or factories.empty()) {
        factories.emplace_back(
          make_text_tree_sink_factory(ctx, make_text_output(ctx)));
    }
    if(factories.size() == 1) {
        return std::move(factories.front());
    }
    return make_combined_sink_factory(ctx, std::move(factories));
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
