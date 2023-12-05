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
      text_tree_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;
    void consume(const text_tree_sink&, const aggregate_interval_info&) noexcept;
    void consume(const text_tree_sink&, const heartbeat_info&) noexcept;
    void consume(const text_tree_sink&, const finish_info&) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

private:
    auto _get_stream_id() noexcept -> std::uintmax_t;

    auto _symbol_fatal() const noexcept -> string_view;
    auto _symbol_error() const noexcept -> string_view;
    auto _symbol_warning() const noexcept -> string_view;
    auto _symbol_change() const noexcept -> string_view;
    auto _symbol_stat() const noexcept -> string_view;
    auto _symbol_debug() const noexcept -> string_view;
    auto _symbol_trace() const noexcept -> string_view;
    auto _symbol_create() const noexcept -> string_view;
    auto _symbol_destroy() const noexcept -> string_view;
    auto _symbol_interrupt() const noexcept -> string_view;
    auto _symbol_locked() const noexcept -> string_view;
    auto _symbol_unlocked() const noexcept -> string_view;
    auto _symbol_board() const noexcept -> string_view;
    auto _symbol_message() const noexcept -> string_view;
    auto _symbol(const message_info&) const noexcept -> string_view;
    auto _symbol_interval() const noexcept -> string_view;
    auto _symbol_heart_beat() const noexcept -> string_view;
    auto _symbol_start() const noexcept -> string_view;
    auto _symbol_finish_ok() const noexcept -> string_view;
    auto _symbol_finish_fail() const noexcept -> string_view;

    void _flush() noexcept;
    auto _write(const string_view) noexcept -> text_tree_sink_factory&;
    auto _conn_I(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_L(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_S(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_s(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_Z(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_T(const text_tree_sink&) noexcept -> text_tree_sink_factory&;

    auto _is_a(
      identifier root,
      identifier source,
      identifier tag,
      const text_tree_sink&,
      const message_info&) const noexcept -> bool;

    // specialized format functions
    void _format_default_heading(
      const text_tree_sink&,
      const string_view symbol,
      const message_info&,
      message_formatter& formatter) noexcept;
    void _format_default_message(
      const text_tree_sink&,
      const message_info&,
      message_formatter& formatter,
      bool) noexcept;
    void _format_default_arg(
      const text_tree_sink&,
      const message_info::arg_info&,
      message_formatter& formatter,
      bool) noexcept;
    void _format_default(
      const text_tree_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;
    void _format_solved_board(
      text_tree_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;

    stream_id_t _id_seq{0};
    unique_holder<text_output> _output;
    flat_map<stream_id_t, text_tree_stream_context> _streams;
    std::string _temp;
    bool _condensed{false};
};
//------------------------------------------------------------------------------
// symbols
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_fatal() const noexcept -> string_view {
    return {" \U0001F4A5 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_error() const noexcept -> string_view {
    return {" \U0001F525 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_warning() const noexcept -> string_view {
    return {" \U0001F6A9 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_change() const noexcept -> string_view {
    return {" \U0001F503 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_stat() const noexcept -> string_view {
    return {" \U0001F4CA "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_debug() const noexcept -> string_view {
    return {" \U0001F41E "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_trace() const noexcept -> string_view {
    return {" \U0001F50E "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_create() const noexcept -> string_view {
    return {" \U00002728 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_destroy() const noexcept -> string_view {
    return {" \U0001F635 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_interrupt() const noexcept -> string_view {
    return {" \U0001F6D1 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_locked() const noexcept -> string_view {
    return {" \U0001F512 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_unlocked() const noexcept -> string_view {
    return {" \U0001F513 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_message() const noexcept -> string_view {
    return {" \U0001F4E8 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_board() const noexcept -> string_view {
    return {" ▦  "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol(const message_info& info) const noexcept
  -> string_view {
    if(info.tag.matches("objCreate")) {
        return _symbol_create();
    }
    if(info.tag.matches("objDestroy")) {
        return _symbol_destroy();
    }
    if(info.tag.matches("reqRutrPwd")) {
        return _symbol_locked();
    }
    if(info.tag.matches("vfyRutrPwd")) {
        return _symbol_unlocked();
    }
    if(info.tag.matches("interrupt")) {
        return _symbol_interrupt();
    }
    switch(info.severity) {
        case log_event_severity::fatal:
            return _symbol_fatal();
        case log_event_severity::error:
            return _symbol_error();
        case log_event_severity::warning:
            return _symbol_warning();
        case log_event_severity::change:
            return _symbol_change();
        case log_event_severity::stat:
            return _symbol_stat();
        case log_event_severity::debug:
            return _symbol_debug();
        case log_event_severity::trace:
        case log_event_severity::backtrace:
            return _symbol_trace();
        default:
            break;
    }
    return _symbol_message();
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_interval() const noexcept -> string_view {
    return {" \U0001F55B "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_heart_beat() const noexcept
  -> string_view {
    return {" \U0001F493 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_start() const noexcept -> string_view {
    return {" \U0001F680 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_finish_ok() const noexcept -> string_view {
    return {" \U0001F642 "};
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_symbol_finish_fail() const noexcept
  -> string_view {
    return {" \U0001F480 "};
}
//------------------------------------------------------------------------------
// I/O
//------------------------------------------------------------------------------
void text_tree_sink_factory::_flush() noexcept {
    _output->flush();
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_write(const string_view text) noexcept
  -> text_tree_sink_factory& {
    _output->write(text);
    return *this;
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

    auto tiling() noexcept -> auto& {
        return _tiling;
    }

private:
    const stream_id_t _id;
    shared_holder<text_tree_sink_factory> _parent;
    message_formatter _formatter;
    float_seconds _prev_offs{};
    identifier _root;
    begin_info _begin{};

    aggregate_intervals _intervals{std::chrono::seconds{120}};

    tiling_state _tiling;
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
auto text_tree_sink_factory::_conn_I(const text_tree_sink&) noexcept
  -> text_tree_sink_factory& {
    _write("┊");
    for(auto& s : _streams) {
        (void)s;
        _write(" │");
    }
    return *this;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_conn_L(const text_tree_sink& si) noexcept
  -> text_tree_sink_factory& {
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
    return *this;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_conn_S(const text_tree_sink& si) noexcept
  -> text_tree_sink_factory& {
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
    return *this;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_conn_s(const text_tree_sink& si) noexcept
  -> text_tree_sink_factory& {
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
    return *this;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_conn_Z(const text_tree_sink& si) noexcept
  -> text_tree_sink_factory& {
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
    if(_condensed) {
        _write("━┑");
    } else {
        _write("━┥");
    }
    return *this;
}
//------------------------------------------------------------------------------
auto text_tree_sink_factory::_conn_T(const text_tree_sink&) noexcept
  -> text_tree_sink_factory& {
    _write("┝");
    for(auto& s : _streams) {
        (void)s;
        _write("━━");
    }
    _write("━┯━┥");
    return *this;
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
    _conn_I(s)._write("   ╭────┬────────────╮\n");
    _conn_T(s)._write(_symbol_start());
    _write("│starting log│\n");
    _conn_I(s)._write(" │ ╰────┴────────────╯\n")._flush();
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
auto text_tree_sink_factory::_is_a(
  identifier root,
  identifier source,
  identifier tag,
  const text_tree_sink& s,
  const message_info& info) const noexcept -> bool {
    if(root and root != s.root()) {
        return false;
    }
    if(source and source != info.source) {
        return false;
    }
    if(tag and tag != info.tag) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_format_default_heading(
  const text_tree_sink& s,
  const string_view symbol,
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(not _condensed) {
        if(info.tag) {
            _conn_I(s)._write(" ╭────┬──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬──────────┬────────────╮\n");
        } else {
            _conn_I(s)._write(" ╭────┬──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬────────────╮\n");
        }
    }
    _conn_Z(s), _write(symbol);
    _write("│")._write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│")._write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│")._write(padded_to(9, enumerator_name(info.severity)));
    _write("│")._write(padded_to(10, s.root()));
    _write("│")._write(padded_to(10, info.source));
    if(info.tag) {
        _write("│")._write(padded_to(10, info.tag));
    }
    _write("│")._write(padded_to(12, format_instance(info.instance, _temp)));
    _write("│\n");
    if(info.tag) {
        _conn_I(s)._write(" ╰┬───┴──────────┴──────────┴─────────┴");
        _write("──────────┴──────────┴──────────┴────────────╯\n");
    } else {
        _conn_I(s)._write(" ╰┬───┴──────────┴──────────┴─────────┴");
        _write("──────────┴──────────┴────────────╯\n");
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_format_default_message(
  const text_tree_sink& s,
  const message_info& info,
  message_formatter& formatter,
  bool continues) noexcept {
    _conn_I(s)._write("  ╰");
    if(continues) {
        _write("─┐");
    } else {
        _write("╼ ");
    }
    _write(formatter.format(info));
    _write("\n");
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_format_default_arg(
  const text_tree_sink& s,
  const message_info::arg_info& arg,
  message_formatter& formatter,
  bool last) noexcept {
    _conn_I(s)._write("    ");
    if(last) {
        _write("╰");
    } else {
        _write("├");
    }
    _write("─╼ ")._write(arg.name.name())._write(": ");
    _write(formatter.format(arg, false))._write("\n");
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_format_default(
  const text_tree_sink& s,
  const message_info& info,
  message_formatter& formatter) noexcept {
    const auto arg_count{info.args.size()};

    _format_default_heading(s, _symbol(info), info, formatter);
    _format_default_message(s, info, formatter, arg_count > 0);

    for(const int ai : integer_range(arg_count)) {
        _format_default_arg(s, info.args[ai], formatter, ai + 1 == arg_count);
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::_format_solved_board(
  text_tree_sink& si,
  const message_info& info,
  message_formatter& formatter) noexcept {
    using ai = message_info::arg_info;
    const auto x{info.find_arg("x").member(&ai::value_int)};
    const auto y{info.find_arg("y").member(&ai::value_int)};
    const auto w{info.find_arg("width").member(&ai::value_int)};
    const auto h{info.find_arg("height").member(&ai::value_int)};
    const auto s{info.find_arg("celPerSide").member(&ai::value_int)};

    const auto rank{info.find_arg("rank")};

    bool has_args{false};

    if(x and y and w and h and s and rank) {
        using math::minimum;
        has_args = true;
        const auto adj{[&](int v) -> int {
            return *s * v;
        }};
        for(int cy = adj(*y); cy < minimum(adj(*y + 1), *h); ++cy) {
            for(int cx = adj(*x); cx < minimum(adj(*x + 1), *w); ++cx) {
                si.tiling().mark_cell_done(cx, cy, *w, *h);
            }
        }
    }

    _format_default_heading(si, _symbol_board(), info, formatter);
    _format_default_message(si, info, formatter, has_args);

    if(has_args) {
        for(const auto i : integer_range(18)) {
            _conn_I(si)._write("    ├");
            for(const auto j : integer_range(36)) {
                _write(si.tiling().pixel_glyph(j, i, 36, 18));
            }
            _write("┤\n");
        }

        if(const auto time{info.find_arg("time")}) {
            _format_default_arg(si, *time, formatter, false);
        }
        if(const auto helper{info.find_arg("helper")}) {
            _format_default_arg(si, *helper, formatter, false);
        }
        _format_default_arg(si, *rank, formatter, true);
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  text_tree_sink& s,
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(_is_a("SudokuTlng", "TilingNode", "solvdBoard", s, info)) {
        _format_solved_board(s, info, formatter);
    } else {
        _format_default(s, info, formatter);
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
  const aggregate_interval_info& info) noexcept {
    if(not _condensed) {
        _conn_I(s)._write(" ╭────┬──────────┬──────────┬────────────╮\n");
    }
    _conn_Z(s)._write(_symbol_interval());
    _write("│")._write(padded_to(10, s.root()));
    _write("│")._write(padded_to(10, info.tag));
    _write("│")._write(padded_to(12, format_instance(info.instance, _temp)));
    _write("│\n");
    _conn_I(s)._write(" ╰┬───┴──────────┴──────────┴────────────╯\n");
    _conn_I(s)._write("  ├─╼ min: ");
    _write(format_reltime_ns(info.min_duration()))._write("\n");
    _conn_I(s)._write("  ├─╼ avg: ");
    _write(format_reltime_ns(info.avg_duration()))._write("\n");
    _conn_I(s)._write("  ╰─╼ max: ");
    _write(format_reltime_ns(info.max_duration()))._write("\n")._flush();
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const interval_info& info) noexcept {
    if(const auto aggregate{_intervals.update(info)}) {
        _parent->consume(*this, *aggregate);
        _intervals.reset(aggregate);
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const heartbeat_info& info) noexcept {
    if(not _condensed) {
        _conn_I(s)._write(
          " ╭────┬──────────┬──────────┬──────────┬──────────╮\n");
    }
    _conn_Z(s)._write(_symbol_heart_beat());
    _write("│")._write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│")._write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│")._write(padded_to(10, s.root()));
    _write("│heart-beat│\n");
    _conn_I(s)
      ._write(" ╰────┴──────────┴──────────┴──────────┴──────────╯\n")
      ._flush();
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
    _write(" ╭────┬──────────┬──────────┬──────────┬───────────┬─────────╮\n");
    if(info.clean) {
        _conn_L(s)._write(_symbol_finish_ok());
    } else {
        _conn_L(s)._write(_symbol_finish_fail());
    }
    _write("│")._write(padded_to(10, format_reltime(s.time_since_start(info))));
    _write("│")._write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│")._write(padded_to(10, s.root()));
    _write("│closing log│");
    if(info.clean) {
        _write(" success ");
    } else {
        _write(" failed  ");
    }
    _write("│\n");
    _conn_S(s);
    _write(" ╰────┴──────────┴──────────┴──────────┴───────────┴─────────╯\n");
    _conn_s(s)._write("\n")._flush();
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
    _flush();
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

