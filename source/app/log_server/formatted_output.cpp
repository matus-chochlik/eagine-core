/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.log_server:formatted_output;

import std;
import eagine.core;
import :interfaces;
import :utilities;

namespace eagine::logs {
//------------------------------------------------------------------------------
class formatted_output;
//------------------------------------------------------------------------------
// stream sink
//------------------------------------------------------------------------------
using stream_id_t = std::size_t;
//------------------------------------------------------------------------------
struct formatted_stream_context {
    begin_info begin{};
    bool clean_shutdown{false};
};
class formatted_sink;
//------------------------------------------------------------------------------
class formatted_output
  : public std::enable_shared_from_this<formatted_output>
  , public stream_sink_factory {
public:
    void consume(const formatted_sink&, const begin_info&) noexcept;
    void consume(const formatted_sink&, const description_info&) noexcept;
    void consume(const formatted_sink&, const declare_state_info&) noexcept;
    void consume(const formatted_sink&, const active_state_info&) noexcept;
    void consume(
      const formatted_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;
    void consume(const formatted_sink&, const interval_info&) noexcept;
    void consume(const formatted_sink&, const heartbeat_info&) noexcept;
    void consume(const formatted_sink&, const finish_info&) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;

protected:
    virtual void write(string_view) noexcept = 0;

    void _start() noexcept;
    void _finish() noexcept;

private:
    void _conn_I(const formatted_sink&) noexcept;
    void _conn_L(const formatted_sink&) noexcept;
    void _conn_S(const formatted_sink&) noexcept;
    void _conn_s(const formatted_sink&) noexcept;
    void _conn_Z(const formatted_sink&) noexcept;
    void _conn_T(const formatted_sink&) noexcept;
    auto _get_stream_id() noexcept -> std::uintmax_t;

    stream_id_t _id_seq{0};
    flat_map<stream_id_t, formatted_stream_context> _streams;
    std::string _temp;
    bool _condensed{false};
};
//------------------------------------------------------------------------------
} // namespace eagine::logs
