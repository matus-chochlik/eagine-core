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
    auto root() const noexcept -> string_view;
    auto time_since_start(const auto&) const noexcept
      -> std::chrono::microseconds;
    auto time_since_prev(const auto&) const noexcept
      -> std::chrono::microseconds;

    void consume(const begin_info&) noexcept final;
    void consume(const message_info&) noexcept final;
    void consume(const heartbeat_info&) noexcept final;
    void consume(const finish_info&) noexcept final;

private:
    const stream_id_t _id;
    shared_holder<ostream_output> _parent;
    message_formatter _formatter;
    std::chrono::duration<float> _prev_offs{};
    std::string _root;
    begin_info _begin{};
};
//------------------------------------------------------------------------------
auto ostream_sink::id() const noexcept -> stream_id_t {
    return _id;
}
//------------------------------------------------------------------------------
auto ostream_sink::root() const noexcept -> string_view {
    return {_root};
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
    void consume(
      const ostream_sink&,
      const message_info&,
      const std::string&) noexcept;
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
    auto _get_stream_id() noexcept -> std::uintmax_t;

    std::ostream& _output;
    stream_id_t _id_seq{0};
    flat_map<stream_id_t, ostream_context> _streams;
    std::string _temp;
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
  const message_info& info,
  const std::string& message) noexcept {
    _conn_Z(s) << "━┑";
    _output << padded_to(10, format_reltime(s.time_since_start(info)));
    _output << "│";
    _output << padded_to(10, format_reltime(s.time_since_prev(info)));
    _output << "│";
    _output << padded_to(9, info.severity);
    _output << "│";
    _output << padded_to(10, s.root());
    _output << "│";
    _output << padded_to(10, info.source);
    _output << "│";
    if(not info.tag.empty()) {
        _output << padded_to(10, info.tag);
        _output << "│";
    }
    _output << padded_to(12, format_instance(info.instance, _temp));
    _output << "│";
    _output << '\n';
    if(info.tag.empty()) {
        _conn_I(s) << " ├──────────┴──────────┴─────────┴"
                      "──────────┴──────────┴────────────╯\n";
    } else {
        _conn_I(s) << " ├──────────┴──────────┴─────────┴"
                      "──────────┴──────────┴──────────┴────────────╯\n";
    }
    _conn_I(s) << " ╰─┤";
    _output << message;
    _output << '\n';
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const message_info& info) noexcept {
    if(_root.empty()) {
        _root = info.source;
    }
    _parent->consume(*this, info, _formatter.format(info));
    _prev_offs = info.offset;
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink&,
  const heartbeat_info&) noexcept {}
//------------------------------------------------------------------------------
void ostream_sink::consume(const heartbeat_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const finish_info& info) noexcept {
    _conn_I(s) << " ╭──────────┬──────────┬────────────┬─────────╮\n";
    _conn_L(s) << padded_to(10, format_reltime(s.time_since_start(info)));
    _output << "│";
    _output << padded_to(10, format_reltime(s.time_since_prev(info)));
    _output << "│ closing log│";
    if(info.clean) {
        _output << " success ";
    } else {
        _output << " failed  ";
    }
    _output << "│\n";
    _conn_S(s) << " ╰──────────┴──────────┴────────────┴─────────╯\n";
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

