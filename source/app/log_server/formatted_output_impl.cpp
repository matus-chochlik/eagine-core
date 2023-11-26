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

import :interfaces;
import :utilities;

namespace eagine::logs {
//------------------------------------------------------------------------------
class formatted_sink final : public stream_sink {
public:
    formatted_sink(
      stream_id_t id,
      shared_holder<formatted_output> parent) noexcept;

    auto id() const noexcept -> stream_id_t;
    auto root() const noexcept -> identifier;
    auto time_since_start(const auto&) const noexcept
      -> std::chrono::microseconds;
    auto time_since_prev(const auto&) const noexcept
      -> std::chrono::microseconds;

    void consume(const begin_info&) noexcept final;
    void consume(const description_info&) noexcept final;
    void consume(const declare_state_info&) noexcept final;
    void consume(const active_state_info&) noexcept final;
    void consume(const message_info&) noexcept final;
    void consume(const interval_info&) noexcept final;
    void consume(const heartbeat_info&) noexcept final;
    void consume(const finish_info&) noexcept final;

private:
    const stream_id_t _id;
    shared_holder<formatted_output> _parent;
    message_formatter _formatter;
    float_seconds _prev_offs{};
    identifier _root;
    begin_info _begin{};

    flat_map<std::tuple<identifier_t, std::uint64_t>, interval_info> _intervals;
};
//------------------------------------------------------------------------------
auto formatted_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
auto formatted_sink::root() const noexcept -> identifier {
    return _root;
}
//------------------------------------------------------------------------------
auto formatted_sink::time_since_start(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(info.offset);
}
//------------------------------------------------------------------------------
auto formatted_sink::time_since_prev(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(
      info.offset - _prev_offs);
}
//------------------------------------------------------------------------------
// factory / output
//------------------------------------------------------------------------------
formatted_sink::formatted_sink(
  stream_id_t id,
  shared_holder<formatted_output> parent) noexcept
  : _id{id}
  , _parent{std::move(parent)} {}
//------------------------------------------------------------------------------
void formatted_output::_start() noexcept {
    write("╮\n");
}
//------------------------------------------------------------------------------
void formatted_output::_finish() noexcept {
    write("╯\n");
}
//------------------------------------------------------------------------------
void formatted_output::_conn_I(const formatted_sink&) noexcept {
    write("┊");
    for(auto& s : _streams) {
        (void)s;
        write(" │");
    }
}
//------------------------------------------------------------------------------
void formatted_output::_conn_L(const formatted_sink& si) noexcept {
    write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            write(" ┕");
            conn = false;
        } else if(conn) {
            write(" │");
        } else {
            write("━━");
        }
    }
    write("━┥");
}
//------------------------------------------------------------------------------
void formatted_output::_conn_S(const formatted_sink& si) noexcept {
    write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            write("  ");
            conn = false;
        } else if(conn) {
            write(" │");
        } else {
            write("╭╯");
        }
    }
}
//------------------------------------------------------------------------------
void formatted_output::_conn_s(const formatted_sink& si) noexcept {
    write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            write(" ");
            conn = false;
        } else if(conn) {
            write(" │");
        } else {
            write("╭╯");
        }
    }
}
//------------------------------------------------------------------------------
void formatted_output::_conn_Z(const formatted_sink& si) noexcept {
    write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            write(" ┝");
            conn = false;
        } else if(conn) {
            write(" │");
        } else {
            write("━━");
        }
    }
}
//------------------------------------------------------------------------------
void formatted_output::_conn_T(const formatted_sink&) noexcept {
    write("┝");
    for(auto& s : _streams) {
        (void)s;
        write("━━");
    }
    write("━┯━┥");
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const begin_info& info) noexcept {
    _conn_I(s);
    write("   ╭────────────╮\n");
    _conn_T(s);
    write("starting log│\n");
    _conn_I(s);
    write(" │ ╰────────────╯\n");
    _streams[s.id()].begin = info;
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const begin_info& info) noexcept {
    _begin = info;
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const description_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const description_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const declare_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const declare_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const active_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const active_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(_condensed) {
        _conn_Z(s);
        write("━┑");
    } else {
        if(info.tag) {
            _conn_I(s);
            write(" ╭──────────┬──────────┬─────────┬");
            write("──────────┬──────────┬──────────┬────────────╮\n");
        } else {
            _conn_I(s);
            write(" ╭──────────┬──────────┬─────────┬");
            write("──────────┬──────────┬────────────╮\n");
        }
        _conn_Z(s);
        write("━┥");
    }
    write(padded_to(10, format_reltime(s.time_since_start(info))));
    write("│");
    write(padded_to(10, format_reltime(s.time_since_prev(info))));
    write("│");
    write(padded_to(9, enumerator_name(info.severity)));
    write("│");
    write(padded_to(10, s.root()));
    write("│");
    write(padded_to(10, info.source));
    write("│");
    if(info.tag) {
        write(padded_to(10, info.tag));
        write("│");
    }
    write(padded_to(12, format_instance(info.instance, _temp)));
    write("│");
    write("\n");
    if(info.tag) {
        _conn_I(s);
        write(" ╰┬─────────┴──────────┴─────────┴");
        write("──────────┴──────────┴──────────┴────────────╯\n");
    } else {
        _conn_I(s);
        write(" ╰┬─────────┴──────────┴─────────┴");
        write("──────────┴──────────┴────────────╯\n");
    }
    const auto arg_count{info.args.size()};

    _conn_I(s);
    write("  ╰");
    if(arg_count > 0) {
        write("─┐");
    } else {
        write("╼ ");
    }
    write(formatter.format(info));
    write("\n");

    for(const int ai : integer_range(arg_count)) {
        _conn_I(s);
        write("    ");
        if(ai + 1 == arg_count) {
            write("╰");
        } else {
            write("├");
        }
        const auto& arg{info.args[ai]};
        write("─╼ ");
        write(arg.name.name());
        write(": ");
        write(formatter.format(arg, false));
        write("\n");
    }
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const message_info& info) noexcept {
    if(not _root) {
        _root = info.source;
    }
    _parent->consume(*this, info, _formatter);
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const interval_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const interval_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const heartbeat_info& info) noexcept {
    _conn_I(s);
    write(" ╭──────────┬──────────┬──────────╮\n");
    _conn_Z(s);
    write("━┥");
    write(padded_to(10, format_reltime(s.time_since_start(info))));
    write("│");
    write(padded_to(10, format_reltime(s.time_since_prev(info))));
    write("│heart-beat│\n");
    _conn_I(s);
    write(" ╰──────────┴──────────┴──────────╯\n");
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const heartbeat_info& info) noexcept {
    _parent->consume(*this, info);
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void formatted_output::consume(
  const formatted_sink& s,
  const finish_info& info) noexcept {
    _conn_I(s);
    write(" ╭──────────┬──────────┬──────────┬───────────┬─────────╮\n");
    _conn_L(s);
    write(padded_to(10, format_reltime(s.time_since_start(info))));
    write("│");
    write(padded_to(10, format_reltime(s.time_since_prev(info))));
    write("│");
    write(padded_to(10, s.root()));
    write("│closing log│");
    if(info.clean) {
        write(" success ");
    } else {
        write(" failed  ");
    }
    write("│\n");
    _conn_S(s);
    write(" ╰──────────┴──────────┴──────────┴───────────┴─────────╯\n");
    _conn_s(s);
    write("\n");
    _streams.erase(s.id());
}
//------------------------------------------------------------------------------
void formatted_sink::consume(const finish_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
auto formatted_output::_get_stream_id() noexcept -> stream_id_t {
    return ++_id_seq;
}
//------------------------------------------------------------------------------
auto formatted_output::make_stream() noexcept -> unique_holder<stream_sink> {
    return {hold<formatted_sink>, _get_stream_id(), shared_from_this()};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs

