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
using stream_id_t = std::size_t;
//------------------------------------------------------------------------------
struct text_tree_stream_context {
    begin_info begin{};
    bool clean_shutdown{false};
};
class text_tree_sink;
//------------------------------------------------------------------------------
// sink factory
//------------------------------------------------------------------------------
class text_tree_sink_factory final
  : public std::enable_shared_from_this<text_tree_sink_factory>
  , public stream_sink_factory {
public:
    text_tree_sink_factory(unique_holder<text_output> output);

    void consume(const text_tree_sink&, const begin_info&) noexcept;
    void consume(const text_tree_sink&, const description_info&) noexcept;
    void consume(const text_tree_sink&, const declare_state_info&) noexcept;
    void consume(const text_tree_sink&, const active_state_info&) noexcept;
    void consume(
      const text_tree_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;
    void consume(const text_tree_sink&, const interval_info&) noexcept;
    void consume(const text_tree_sink&, const heartbeat_info&) noexcept;
    void consume(const text_tree_sink&, const finish_info&) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

private:
    void _write(const string_view) noexcept;
    void _conn_I(const text_tree_sink&) noexcept;
    void _conn_L(const text_tree_sink&) noexcept;
    void _conn_S(const text_tree_sink&) noexcept;
    void _conn_s(const text_tree_sink&) noexcept;
    void _conn_Z(const text_tree_sink&) noexcept;
    void _conn_T(const text_tree_sink&) noexcept;
    auto _get_stream_id() noexcept -> std::uintmax_t;

    stream_id_t _id_seq{0};
    unique_holder<text_output> _output;
    flat_map<stream_id_t, text_tree_stream_context> _streams;
    std::string _temp;
    bool _condensed{false};
};
//------------------------------------------------------------------------------
void text_tree_sink_factory::_write(const string_view text) noexcept {
    _output->write(text);
}
//------------------------------------------------------------------------------
text_tree_sink_factory::text_tree_sink_factory(unique_holder<text_output> output)
  : _output{std::move(output)} {
    _write("╮\n");
}
//------------------------------------------------------------------------------
// text_tree sink
//------------------------------------------------------------------------------
class text_tree_sink final : public stream_sink {
public:
    text_tree_sink(
      stream_id_t id,
      shared_holder<text_tree_sink_factory> parent) noexcept;

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
    shared_holder<text_tree_sink_factory> _parent;
    message_formatter _formatter;
    float_seconds _prev_offs{};
    identifier _root;
    begin_info _begin{};

    flat_map<std::tuple<identifier_t, std::uint64_t>, interval_info> _intervals;
};
//------------------------------------------------------------------------------
auto text_tree_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
auto text_tree_sink::root() const noexcept -> identifier {
    return _root;
}
//------------------------------------------------------------------------------
auto text_tree_sink::time_since_start(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(info.offset);
}
//------------------------------------------------------------------------------
auto text_tree_sink::time_since_prev(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(
      info.offset - _prev_offs);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_I(const text_tree_sink&) noexcept {
    _write("┊");
    for(auto& s : _streams) {
        (void)s;
        _write(" │");
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_L(const text_tree_sink& si) noexcept {
    _write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _write(" ┕");
            conn = false;
        } else if(conn) {
            _write(" │");
        } else {
            _write("━━");
        }
    }
    _write("━┥");
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_S(const text_tree_sink& si) noexcept {
    _write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _write("  ");
            conn = false;
        } else if(conn) {
            _write(" │");
        } else {
            _write("╭╯");
        }
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_s(const text_tree_sink& si) noexcept {
    _write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _write(" ");
            conn = false;
        } else if(conn) {
            _write(" │");
        } else {
            _write("╭╯");
        }
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_Z(const text_tree_sink& si) noexcept {
    _write("┊");
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _write(" ┝");
            conn = false;
        } else if(conn) {
            _write(" │");
        } else {
            _write("━━");
        }
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_conn_T(const text_tree_sink&) noexcept {
    _write("┝");
    for(auto& s : _streams) {
        (void)s;
        _write("━━");
    }
    _write("━┯━┥");
}
//------------------------------------------------------------------------------
// factory / output
//------------------------------------------------------------------------------
text_tree_sink::text_tree_sink(
  stream_id_t id,
  shared_holder<text_tree_sink_factory> parent) noexcept
  : _id{id}
  , _parent{std::move(parent)} {}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const begin_info& info) noexcept {
    _conn_I(s);
    _write("   ╭────────────╮\n");
    _conn_T(s);
    _write("starting log│\n");
    _conn_I(s);
    _write(" │ ╰────────────╯\n");
    _streams[s.id()].begin = info;
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const begin_info& info) noexcept {
    _begin = info;
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const description_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const description_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const declare_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const declare_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const active_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const active_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(_condensed) {
        _conn_Z(s);
        _write("━┑");
    } else {
        if(info.tag) {
            _conn_I(s);
            _write(" ╭──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬──────────┬────────────╮\n");
        } else {
            _conn_I(s);
            _write(" ╭──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬────────────╮\n");
        }
        _conn_Z(s);
        _write("━┥");
    }
    _write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│");
    _write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│");
    _write(padded_to(9, enumerator_name(info.severity)));
    _write("│");
    _write(padded_to(10, s.root()));
    _write("│");
    _write(padded_to(10, info.source));
    _write("│");
    if(info.tag) {
        _write(padded_to(10, info.tag));
        _write("│");
    }
    _write(padded_to(12, format_instance(info.instance, _temp)));
    _write("│");
    _write("\n");
    if(info.tag) {
        _conn_I(s);
        _write(" ╰┬─────────┴──────────┴─────────┴");
        _write("──────────┴──────────┴──────────┴────────────╯\n");
    } else {
        _conn_I(s);
        _write(" ╰┬─────────┴──────────┴─────────┴");
        _write("──────────┴──────────┴────────────╯\n");
    }
    const auto arg_count{info.args.size()};

    _conn_I(s);
    _write("  ╰");
    if(arg_count > 0) {
        _write("─┐");
    } else {
        _write("╼ ");
    }
    _write(formatter.format(info));
    _write("\n");

    for(const int ai : integer_range(arg_count)) {
        _conn_I(s);
        _write("    ");
        if(ai + 1 == arg_count) {
            _write("╰");
        } else {
            _write("├");
        }
        const auto& arg{info.args[ai]};
        _write("─╼ ");
        _write(arg.name.name());
        _write(": ");
        _write(formatter.format(arg, false));
        _write("\n");
    }
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const message_info& info) noexcept {
    if(not _root) {
        _root = info.source;
    }
    _parent->consume(*this, info, _formatter);
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const interval_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const interval_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const heartbeat_info& info) noexcept {
    _conn_I(s);
    _write(" ╭──────────┬──────────┬──────────╮\n");
    _conn_Z(s);
    _write("━┥");
    _write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│");
    _write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│heart-beat│\n");
    _conn_I(s);
    _write(" ╰──────────┴──────────┴──────────╯\n");
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const heartbeat_info& info) noexcept {
    _parent->consume(*this, info);
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const finish_info& info) noexcept {
    _conn_I(s);
    _write(" ╭──────────┬──────────┬──────────┬───────────┬─────────╮\n");
    _conn_L(s);
    _write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│");
    _write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│");
    _write(padded_to(10, s.root()));
    _write("│closing log│");
    if(info.clean) {
        _write(" success ");
    } else {
        _write(" failed  ");
    }
    _write("│\n");
    _conn_S(s);
    _write(" ╰──────────┴──────────┴──────────┴───────────┴─────────╯\n");
    _conn_s(s);
    _write("\n");
    _streams.erase(s.id());
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const finish_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_get_stream_id() noexcept -> stream_id_t {
    return ++_id_seq;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::make_stream() noexcept
  -> unique_holder<stream_sink> {
    return {hold<text_tree_sink>, _get_stream_id(), shared_from_this()};
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::update() noexcept {
    _output->flush();
}
//------------------------------------------------------------------------------
// factory functions
//------------------------------------------------------------------------------
auto make_text_tree_sink_factory(main_ctx&, unique_holder<text_output> output)
  -> shared_holder<stream_sink_factory> {
    return {hold<text_tree_sink_factory>, std::move(output)};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs

