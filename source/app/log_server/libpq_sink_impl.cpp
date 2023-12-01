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
namespace pq {
//------------------------------------------------------------------------------
using connection_handle = std::unique_ptr<PGconn, void (*)(PGconn*)>;
using result_handle = std::unique_ptr<PGresult, void (*)(PGresult*)>;
//------------------------------------------------------------------------------
class result : result_handle {
public:
    result(PGresult* res) noexcept
      : result_handle{res, ::PQclear} {}

    auto tuples_ok() const noexcept -> bool {
        return ::PQresultStatus(get()) == PGRES_TUPLES_OK;
    }

    explicit operator bool() const noexcept {
        return result_handle::operator bool() and tuples_ok();
    }

    auto get_value(int row, int col) const noexcept -> string_view {
        return {
          static_cast<const char*>(::PQgetvalue(get(), row, col)),
          static_cast<span_size_t>(::PQgetlength(get(), row, col))};
    }

    template <typename T>
    auto get_value_as(int row, int col) const noexcept {
        return from_string<T>(get_value(row, col));
    }
};
//------------------------------------------------------------------------------
class connection : connection_handle {
public:
    connection(PGconn* conn) noexcept
      : connection_handle{conn, ::PQfinish} {}

    static auto connect(string_view conn_params) noexcept {
        return connection{::PQconnectdb(c_str(conn_params))};
    }

    auto is_ok() const noexcept -> bool {
        return ::PQstatus(get()) == CONNECTION_OK;
    }

    explicit operator bool() const noexcept {
        return connection_handle::operator bool() and is_ok();
    }

    auto error_message() const noexcept -> string_view {
        return string_view{::PQerrorMessage(get())};
    }

    auto reconnect() noexcept -> bool {
        ::PQreset(get());
        return is_ok();
    }

    auto exec(string_view sql) const noexcept -> result {
        if(is_ok()) {
            return {::PQexec(get(), c_str(sql))};
        }
        return {nullptr};
    }

    template <typename... Args>
    auto exec(string_view sql, Args&&... args) const noexcept
      -> std::enable_if_t<(sizeof...(Args) > 0), result> {
        if(is_ok()) {
            return _exec_p(
              sql,
              std::array<string_view, sizeof...(Args)>{
                {std::forward<Args>(args)...}},
              std::make_index_sequence<sizeof...(Args)>{});
        }
        return {nullptr};
    }

private:
    template <std::size_t N, std::size_t... I>
    auto _exec_p(
      string_view sql,
      std::array<string_view, N> args,
      std::index_sequence<I...>) const noexcept -> result {
        const std::array<const char*, N> argstr{{args[I].data()...}};
        const std::array<int, N> arglen{{int(args[I].size())...}};
        const std::array<int, N> argfmt{{int(I - I)...}};
        return {::PQexecParams(
          get(),
          c_str(sql),
          int(N),
          nullptr,
          argstr.data(),
          arglen.data(),
          argfmt.data(),
          0)};
    }
};
//------------------------------------------------------------------------------
} // namespace pq
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
    auto get_stream_id() noexcept -> optionally_valid<stream_id_t>;

    auto consume(libpq_stream_sink&, const begin_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const description_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const declare_state_info&) noexcept
      -> bool;
    auto consume(libpq_stream_sink&, const active_state_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const message_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const interval_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const heartbeat_info&) noexcept -> bool;
    auto consume(const libpq_stream_sink&, const finish_info&) noexcept -> bool;

private:
    pq::connection _conn;

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
void libpq_stream_sink::set_id(stream_id_t id) noexcept {
    _id = id;
}
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
  : _conn{pq::connection::connect(conn_params)} {}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::make_stream() noexcept
  -> unique_holder<stream_sink> {
    return {hold<libpq_stream_sink>, shared_from_this()};
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::is_conn_ok() const noexcept -> bool {
    return _conn.is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::get_stream_id() noexcept
  -> optionally_valid<stream_id_t> {
    if(const auto res{_conn.exec("SELECT eagilog.start_stream()")}) {
        return res.get_value_as<stream_id_t>(0, 0);
    }
    return {};
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& si,
  const begin_info&) noexcept -> bool {
    if(const auto id{get_stream_id()}) {
        si.set_id(*id);
        return true;
    }
    return false;
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
  const libpq_stream_sink& si,
  const finish_info& info) noexcept -> bool {
    if(const auto res{_conn.exec(
         "SELECT eagilog.finish_stream($1::INTEGER, $2::BOOLEAN)",
         std::to_string(si.id()),
         info.clean ? string_view{"TRUE"} : string_view{"FALSE"})}) {
        return res.tuples_ok();
    }
    return false;
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::update() noexcept {
    if(not is_conn_ok()) {
        if(_should_reconnect.is_expired()) {
            if(_conn.reconnect()) {
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
