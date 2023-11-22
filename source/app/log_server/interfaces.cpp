/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.log_server:interfaces;

import eagine.core;
import std;

namespace eagine::logs {
//------------------------------------------------------------------------------
// sink
//------------------------------------------------------------------------------
struct begin_info {
    std::chrono::system_clock::time_point start{};
    std::string session;
    std::string identity;
};
//------------------------------------------------------------------------------
struct message_info {
    std::chrono::duration<float> offset;
    std::string format;
    std::string severity;
    std::string source;
    std::string tag;
    std::uint64_t instance{0};
};
//------------------------------------------------------------------------------
auto format_message(const message_info&) noexcept -> std::string;
//------------------------------------------------------------------------------
struct heartbeat_info {
    std::chrono::duration<float> offset;
};
//------------------------------------------------------------------------------
struct finish_info {
    std::chrono::duration<float> offset;
    bool clean{false};
};
//------------------------------------------------------------------------------
struct stream_sink : interface<stream_sink> {
    virtual void consume(const begin_info&) noexcept = 0;
    virtual void consume(const message_info&) noexcept = 0;
    virtual void consume(const heartbeat_info&) noexcept = 0;
    virtual void consume(const finish_info&) noexcept = 0;
};
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
export struct stream_sink_factory : interface<stream_sink_factory> {
    virtual auto make_stream() noexcept -> unique_holder<stream_sink> = 0;
};
//------------------------------------------------------------------------------
auto make_ostream_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
export auto make_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
// parser
//------------------------------------------------------------------------------
using parser_input = valtree::value_tree_stream_input;
//------------------------------------------------------------------------------
auto make_json_parser(main_ctx&, shared_holder<stream_sink>) noexcept
  -> parser_input;
//------------------------------------------------------------------------------
export auto make_data_parser(main_ctx&, shared_holder<stream_sink>) noexcept
  -> parser_input;
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
auto make_asio_local_reader(
  main_ctx&,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept -> unique_holder<reader>;
//------------------------------------------------------------------------------
auto make_asio_tcp_ipv4_reader(
  main_ctx&,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept -> unique_holder<reader>;
//------------------------------------------------------------------------------
export auto make_reader(
  main_ctx&,
  shared_holder<stream_sink_factory> factory) noexcept -> unique_holder<reader>;
//------------------------------------------------------------------------------
// internal logger backeng
//------------------------------------------------------------------------------
export class internal_backend final : public logger_backend {
public:
    internal_backend() noexcept;
    internal_backend(internal_backend&&) = delete;
    internal_backend(const internal_backend&) = delete;
    auto operator=(internal_backend&&) = delete;
    auto operator=(const internal_backend&) = delete;
    ~internal_backend() noexcept final;

    static void set_sink(unique_holder<stream_sink>) noexcept;

private:
    static auto _single_instance_ptr() -> internal_backend*&;

    void _set_sink(unique_holder<stream_sink>) noexcept;

    void _dispatch(auto&& entry) noexcept;

    auto _offset() const noexcept -> std::chrono::duration<float>;

    auto entry_backend(log_event_severity) noexcept -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;

    auto type_id() noexcept -> identifier final;

    void begin_log() noexcept final;

    void time_interval_begin(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final;

    void time_interval_end(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final;

    void set_description(
      identifier source,
      logger_instance_id instance,
      string_view display_name,
      string_view description) noexcept final;

    void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) noexcept final;

    void active_state(
      const identifier source,
      const identifier state_tag) noexcept final;

    auto begin_message(
      identifier source,
      identifier tag,
      logger_instance_id instance,
      log_event_severity severity,
      string_view format) noexcept -> bool final;

    void add_nothing(identifier name, identifier tag) noexcept final;

    void add_identifier(
      identifier name,
      identifier tag,
      identifier value) noexcept final;

    void add_message_id(
      identifier name,
      identifier tag,
      message_id value) noexcept final;

    void add_bool(identifier name, identifier tag, bool value) noexcept final;

    void add_integer(
      identifier name,
      identifier tag,
      std::intmax_t value) noexcept final;

    void add_unsigned(
      identifier name,
      identifier tag,
      std::uintmax_t value) noexcept final;

    void add_float(identifier name, identifier tag, float value) noexcept final;

    void add_float(
      identifier name,
      identifier tag,
      float min,
      float value,
      float max) noexcept final;

    void add_duration(
      identifier name,
      identifier tag,
      std::chrono::duration<float> value) noexcept final;

    void add_string(identifier name, identifier tag, string_view value) noexcept
      final;

    void add_blob(identifier, identifier, memory::const_block) noexcept final;

    void finish_message() noexcept final;

    void heartbeat() noexcept final;

    void finish_log() noexcept final;

    void log_chart_sample(
      identifier,
      logger_instance_id,
      identifier,
      float) noexcept final;

    const std::chrono::steady_clock::time_point _start{
      std::chrono::steady_clock::now()};
    unique_holder<stream_sink> _sink;
    std::vector<
      std::variant<begin_info, message_info, heartbeat_info, finish_info>>
      _backlog;
};
//------------------------------------------------------------------------------
} // namespace eagine::logs