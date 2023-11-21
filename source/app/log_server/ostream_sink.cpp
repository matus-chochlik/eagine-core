/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "interfaces.hpp"
#include "utilities.hpp"

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
    void begin(const begin_info&) noexcept final;
    void consume(const message_info&) noexcept final;
    void finish(const finish_info&) noexcept final;

private:
    const stream_id_t _id;
    shared_holder<ostream_output> _parent;
    begin_info _begin{};
};
//------------------------------------------------------------------------------
auto ostream_sink::id() const noexcept -> stream_id_t {
    return _id;
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

    void begin(const ostream_sink&, const begin_info&) noexcept;
    void consume(const ostream_sink&, const message_info&) noexcept;
    void finish(const ostream_sink&, const finish_info&) noexcept;

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
    flat_map<stream_id_t, ostream_context> _streams;
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
            _output << "  ";
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
            _output << "╭╯";
        } else {
            _output << " │";
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
            _output << "╭╯";
        } else {
            _output << " │";
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
            _output << "  ";
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
void ostream_output::begin(
  const ostream_sink& s,
  const begin_info& info) noexcept {
    _conn_I(s) << "   ╭────────────╮\n";
    _conn_T(s) << "starting log│\n";
    _conn_I(s) << " │ ╰────────────╯\n";
    _streams[s.id()].begin = info;
}
//------------------------------------------------------------------------------
void ostream_sink::begin(const begin_info& info) noexcept {
    _begin = info;
    _parent->begin(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::consume(
  const ostream_sink& s,
  const message_info& info) noexcept {
    _conn_I(s) << '\n';
    _conn_Z(s) << "━┑";
    _output << padded_to(9, {});
    _output << "│";
    _output << padded_to(9, {});
    _output << "│";
    _output << padded_to(9, {});
    _output << "│";
    _output << padded_to(10, {});
    _output << "│";
    _output << padded_to(10, {info.source});
    _output << "│";
    if(not info.tag.empty()) {
        _output << padded_to(10, info.tag);
        _output << "│";
    }
    _output << padded_to(12, {});
    _output << "│";
    _output << '\n';
    if(info.tag.empty()) {
        _conn_I(s) << " ├─────────┴─────────┴─────────┴"
                      "──────────┴──────────┴────────────╯\n";
    } else {
        _conn_I(s) << " ├─────────┴─────────┴─────────┴"
                      "──────────┴──────────┴──────────┴────────────╯\n";
    }
}
//------------------------------------------------------------------------------
void ostream_sink::consume(const message_info& info) noexcept {
    _parent->consume(*this, info);
}
//------------------------------------------------------------------------------
void ostream_output::finish(
  const ostream_sink& s,
  const finish_info& info) noexcept {
    _conn_I(s) << " ╭─────────┬──────────┬────────────┬─────────╮\n";
    _conn_L(s) << "         │          │closing log │";
    if(info.clean) {
        _output << " success ";
    } else {
        _output << " failed  ";
    }
    _output << "│\n";
    _conn_S(s) << " ╰─────────┴──────────┴────────────┴─────────╯\n";
    _conn_s(s) << '\n';
    _streams.erase(s.id());
}
//------------------------------------------------------------------------------
void ostream_sink::finish(const finish_info& info) noexcept {
    _parent->finish(*this, info);
}
//------------------------------------------------------------------------------
auto ostream_output::_get_stream_id() noexcept -> stream_id_t {
    if(not _streams.empty()) {
        return std::get<0>(_streams.back()) + 1U;
    }
    return 1U;
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

