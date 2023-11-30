/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<libpq-fe.h>)
#include <libpq-fe.h>
#define EAGINE_USE_LIBPQ 1
#else
#define EAGINE_USE_LIBPQ 0
#endif

module eagine.core.log_server;

import std;
import eagine.core;

import :interfaces;
import :utilities;

namespace eagine::logs {
#if EAGINE_USE_LIBPQ
//------------------------------------------------------------------------------
using stream_id_t = std::size_t;
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
class libpq_stream_sink;
class libpq_stream_sink_factory final
  : public std::enable_shared_from_this<libpq_stream_sink_factory>
  , public stream_sink_factory {
public:
    libpq_stream_sink_factory(main_ctx&, string_view conn_params) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

    auto is_conn_ok() const noexcept -> bool;

    auto consume(libpq_stream_sink&, const begin_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const description_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const declare_state_info&) noexcept
      -> bool;
    auto consume(libpq_stream_sink&, const active_state_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const message_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const interval_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const heartbeat_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const finish_info&) noexcept -> bool;

private:
    auto _get_stream_id() noexcept -> optionally_valid<stream_id_t>;

    std::unique_ptr<PGconn, void (*)(PGconn*)> _conn;
    backing_off_timeout _should_reconnect{
      std::chrono::seconds{1},
      std::chrono::minutes{1}};
};
//------------------------------------------------------------------------------
// stream sink
//------------------------------------------------------------------------------
class libpq_stream_sink final : public stream_sink {
public:
    libpq_stream_sink(shared_holder<libpq_stream_sink_factory> parent) noexcept;

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

    shared_holder<libpq_stream_sink_factory> _parent;
    stream_id_t _id{0};
    identifier _root{};
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
};
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
libpq_stream_sink::libpq_stream_sink(
  shared_holder<libpq_stream_sink_factory> parent) noexcept
  : _parent{std::move(parent)} {}
//------------------------------------------------------------------------------
auto libpq_stream_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink::root() const noexcept -> identifier {
    return _root;
}
//------------------------------------------------------------------------------
void libpq_stream_sink::_flush_backlog() noexcept {
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
void libpq_stream_sink::_dispatch(const auto& info) noexcept {
    if(not _backlog.empty()) {
        _flush_backlog();
    }
    if(not _parent->consume(*this, info)) {
        _backlog.push_back(info);
    }
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const begin_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const description_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const declare_state_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const active_state_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const message_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const interval_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const heartbeat_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const finish_info& info) noexcept {
    _dispatch(info);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
libpq_stream_sink_factory::libpq_stream_sink_factory(
  main_ctx&,
  string_view conn_params) noexcept
  : _conn{::PQconnectdb(c_str(conn_params)), &::PQfinish} {}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::make_stream() noexcept
  -> unique_holder<stream_sink> {
    return {hold<libpq_stream_sink>, shared_from_this()};
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::is_conn_ok() const noexcept -> bool {
    return ::PQstatus(_conn.get()) == CONNECTION_OK;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const begin_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const description_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const declare_state_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const active_state_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const message_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const interval_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const heartbeat_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink&,
  const finish_info&) noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::update() noexcept {
    if(not is_conn_ok()) {
        if(_should_reconnect.is_expired()) {
            ::PQreset(_conn.get());
            if(is_conn_ok()) {
                _should_reconnect.rewind();
            } else {
                _should_reconnect.extend();
            }
        }
    }
}
//------------------------------------------------------------------------------
#endif // EAGINE_USE_LIBPQ
//------------------------------------------------------------------------------
auto make_libpq_sink_factory(
  [[maybe_unused]] main_ctx& ctx,
  [[maybe_unused]] string_view conn_params) noexcept
  -> shared_holder<stream_sink_factory> {
#if EAGINE_USE_LIBPQ
    if(conn_params.empty()) {
        conn_params = "postgresql://eagilog@localhost/eagilog";
    }
    return {hold<libpq_stream_sink_factory>, ctx, conn_params};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
