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
class lib_init {
public:
    lib_init() noexcept {
        ::curl_global_init(CURL_GLOBAL_ALL);
    }

    ~lib_init() noexcept {
        ::curl_global_cleanup();
    }
};
//------------------------------------------------------------------------------
using slist_handle = std::unique_ptr<::curl_slist, void (*)(::curl_slist*)>;
//------------------------------------------------------------------------------
class string_list : public slist_handle {
public:
    string_list() noexcept
      : slist_handle{nullptr, ::curl_slist_free_all} {}

    auto reset() noexcept -> string_list& {
        slist_handle::reset();
        return *this;
    }

    auto append(string_view str) noexcept -> string_list& {
        slist_handle::reset(::curl_slist_append(get(), c_str(str).c_str()));
        return *this;
    }
};
//------------------------------------------------------------------------------
using connection_handle = std::unique_ptr<::CURL, void (*)(::CURL*)>;
//------------------------------------------------------------------------------
class connection : connection_handle {
public:
    connection(::CURL* conn, shared_holder<lib_init> lib) noexcept
      : connection_handle{conn, ::curl_easy_cleanup}
      , _lib{std::move(lib)} {}

    auto is_ok() const noexcept -> bool {
        return connection_handle::operator bool();
    }

    explicit operator bool() const noexcept {
        return is_ok();
    }

    auto reinit() noexcept -> connection& {
        ::curl_easy_reset(get());
        _headers.reset();
        return *this;
    }

    auto post_data(const memory::const_block data) noexcept -> connection& {
        ::curl_easy_setopt(get(), CURLOPT_POST, 1L);
        ::curl_easy_setopt(get(), CURLOPT_POSTFIELDSIZE, data.size());
        ::curl_easy_setopt(get(), CURLOPT_POSTFIELDS, data.data());
        return *this;
    }

    auto post_data(const string_view str) noexcept -> connection& {
        return post_data(as_bytes(str));
    }

    auto set_url(const string_view locator) noexcept -> connection& {
        ::curl_easy_setopt(get(), CURLOPT_URL, c_str(locator).c_str());
        return *this;
    }

    auto add_header(string_view header) noexcept -> connection& {
        _headers.append(header);
        return *this;
    }

    auto perform() noexcept -> bool {
        ::curl_easy_setopt(get(), CURLOPT_HTTPHEADER, _headers.get());
        return ::curl_easy_perform(get()) == CURLE_OK;
    }

private:
    shared_holder<lib_init> _lib;
    string_list _headers;
};
//------------------------------------------------------------------------------
class library {
public:
    auto connect() const noexcept {
        return connection{::curl_easy_init(), _lib};
    }

private:
    shared_holder<lib_init> _lib{default_selector};
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
    influxdb_stream_sink_factory(
      main_ctx&,
      const string_view conn_params) noexcept;

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
    auto consume(influxdb_stream_sink&, const aggregate_interval_info&) noexcept
      -> bool;
    auto consume(influxdb_stream_sink&, const heartbeat_info&) noexcept -> bool;
    auto consume(const influxdb_stream_sink&, const finish_info&) noexcept
      -> bool;

private:
    void _append_data(
      influxdb_stream_sink&,
      const message_info&,
      const message_info::arg_info&) noexcept;

    auto _write_data() noexcept -> bool;

    auto _make_write_url() noexcept -> std::string;
    auto _make_auth_hdr() noexcept -> std::string;

    const url _conn_params;
    const std::string _write_url{_make_write_url()};
    const std::string _auth_hdr{_make_auth_hdr()};

    curl::library _lib_curl;
    curl::connection _conn;
    std::string _entry_batch;
    std::string _entry_buffer;
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

