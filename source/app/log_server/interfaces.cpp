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
using parser_input = valtree::value_tree_stream_input;
using float_seconds = std::chrono::duration<float>;
//------------------------------------------------------------------------------
// parsed items
//------------------------------------------------------------------------------
struct begin_info {
    std::chrono::system_clock::time_point start{};
    std::string session;
    std::string identity;
};
//------------------------------------------------------------------------------
struct description_info {
    float_seconds offset;
    identifier source;
    std::string display_name;
    std::string description;
    std::uint64_t instance{0};
};
//------------------------------------------------------------------------------
struct declare_state_info {
    float_seconds offset;
    identifier source;
    identifier state_tag;
    identifier begin_tag;
    identifier end_tag;
    std::uint64_t instance{0};
};
//------------------------------------------------------------------------------
struct active_state_info {
    float_seconds offset;
    identifier source;
    identifier tag;
    std::uint64_t instance{0};
};
//------------------------------------------------------------------------------
struct message_info {
    float_seconds offset;
    std::string format;
    log_event_severity severity;
    identifier source;
    identifier tag;
    std::uint64_t instance{0};

    struct arg_info {
        identifier name;
        identifier tag;
        std::variant<
          std::string,
          identifier,
          float_seconds,
          float,
          std::int64_t,
          std::uint64_t,
          bool>
          value;
        std::optional<float> min;
        std::optional<float> max;

        auto value_bool() const noexcept -> tribool;
        auto value_int() const noexcept -> optionally_valid<int>;
        auto value_int64() const noexcept -> optionally_valid<std::int64_t>;
        auto value_uint64() const noexcept -> optionally_valid<std::uint64_t>;
        auto value_float() const noexcept -> optionally_valid<float>;
        auto value_duration() const noexcept -> optionally_valid<float_seconds>;
        auto value_string() const noexcept -> optionally_valid<string_view>;
    };

    std::vector<arg_info> args;

    auto find_arg(identifier) const noexcept
      -> optional_reference<const arg_info>;
};
//------------------------------------------------------------------------------
struct interval_info {
    identifier tag;
    std::uint64_t instance{0};
    std::chrono::nanoseconds duration{0};
};
//------------------------------------------------------------------------------
struct heartbeat_info {
    float_seconds offset;
};
//------------------------------------------------------------------------------
struct finish_info {
    float_seconds offset;
    bool clean{false};
};
//------------------------------------------------------------------------------
// text output
//------------------------------------------------------------------------------
struct text_output : interface<text_output> {
    virtual void write(const string_view) noexcept = 0;
    virtual void flush() noexcept = 0;
};
//------------------------------------------------------------------------------
auto make_ostream_text_output(main_ctx&) -> unique_holder<text_output>;
auto make_asio_local_text_output(main_ctx&, string_view address)
  -> unique_holder<text_output>;
auto make_asio_tcp_ipv4_text_output(main_ctx&, string_view address)
  -> unique_holder<text_output>;
auto make_combined_text_output(std::vector<unique_holder<text_output>>)
  -> unique_holder<text_output>;
//------------------------------------------------------------------------------
export auto make_text_output(main_ctx&) -> unique_holder<text_output>;
//------------------------------------------------------------------------------
// object stream sink
//------------------------------------------------------------------------------
struct stream_sink : interface<stream_sink> {
    virtual void consume(const begin_info&) noexcept = 0;
    virtual void consume(const description_info&) noexcept = 0;
    virtual void consume(const declare_state_info&) noexcept = 0;
    virtual void consume(const active_state_info&) noexcept = 0;
    virtual void consume(const message_info&) noexcept = 0;
    virtual void consume(const interval_info&) noexcept = 0;
    virtual void consume(const heartbeat_info&) noexcept = 0;
    virtual void consume(const finish_info&) noexcept = 0;
};
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
export struct stream_sink_factory : interface<stream_sink_factory> {
    virtual auto make_stream() noexcept -> unique_holder<stream_sink> = 0;
    virtual void update() noexcept = 0;
};
//------------------------------------------------------------------------------
auto make_text_tree_sink_factory(main_ctx&, unique_holder<text_output>)
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
auto make_libpq_sink_factory(main_ctx&, string_view params) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
auto make_combined_sink_factory(
  main_ctx&,
  std::vector<shared_holder<stream_sink_factory>>) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
export auto make_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory>;
//------------------------------------------------------------------------------
// parser
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

    auto _offset() const noexcept -> float_seconds;

    auto entry_backend(log_event_severity) noexcept -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;

    auto type_id() noexcept -> identifier final;

    void begin_log() noexcept final;

    auto register_time_interval(
      const identifier tag,
      const logger_instance_id) noexcept -> time_interval_id final;

    void time_interval_begin(time_interval_id) noexcept final;

    void time_interval_end(time_interval_id) noexcept final;

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
      float_seconds value) noexcept final;

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
    std::vector<std::variant<
      begin_info,
      description_info,
      declare_state_info,
      active_state_info,
      message_info,
      interval_info,
      heartbeat_info,
      finish_info>>
      _backlog;
    message_info _message{};
};
//------------------------------------------------------------------------------
} // namespace eagine::logs
