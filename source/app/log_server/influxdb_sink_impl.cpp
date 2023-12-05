/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<curl/curl.h>)
#include <curl/curl.h>
#define EAGINE_USE_LIBCURL 1
#else
#define EAGINE_USE_LIBCURL 0
#endif

module eagine.core.log_server;

import std;
import eagine.core;

import :interfaces;
import :utilities;

namespace eagine::logs {
#if EAGINE_USE_LIBCURL
//------------------------------------------------------------------------------
namespace curl {
//------------------------------------------------------------------------------
using connection_handle = std::unique_ptr<::CURL, void (*)(::CURL*)>;
//------------------------------------------------------------------------------
class connection : connection_handle {
public:
    connection(::CURL* conn, std::string conn_params) noexcept
      : connection_handle{conn, ::curl_easy_cleanup}
      , _conn_params{std::move(conn_params)} {}

    static auto connect(std::string conn_params) noexcept {
        return connection{::curl_easy_init(), std::move(conn_params)};
    }

    auto is_ok() const noexcept -> bool {
        return connection_handle::operator bool();
    }

    explicit operator bool() const noexcept {
        return is_ok();
    }

private:
    url _conn_params;
};
//------------------------------------------------------------------------------
} // namespace curl
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
class influxdb_stream_sink;
class influxdb_stream_sink_factory final
  : public std::enable_shared_from_this<influxdb_stream_sink_factory>
  , public stream_sink_factory {
public:
    influxdb_stream_sink_factory(main_ctx&, std::string conn_params) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

    auto is_conn_ok() const noexcept -> bool;

    auto consume(influxdb_stream_sink&, const begin_info&) noexcept -> bool;
    auto consume(influxdb_stream_sink&, const description_info&) noexcept
      -> bool;
    auto consume(influxdb_stream_sink&, const declare_state_info&) noexcept
      -> bool;
    auto consume(influxdb_stream_sink&, const active_state_info&) noexcept
      -> bool;
    auto consume(influxdb_stream_sink&, const message_info&) noexcept -> bool;
    auto consume(
      influxdb_stream_sink&,
      const span_size_t entry_id,
      const message_info::arg_info&) noexcept -> bool;
    auto consume(influxdb_stream_sink&, const aggregate_interval_info&) noexcept
      -> bool;
    auto consume(influxdb_stream_sink&, const heartbeat_info&) noexcept -> bool;
    auto consume(const influxdb_stream_sink&, const finish_info&) noexcept
      -> bool;

private:
    curl::connection _conn;
};
//------------------------------------------------------------------------------
// stream sink
//------------------------------------------------------------------------------
class influxdb_stream_sink final : public stream_sink {
public:
    influxdb_stream_sink(
      shared_holder<influxdb_stream_sink_factory> parent) noexcept;

    void set_id(stream_id_t) noexcept;
    auto id() const noexcept -> stream_id_t;
    auto root() const noexcept -> identifier;

    void consume(const begin_info&) noexcept final;
    void consume(const description_info&) noexcept final;
    void consume(const declare_state_info&) noexcept final;
    void consume(const active_state_info&) noexcept final;
    void consume(const message_info&) noexcept final;
    void consume(const interval_info&) noexcept final;
    void consume(const heartbeat_info&) noexcept final;
    void consume(const finish_info&) noexcept final;

private:
    void _flush_backlog() noexcept;
    void _dispatch(const auto&) noexcept;

    shared_holder<influxdb_stream_sink_factory> _parent;
    stream_id_t _id{0};
    identifier _root{};
    std::vector<std::variant<
      begin_info,
      description_info,
      declare_state_info,
      active_state_info,
      message_info,
      aggregate_interval_info,
      heartbeat_info,
      finish_info>>
      _backlog;

    aggregate_intervals _intervals{std::chrono::seconds{120}};
};
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
influxdb_stream_sink::influxdb_stream_sink(
  shared_holder<influxdb_stream_sink_factory> parent) noexcept
  : _parent{std::move(parent)} {}
//------------------------------------------------------------------------------
void influxdb_stream_sink::set_id(stream_id_t id) noexcept {
    _id = id;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::_flush_backlog() noexcept {
    auto pos{_backlog.begin()};
    while(pos != _backlog.end()) {
        if(not std::visit(
             [this](const auto& info) { return _parent->consume(*this, info); },
             *pos)) {
            break;
        }
        ++pos;
    }
    if(pos == _backlog.end()) {
        _backlog.clear();
    } else {
        _backlog.erase(_backlog.begin(), pos);
    }
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::_dispatch(const auto& info) noexcept {
    if(not _backlog.empty()) {
        _flush_backlog();
    }
    if(not _parent->consume(*this, info)) {
        _backlog.push_back(info);
    }
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink::root() const noexcept -> identifier {
    return _root;
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const begin_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const description_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const declare_state_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const active_state_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const message_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const interval_info& info) noexcept {
    if(const auto aggregate{_intervals.update(info)}) {
        _dispatch(*aggregate);
        _intervals.reset(aggregate);
    }
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const heartbeat_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const finish_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const begin_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const description_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const declare_state_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const active_state_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const message_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const span_size_t entry_id,
  const message_info::arg_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const aggregate_interval_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink&,
  const heartbeat_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  const influxdb_stream_sink&,
  const finish_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
// factory function
//------------------------------------------------------------------------------
auto make_influxdb_sink_factory(
  [[maybe_unused]] main_ctx& ctx,
  [[maybe_unused]] string_view conn_params) noexcept
  -> shared_holder<stream_sink_factory> {
#if EAGINE_USE_LIBPQ
    if(conn_params.empty()) {
        conn_params = "http://admin:admin@localhost:8086/api/v2";
    }
    return {hold<influxdb_stream_sink_factory>, ctx, to_string(conn_params)};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