    auto time_since_start(const message_info&) noexcept
      -> std::chrono::nanoseconds;

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
    begin_info _begin{};
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
auto influxdb_stream_sink::time_since_start(const message_info& info) noexcept
  -> std::chrono::nanoseconds {
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    return duration_cast<nanoseconds>(_begin.start.time_since_epoch()) +
           duration_cast<nanoseconds>(info.offset);
}
//------------------------------------------------------------------------------
void influxdb_stream_sink::consume(const begin_info& info) noexcept {
    _begin = info;
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
    if(not _root) {
        _root = info.source;
    }
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
influxdb_stream_sink_factory::influxdb_stream_sink_factory(
  main_ctx&,
  const string_view conn_params) noexcept
  : _conn_params{to_string(conn_params)}
  , _conn{_lib_curl.connect()} {}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::_make_write_url() noexcept -> std::string {
    const auto& q{_conn_params.query()};
    std::string result;
    append_to(_conn_params.scheme().value_or("http"), result);
    result.append("://");
    append_to(_conn_params.domain().value_or("localhost"), result);
    result.append(":");
    append_to(_conn_params.port_str().value_or("8086"), result);
    append_to(_conn_params.path().as_string("/", false), result);
    result.append("/write?org=");
    append_to(q.arg_value("org").value_or("EAGine"), result);
    result.append("&bucket=");
    append_to(q.arg_value("bucket").value_or("eagilog"), result);
    return result;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::_make_auth_hdr() noexcept -> std::string {
    std::string result{"Authorization: Token "};
    append_to(_conn_params.query().arg_value("token").or_default(), result);
    return result;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::make_stream() noexcept
  -> unique_holder<stream_sink> {
    return {hold<influxdb_stream_sink>, shared_from_this()};
}
//------------------------------------------------------------------------------
void influxdb_stream_sink_factory::update() noexcept {}
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
void influxdb_stream_sink_factory::_append_data(
  influxdb_stream_sink& stream,
  const message_info& msg,
  const message_info::arg_info& arg) noexcept {
    if(const auto val{arg.value_float()}) {
        if(arg.min and arg.max) {
            append_to(
              std::format(
                "arg,stream={},app={},source={},msg_tag={},name={},arg_tag={},"
                "instance={} value={} {}\n",
                stream.id(),
                std::string_view{stream.root().name()},
                std::string_view{msg.source.name()},
                std::string_view{msg.tag.name()},
                std::string_view{arg.name.name()},
                std::string_view{arg.tag.name()},
                msg.instance,
                *val,
                stream.time_since_start(msg).count()),
              _entry_batch);
        } else {
            append_to(
              std::format(
                "arg,stream={},app={},source={},msg_tag={},name={},arg_tag={},"
                "instance={} value={},min={},max={} {}\n",
                stream.id(),
                std::string_view{stream.root().name()},
                std::string_view{msg.source.name()},
                std::string_view{msg.tag.name()},
                std::string_view{arg.name.name()},
                std::string_view{arg.tag.name()},
                msg.instance,
                *val,
                *(arg.min),
                *(arg.max),
                stream.time_since_start(msg).count()),
              _entry_batch);
        }
    }
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::_write_data() noexcept -> bool {
    if(not _entry_buffer.empty()) {
        if(not _conn.reinit()
                 .set_url(_write_url)
                 .add_header(_auth_hdr)
                 .post_data(_entry_buffer)
                 .perform()) {
            return false;
        }
        _entry_buffer.clear();
    }
    append_to(_entry_batch, _entry_buffer);
    _entry_batch.clear();
    return true;
}
//------------------------------------------------------------------------------
auto influxdb_stream_sink_factory::consume(
  influxdb_stream_sink& stream,
  const message_info& info) noexcept -> bool {
    for(const auto& arg : info.args) {
        _append_data(stream, info, arg);
    }
    _write_data();
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
#if EAGINE_USE_LIBCURL
    if(conn_params.empty()) {
        conn_params = "http://admin:admin@localhost:8086/api/v2";
    }
    return {hold<influxdb_stream_sink_factory>, ctx, conn_params};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
