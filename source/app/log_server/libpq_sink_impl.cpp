/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
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
namespace pq {
//------------------------------------------------------------------------------
using connection_handle = std::unique_ptr<PGconn, void (*)(PGconn*)>;
using result_handle = std::unique_ptr<PGresult, void (*)(PGresult*)>;
//------------------------------------------------------------------------------
class result : result_handle {
public:
    result(PGresult* res) noexcept
      : result_handle{res, ::PQclear} {}

    auto is_ok() const noexcept -> bool {
        return result_handle::operator bool() and
               (::PQresultStatus(get()) == PGRES_TUPLES_OK);
    }

    explicit operator bool() const noexcept {
        return is_ok();
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
        return connection_handle::operator bool() and
               (::PQstatus(get()) == CONNECTION_OK);
    }

    explicit operator bool() const noexcept {
        return is_ok();
    }

    auto error_message() const noexcept -> string_view {
        return string_view{::PQerrorMessage(get())};
    }

    auto reconnect() noexcept -> bool {
        ::PQreset(get());
        return is_ok();
    }

    auto execute(string_view sql) const noexcept -> result {
        if(is_ok()) {
            return {::PQexec(get(), c_str(sql))};
        }
        return {nullptr};
    }

    template <typename... Args>
    auto execute(string_view sql, Args&&... args) const noexcept
      -> std::enable_if_t<(sizeof...(Args) > 0), result> {
        if(is_ok()) {
            return _exec_p(
              sql,
              std::array<string_view, sizeof...(Args)>{
                {_conv(std::forward<Args>(args))...}},
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

    static auto _conv(const string_view arg) noexcept -> string_view {
        return arg;
    }

    static auto _conv(const identifier arg) noexcept {
        return arg.name().str();
    }

    static auto _conv(const std::integral auto arg) noexcept {
        return std::to_string(arg);
    }

    static auto _conv(const std::floating_point auto arg) noexcept {
        return std::to_string(arg);
    }

    static auto _conv(const std::chrono::duration<float> arg) noexcept {
        return std::to_string(arg.count());
    }

    static auto _conv(const bool arg) noexcept -> string_view {
        return arg ? string_view{"TRUE"} : string_view{"FALSE"};
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
    auto get_entry_id(libpq_stream_sink&, const message_info&) noexcept
      -> optionally_valid<span_size_t>;

    auto consume(libpq_stream_sink&, const begin_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const description_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const declare_state_info&) noexcept
      -> bool;
    auto consume(libpq_stream_sink&, const active_state_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const message_info&) noexcept -> bool;
    auto consume(
      libpq_stream_sink&,
      const span_size_t entry_id,
      const message_info::arg_info&) noexcept -> bool;
    auto consume(libpq_stream_sink&, const aggregate_interval_info&) noexcept
      -> bool;
    auto consume(libpq_stream_sink&, const heartbeat_info&) noexcept -> bool;
    auto consume(const libpq_stream_sink&, const finish_info&) noexcept -> bool;

    void set_stream_application_id(stream_id_t, identifier app_id) noexcept;

private:
    auto add_entry_arg_boolean(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;
    auto add_entry_arg_integer(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;
    auto add_entry_arg_unsigned(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;
    auto add_entry_arg_float(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;
    auto add_entry_arg_duration(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;
    auto add_entry_arg_string(
      libpq_stream_sink& stream,
      const span_size_t entry_id,
      const message_info::arg_info& info) noexcept -> bool;

    pq::connection _conn;

    backing_off_timeout _should_reconnect{
      std::chrono::seconds{1},
      std::chrono::minutes{1}};

    void _setup_special_args() noexcept;
    void _handle_special_arg(
      const span_size_t stream_id,
      const message_info& msg,
      const message_info::arg_info& arg) noexcept;

    void _handle_object_created(
      const span_size_t stream_id,
      const message_info& msg,
      bool destroy_parent) noexcept;

    void _handle_object_destroyed(
      const span_size_t stream_id,
      const message_info& msg) noexcept;

    void _handle_lifetime_msg(
      const span_size_t stream_id,
      const message_info& msg) noexcept;

    struct _special_arg {
        std::string_view attribute_name;
        span_size_t max_length{0};
    };
    flat_map<identifier_t, flat_map<identifier_t, _special_arg>> _special_args;
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
      aggregate_interval_info,
      heartbeat_info,
      finish_info>>
      _backlog;

    aggregate_intervals _intervals{std::chrono::seconds{120}};
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
    if(not _root) {
        _root = info.source;
        _parent->set_stream_application_id(_id, _root);
    }
    _dispatch(info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink::consume(const interval_info& info) noexcept {
    if(const auto aggregate{_intervals.update(info)}) {
        _dispatch(*aggregate);
        _intervals.reset(aggregate);
    }
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
void libpq_stream_sink_factory::_setup_special_args() noexcept {
    _special_args[id_v("ProgArgs")][id_v("cmd")] = {
      .attribute_name = "command", .max_length = 128};
    _special_args[id_v("OSInfo")][id_v("osCodeName")] = {
      .attribute_name = "os_name", .max_length = 64};
    _special_args[id_v("Instance")][id_v("osPID")] = {
      .attribute_name = "os_pid"};
    _special_args[id_v("Instance")][id_v("hostname")] = {
      .attribute_name = "hostname", .max_length = 64};
    _special_args[id_v("GitInfo")][id_v("gitHashId")] = {
      .attribute_name = "git_hash", .max_length = 64};
    _special_args[id_v("GitInfo")][id_v("gitVersion")] = {
      .attribute_name = "git_version", .max_length = 32};
    _special_args[id_v("BuildInfo")][id_v("onValgrind")] = {
      .attribute_name = "running_on_valgrind"};
    _special_args[id_v("BuildInfo")][id_v("lowProfile")] = {
      .attribute_name = "low_profile_build"};
    _special_args[id_v("BuildInfo")][id_v("debug")] = {
      .attribute_name = "debug_build"};
    _special_args[id_v("Compiler")][id_v("complrName")] = {
      .attribute_name = "compiler", .max_length = 32};
    _special_args[id_v("Compiler")][id_v("archtcture")] = {
      .attribute_name = "architecture", .max_length = 32};
    _special_args[id_v("asignEptId")][id_v("eptId")] = {
      .attribute_name = "endpoint_id"};
    _special_args[id_v("cnfrmEptId")][id_v("eptId")] = {
      .attribute_name = "endpoint_id"};
}
//------------------------------------------------------------------------------
libpq_stream_sink_factory::libpq_stream_sink_factory(
  main_ctx&,
  string_view conn_params) noexcept
  : _conn{pq::connection::connect(conn_params)} {
    _setup_special_args();
}
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
    if(const auto res{_conn.execute("SELECT eagilog.start_stream()")}) {
        return res.get_value_as<stream_id_t>(0, 0);
    }
    return {};
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const begin_info&) noexcept -> bool {
    if(const auto id{get_stream_id()}) {
        stream.set_id(*id);
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
  libpq_stream_sink& stream,
  const declare_state_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.declare_stream_state($1::INTEGER, $2, $3, $4, $5)",
        stream.id(),
        info.source,
        info.state_tag,
        info.begin_tag,
        info.end_tag)
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const active_state_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.make_stream_state_active($1::INTEGER, $2, $3)",
        stream.id(),
        info.source,
        info.tag)
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::get_entry_id(
  libpq_stream_sink& stream,
  const message_info& info) noexcept -> optionally_valid<span_size_t> {
    if(info.tag) {
        return _conn
          .execute(
            "SELECT eagilog.add_entry("
            "	$1::INTEGER,"
            "	$2,"
            "	$3::BIGINT,"
            "	$4,"
            "	$5,"
            "	$6,"
            "	$7::INTERVAL)",
            stream.id(),
            info.source,
            info.instance,
            enumerator_name(info.severity),
            info.tag,
            info.format,
            info.offset)
          .get_value_as<span_size_t>(0, 0);
    } else {
        return _conn
          .execute(
            "SELECT eagilog.add_entry("
            "	$1::INTEGER,"
            "	$2,"
            "	$3::BIGINT,"
            "	$4,"
            "	NULL,"
            "	$5,"
            "	$6::INTERVAL)",
            stream.id(),
            info.source,
            info.instance,
            enumerator_name(info.severity),
            info.format,
            info.offset)
          .get_value_as<span_size_t>(0, 0);
    }
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_boolean(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.add_entry_arg_boolean("
        "	$1::INTEGER, $2, $3, $4::BOOLEAN)",
        entry_id,
        info.name,
        info.tag,
        info.value_bool().or_false())
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_integer(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.add_entry_arg_integer("
        "	$1::INTEGER, $2, $3, $4::NUMERIC)",
        entry_id,
        info.name,
        info.tag,
        info.value_int64().or_default())
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_unsigned(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.add_entry_arg_integer("
        "	$1::INTEGER, $2, $3, $4::NUMERIC)",
        entry_id,
        info.name,
        info.tag,
        info.value_uint64().or_default())
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_float(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.add_entry_arg_float("
        "	$1::INTEGER, $2, $3, $4::DOUBLE PRECISION)",
        entry_id,
        info.name,
        info.tag,
        info.value_float().or_default())
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_duration(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.add_entry_arg_duration("
        "	$1::INTEGER, $2, $3, $4::INTERVAL)",
        entry_id,
        info.name,
        info.tag,
        info.value_duration().or_default())
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::add_entry_arg_string(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    if(const auto val{info.value_string()}) {
        if(not val->empty()) {
            return _conn
              .execute(
                "SELECT eagilog.add_entry_arg_string("
                "	$1::INTEGER, $2, $3, $4)",
                entry_id,
                info.name,
                info.tag,
                *val)
              .is_ok();
        }
    }
    return true;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const span_size_t entry_id,
  const message_info::arg_info& info) noexcept -> bool {
    if(info.min and info.max) {
        _conn
          .execute(
            "SELECT eagilog.add_entry_arg_min_max("
            "	$1::INTEGER, $2, $3::DOUBLE PRECISION, $4::DOUBLE PRECISION)",
            entry_id,
            info.name,
            *info.min,
            *info.max)
          .is_ok();
    }
    switch(info.tag.value()) {
        case id_v("duration"):
            return add_entry_arg_duration(stream, entry_id, info);
        default:
            break;
    }
    if(std::holds_alternative<float_seconds>(info.value)) {
        return add_entry_arg_duration(stream, entry_id, info);
    }
    if(std::holds_alternative<float>(info.value)) {
        return add_entry_arg_float(stream, entry_id, info);
    }
    if(std::holds_alternative<std::uint64_t>(info.value)) {
        return add_entry_arg_unsigned(stream, entry_id, info);
    }
    if(std::holds_alternative<std::int64_t>(info.value)) {
        return add_entry_arg_integer(stream, entry_id, info);
    }
    if(std::holds_alternative<bool>(info.value)) {
        return add_entry_arg_boolean(stream, entry_id, info);
    }
    return add_entry_arg_string(stream, entry_id, info);
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::set_stream_application_id(
  stream_id_t stream_id,
  identifier app_id) noexcept {
    _conn.execute(
      "SELECT eagilog.set_stream_application_id($1::INTEGER, $2)",
      stream_id,
      app_id);
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::_handle_special_arg(
  const span_size_t stream_id,
  const message_info& msg,
  const message_info::arg_info& arg) noexcept {
    if(const auto found_msg{find(_special_args, msg.tag.value())}) {
        if(const auto found_arg{find(*found_msg, arg.name.value())}) {
            const auto query{std::format(
              "SELECT eagilog.set_stream_{}($1::INTEGER, $2)",
              found_arg->attribute_name)};

            if(const auto int_val{arg.value_int64()}) {
                _conn.execute(query, stream_id, *int_val);
            } else if(const auto bool_val{arg.value_bool()}) {
                _conn.execute(query, stream_id, *bool_val);
            } else if(const auto str_val{arg.value_string()}) {
                if(found_arg->max_length > 0) {
                    _conn.execute(
                      query, stream_id, head(*str_val, found_arg->max_length));
                } else {
                    _conn.execute(query, stream_id, *str_val);
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::_handle_object_created(
  const span_size_t stream_id,
  const message_info& msg,
  bool destroy_parent) noexcept {
    const auto srch_arg{
      [&](identifier id) -> optional_reference<const message_info::arg_info> {
          if(const auto pos{std::find_if(
               msg.args.begin(),
               msg.args.end(),
               [id, &msg](const auto& arg) { return arg.name == id; })};
             pos != msg.args.end()) {
              return {*pos};
          }
          return {};
      }};
    const auto id_arg{srch_arg("sourceId")};
    const auto inst_arg{srch_arg("sourceInst")};
    if(id_arg and inst_arg) {
        const auto parent_id{id_arg->value_string()};
        const auto parent_inst{inst_arg->value_uint64()};
        if(parent_id and parent_inst) {
            _conn.execute(
              "SELECT eagilog.create_object("
              "	$1, $2, $3, $4, $5, $6::INTERVAL, $7::BOOLEAN)",
              stream_id,
              msg.source,
              msg.instance,
              *parent_id,
              *parent_inst,
              msg.offset,
              destroy_parent);
        }
    }
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::_handle_object_destroyed(
  const span_size_t stream_id,
  const message_info& msg) noexcept {
    _conn.execute(
      "SELECT eagilog.destroy_object($1, $2, $3, $4::INTERVAL)",
      stream_id,
      msg.source,
      msg.instance,
      msg.offset);
}
//------------------------------------------------------------------------------
void libpq_stream_sink_factory::_handle_lifetime_msg(
  const span_size_t stream_id,
  const message_info& msg) noexcept {
    if(msg.tag.matches("objCreate")) {
        _handle_object_created(stream_id, msg, false);
    } else if(msg.tag.matches("objMove")) {
        _handle_object_created(stream_id, msg, true);
    } else if(msg.tag.matches("objCopy")) {
        _handle_object_created(stream_id, msg, false);
    } else if(msg.tag.matches("assignMove")) {
        _handle_object_destroyed(stream_id, msg);
        _handle_object_created(stream_id, msg, true);
    } else if(msg.tag.matches("assignCopy")) {
        _handle_object_destroyed(stream_id, msg);
        _handle_object_created(stream_id, msg, false);
    } else if(msg.tag.matches("objDestroy")) {
        _handle_object_destroyed(stream_id, msg);
    }
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const message_info& info) noexcept -> bool {
    if(const auto entry_id{get_entry_id(stream, info)}) {
        for(const auto& arg : info.args) {
            consume(stream, *entry_id, arg);
            _handle_special_arg(stream.id(), info, arg);
        }
        _handle_lifetime_msg(stream.id(), info);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const aggregate_interval_info& info) noexcept -> bool {
    const auto to_ms{[](auto d) {
        return std::chrono::duration_cast<
                 std::chrono::duration<float, std::milli>>(d)
          .count();
    }};
    return _conn
      .execute(
        "SELECT eagilog.add_profile_interval("
        "	$1,"
        "	$2,"
        "	$3,"
        "	$4,"
        "	$5::INTERVAL,"
        "	$6,"
        "	$7,"
        "	$8)",
        stream.id(),
        info.tag,
        info.instance,
        info.hit_count(),
        info.hit_interval(),
        to_ms(info.min_duration()),
        to_ms(info.avg_duration()),
        to_ms(info.max_duration()))
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  libpq_stream_sink& stream,
  const heartbeat_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.stream_heartbeat($1::INTEGER, $2::INTERVAL)",
        stream.id(),
        info.offset)
      .is_ok();
}
//------------------------------------------------------------------------------
auto libpq_stream_sink_factory::consume(
  const libpq_stream_sink& stream,
  const finish_info& info) noexcept -> bool {
    return _conn
      .execute(
        "SELECT eagilog.finish_stream($1::INTEGER, $2::BOOLEAN)",
        stream.id(),
        info.clean)
      .is_ok();
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
