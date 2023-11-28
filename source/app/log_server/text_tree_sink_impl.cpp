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
class text_tree_interval_info {
public:
    void update(const interval_info& i) noexcept;
    auto should_consume() noexcept -> bool;
    void reset() noexcept;

    auto min_duration() const noexcept -> std::chrono::nanoseconds;
    auto max_duration() const noexcept -> std::chrono::nanoseconds;
    auto avg_duration() const noexcept -> std::chrono::nanoseconds;

private:
    std::chrono::nanoseconds _duration_min{};
    std::chrono::nanoseconds _duration_max{};
    std::chrono::nanoseconds _duration_sum{};
    span_size_t _count{0};
};
//------------------------------------------------------------------------------
void text_tree_interval_info::update(const interval_info& i) noexcept {
    _duration_sum += i.duration;
    if(_count == 0) {
        _duration_min = i.duration;
    } else {
        _duration_min = math::minimum(_duration_min, i.duration);
    }
    _duration_max = math::maximum(_duration_max, i.duration);

    _count++;
}
//------------------------------------------------------------------------------
auto text_tree_interval_info::should_consume() noexcept -> bool {
    return _count >= 10;
}
//------------------------------------------------------------------------------
void text_tree_interval_info::reset() noexcept {
    _duration_sum = {};
    _count = 0;
}
//------------------------------------------------------------------------------
auto text_tree_interval_info::min_duration() const noexcept
  -> std::chrono::nanoseconds {
    return _duration_min;
}
//------------------------------------------------------------------------------
auto text_tree_interval_info::max_duration() const noexcept
  -> std::chrono::nanoseconds {
    return _duration_max;
}
//------------------------------------------------------------------------------
auto text_tree_interval_info::avg_duration() const noexcept
  -> std::chrono::nanoseconds {
    if(_count > 0) {
        return _duration_sum / _count;
    }
    return {};
}
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
    void consume(
      const text_tree_sink&,
      identifier tag,
      std::uint64_t instance,
      const text_tree_interval_info&) noexcept;
    void consume(const text_tree_sink&, const heartbeat_info&) noexcept;
    void consume(const text_tree_sink&, const finish_info&) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

private:
    void _flush() noexcept;
    auto _write(const string_view) noexcept -> text_tree_sink_factory&;
    auto _conn_I(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_L(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_S(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_s(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_Z(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _conn_T(const text_tree_sink&) noexcept -> text_tree_sink_factory&;
    auto _get_stream_id() noexcept -> std::uintmax_t;

    auto _is_a(
      identifier root,
      identifier source,
      identifier tag,
      const text_tree_sink&,
      const message_info&) const noexcept -> bool;

    // specialized format functions
    void _format_default_heading(
      const text_tree_sink&,
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

    flat_map<std::tuple<identifier_t, std::uint64_t>, text_tree_interval_info>
      _intervals;

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
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(_condensed) {
        _conn_Z(s)._write("━┑");
    } else {
        if(info.tag) {
            _conn_I(s)._write(" ╭──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬──────────┬────────────╮\n");
        } else {
            _conn_I(s)._write(" ╭──────────┬──────────┬─────────┬");
            _write("──────────┬──────────┬────────────╮\n");
        }
        _conn_Z(s)._write("━┥");
    }
    _write(padded_to(10, format_reltime(s.time_since_start(info))))._write("│");
    _write(padded_to(10, format_reltime(s.time_since_prev(info))))._write("│");
    _write(padded_to(9, enumerator_name(info.severity)))._write("│");
    _write(padded_to(10, s.root()))._write("│");
    _write(padded_to(10, info.source))._write("│");
    if(info.tag) {
        _write(padded_to(10, info.tag))._write("│");
    }
    _write(padded_to(12, format_instance(info.instance, _temp)))._write("│\n");
    if(info.tag) {
        _conn_I(s)._write(" ╰┬─────────┴──────────┴─────────┴");
        _write("──────────┴──────────┴──────────┴────────────╯\n");
    } else {
        _conn_I(s)._write(" ╰┬─────────┴──────────┴─────────┴");
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

    _format_default_heading(s, info, formatter);
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

    _format_default_heading(si, info, formatter);
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
  identifier tag,
  std::uint64_t instance,
  const text_tree_interval_info& info) noexcept {
    _conn_I(s)._write(" ╭──────────┬──────────┬────────────╮\n");
    _conn_Z(s)._write("━┥");
    _write(padded_to(10, s.root()))._write("│");
    _write(padded_to(10, tag))._write("│");
    _write(padded_to(12, format_instance(instance, _temp)))._write("│\n");
    _conn_I(s)._write(" ╰┬─────────┴──────────┴────────────╯\n");
    _conn_I(s)._write("  ├─╼ min: ");
    _write(format_reltime_ns(info.min_duration()))._write("\n");
    _conn_I(s)._write("  ├─╼ avg: ");
    _write(format_reltime_ns(info.avg_duration()))._write("\n");
    _conn_I(s)._write("  ╰─╼ max: ");
    _write(format_reltime_ns(info.max_duration()))._write("\n");
}
//------------------------------------------------------------------------------
void text_tree_sink::consume(const interval_info& info) noexcept {
    const auto key{std::make_tuple(info.tag.value(), info.instance)};
    auto pos{_intervals.find(key)};
    if(pos == _intervals.end()) {
        pos = _intervals.emplace(key, text_tree_interval_info{}).first;
    }
    auto& state{std::get<1>(*pos)};
    state.update(info);
    if(state.should_consume()) {
        _parent->consume(*this, info.tag, info.instance, state);
        state.reset();
    }
}
//------------------------------------------------------------------------------
void text_tree_sink_factory::consume(
  const text_tree_sink& s,
  const heartbeat_info& info) noexcept {
    _conn_I(s)._write(" ╭──────────┬──────────┬──────────╮\n");
    _conn_Z(s)._write("━┥");
    _write(padded_to(10, format_reltime(s.time_since_start(info))))._write("│");
    _write(padded_to(10, format_reltime(s.time_since_prev(info))));
    _write("│heart-beat│\n");
    _conn_I(s)._write(" ╰──────────┴──────────┴──────────╯\n");
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
    _conn_L(s)._write(padded_to(10, format_reltime(s.time_since_start(info))));
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
    _write(" ╰──────────┴──────────┴──────────┴───────────┴─────────╯\n");
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

