/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_CORE_LOG_SERVER_INTERFACES_HPP
#define EAGINE_CORE_LOG_SERVER_INTERFACES_HPP

import eagine.core;
import std;

namespace eagine::logs {
//------------------------------------------------------------------------------
// sink
//------------------------------------------------------------------------------
struct begin_info {
    std::chrono::system_clock::time_point start;
    std::string session;
    std::string identity;
};
//------------------------------------------------------------------------------
struct message_info {
    std::chrono::duration<float> offset;
    std::string format;
    std::string source;
    std::string tag;
};
//------------------------------------------------------------------------------
struct finish_info {
    bool clean{false};
};
//------------------------------------------------------------------------------
struct stream_sink : interface<stream_sink> {
    virtual void begin(const begin_info&) noexcept = 0;
    virtual void consume(const message_info&) noexcept = 0;
    virtual void finish(const finish_info&) noexcept = 0;
};
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
struct stream_sink_factory : interface<stream_sink_factory> {
    virtual auto make_stream() noexcept -> unique_holder<stream_sink> = 0;
};
//------------------------------------------------------------------------------
auto make_ostream_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
auto make_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
// parser
//------------------------------------------------------------------------------
auto make_json_parser(main_ctx&, shared_holder<stream_sink>) noexcept
  -> valtree::value_tree_stream_input;
//------------------------------------------------------------------------------
auto make_data_parser(main_ctx&, shared_holder<stream_sink>) noexcept
  -> valtree::value_tree_stream_input;
//------------------------------------------------------------------------------
// reader
//------------------------------------------------------------------------------
struct reader : interface<reader> {
    virtual auto run() noexcept -> bool = 0;
};
//------------------------------------------------------------------------------
auto make_cin_reader(
  main_ctx&,
  shared_holder<stream_sink_factory> factory) noexcept -> unique_holder<reader>;
//------------------------------------------------------------------------------
auto make_reader(main_ctx&, shared_holder<stream_sink_factory> factory) noexcept
  -> unique_holder<reader>;
//------------------------------------------------------------------------------
} // namespace eagine::logs

#endif

