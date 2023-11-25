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
class ostream_output;
//------------------------------------------------------------------------------
// stream sink
//------------------------------------------------------------------------------
using stream_id_t = std::size_t;
//------------------------------------------------------------------------------
class ostream_sink final : public stream_sink {
public:
    ostream_sink(stream_id_t id, shared_holder<ostream_output> parent) noexcept;

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
    shared_holder<ostream_output> _parent;
    message_formatter _formatter;
    float_seconds _prev_offs{};
    identifier _root;
    begin_info _begin{};

    flat_map<std::tuple<identifier_t, std::uint64_t>, interval_info> _intervals;
};
//------------------------------------------------------------------------------
auto ostream_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
auto ostream_sink::root() const noexcept -> identifier {
    return _root;
}
//------------------------------------------------------------------------------
auto ostream_sink::time_since_start(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(info.offset);
}
//------------------------------------------------------------------------------
auto ostream_sink::time_since_prev(const auto& info) const noexcept
  -> std::chrono::microseconds {
    return std::chrono::duration_cast<std::chrono::microseconds>(
      info.offset - _prev_offs);
}
//------------------------------------------------------------------------------
// factory / output
//------------------------------------------------------------------------------
struct ostream_context {
    begin_info begin{};
    bool clean_shutdown{false};
};
//------------------------------------------------------------------------------
class ostream_output final
  : public std::enable_shared_from_this<ostream_output>
  , public stream_sink_factory {
public:
    ostream_output(std::ostream& output) noexcept;
    ~ostream_output() noexcept final;

    void consume(const ostream_sink&, const begin_info&) noexcept;
    void consume(const ostream_sink&, const description_info&) noexcept;
    void consume(const ostream_sink&, const declare_state_info&) noexcept;
    void consume(const ostream_sink&, const active_state_info&) noexcept;
    void consume(
      const ostream_sink&,
      const message_info&,
      message_formatter& formatter) noexcept;
    void consume(const ostream_sink&, const interval_info&) noexcept;
    void consume(const ostream_sink&, const heartbeat_info&) noexcept;
    void consume(const ostream_sink&, const finish_info&) noexcept;

    auto make_stream() noexcept -> unique_holder<stream_sink> final;

private:
    auto _conn_I(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_L(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_S(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_s(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_Z(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_T(const ostream_sink&) noexcept -> std::ostream&;
    auto _conn_t(const ostream_sink&) noexcept -> std::ostream&;
    auto _get_stream_id() noexcept -> std::uintmax_t;

    std::ostream& _output;
    stream_id_t _id_seq{0};
    flat_map<stream_id_t, ostream_context> _streams;
    std::string _temp;
    bool _condensed{false};
};
//------------------------------------------------------------------------------
ostream_sink::ostream_sink(
  stream_id_t id,
  shared_holder<ostream_output> parent) noexcept
  : _id{id}
  , _parent{std::move(parent)} {}
//------------------------------------------------------------------------------
ostream_output::ostream_output(std::ostream& output) noexcept
  : _output{output} {
    _output << "╮\n";
}
//------------------------------------------------------------------------------
ostream_output::~ostream_output() noexcept {
    _output << "╯\n";
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_I(const ostream_sink&) noexcept -> std::ostream& {
    _output << "┊";
    for(auto& s : _streams) {
        (void)s;
        _output << " │";
    }
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_L(const ostream_sink& si) noexcept -> std::ostream& {
    _output << "┊";
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _output << " ┕";
            conn = false;
        } else if(conn) {
            _output << " │";
        } else {
            _output << "━━";
        }
    }
    _output << "━┥";
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_S(const ostream_sink& si) noexcept -> std::ostream& {
    _output << "┊";
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _output << "  ";
            conn = false;
        } else if(conn) {
            _output << " │";
        } else {
            _output << "╭╯";
        }
    }
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_s(const ostream_sink& si) noexcept -> std::ostream& {
    _output << "┊";
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _output << " ";
            conn = false;
        } else if(conn) {
            _output << " │";
        } else {
            _output << "╭╯";
        }
    }
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_Z(const ostream_sink& si) noexcept -> std::ostream& {
    _output << "┊";
    bool conn{true};
    for(auto& st : _streams) {
        if(std::get<0>(st) == si.id()) {
            _output << " ┝";
            conn = false;
        } else if(conn) {
            _output << " │";
        } else {
            _output << "━━";
        }
    }
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_T(const ostream_sink&) noexcept -> std::ostream& {
    _output << "┝";
    for(auto& s : _streams) {
        (void)s;
        _output << "━━";
    }
    _output << "━┯━┥";
    return _output;
}
//------------------------------------------------------------------------------
auto ostream_output::_conn_t(const ostream_sink&) noexcept -> std::ostream& {
    _output << "┝";
    for(auto& s : _streams) {
        (void)s;
        _output << "━━";
    }
    _output << "━┥";
    return _output;
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const begin_info& info) noexcept {
    _conn_I(s) << "   ╭────────────╮\n";
    _conn_T(s) << "starting log│\n";
    _conn_I(s) << " │ ╰────────────╯\n";
    _streams[s.id()].begin = info;
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const begin_info& info) noexcept {
    _begin = info;
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const description_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const description_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const declare_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const declare_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const active_state_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const active_state_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const message_info& info,
  message_formatter& formatter) noexcept {
    if(_condensed) {
        _conn_Z(s) << "━┑";
    } else {
        if(info.tag) {
            _conn_I(s) << " ╭──────────┬──────────┬─────────┬"
                          "──────────┬──────────┬──────────┬────────────╮\n";
        } else {
            _conn_I(s) << " ╭──────────┬──────────┬─────────┬"
                          "──────────┬──────────┬────────────╮\n";
        }
        _conn_Z(s) << "━┥";
    }
    _output << padded_to(10, format_reltime(s.time_since_start(info)));
    _output << "│";
    _output << padded_to(10, format_reltime(s.time_since_prev(info)));
    _output << "│";
    _output << padded_to(9, enumerator_name(info.severity));
    _output << "│";
    _output << padded_to(10, s.root());
    _output << "│";
    _output << padded_to(10, info.source);
    _output << "│";
    if(info.tag) {
        _output << padded_to(10, info.tag);
        _output << "│";
    }
    _output << padded_to(12, format_instance(info.instance, _temp));
    _output << "│";
    _output << '\n';
    if(info.tag) {
        _conn_I(s) << " ╰┬─────────┴──────────┴─────────┴"
                      "──────────┴──────────┴──────────┴────────────╯\n";
    } else {
        _conn_I(s) << " ╰┬─────────┴──────────┴─────────┴"
                      "──────────┴──────────┴────────────╯\n";
    }
    const auto arg_count{info.args.size()};

    _conn_I(s) << "  ╰";
    if(arg_count > 0) {
        _output << "─┐";
    } else {
        _output << "╼ ";
    }
    _output << formatter.format(info);
    _output << '\n';

    for(const int ai : integer_range(arg_count)) {
        _conn_I(s) << "    ";
        if(ai + 1 == arg_count) {
            _output << "╰";
        } else {
            _output << "├";
        }
        const auto& arg{info.args[ai]};
        _output << "─╼ " << arg.name.name() << ": ";
        _output << formatter.format(arg, false);
        _output << '\n';
    }
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const message_info& info) noexcept {
    if(not _root) {
        _root = info.source;
    }
    _parent->consume(*this, info, _formatter);
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const interval_info& info) noexcept {
    (void)s;
    (void)info;
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const interval_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const heartbeat_info& info) noexcept {
    _conn_I(s) << " ╭──────────┬──────────╮\n";
    _conn_t(s) << padded_to(10, format_reltime(s.time_since_start(info)));
    _output << "│heart-beat│\n";
    _conn_I(s) << " ╰──────────┴──────────╯\n";
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const heartbeat_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const finish_info& info) noexcept {
    _conn_I(s) << " ╭──────────┬──────────┬──────────┬───────────┬─────────╮\n";
    _conn_L(s) << padded_to(10, format_reltime(s.time_since_start(info)));
    _output << "│";
    _output << padded_to(10, format_reltime(s.time_since_prev(info)));
    _output << "│";
    _output << padded_to(10, s.root());
    _output << "│closing log│";
    if(info.clean) {
        _output << " success ";
    } else {
        _output << " failed  ";
    }
    _output << "│\n";
    _conn_S(s) << " ╰──────────┴──────────┴──────────┴───────────┴─────────╯\n";
    _conn_s(s) << '\n';
    _streams.erase(s.id());
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const finish_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
auto ostream_output::_get_stream_id() noexcept -> stream_id_t {
    return ++_id_seq;
}
//------------------------------------------------------------------------------
auto ostream_output::make_stream() noexcept -> unique_holder<stream_sink> {
    return {hold<ostream_sink>, _get_stream_id(), shared_from_this()};
}
//------------------------------------------------------------------------------
// make factory
//------------------------------------------------------------------------------
auto make_ostream_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory> {
    return {hold<ostream_output>, std::cout};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs

