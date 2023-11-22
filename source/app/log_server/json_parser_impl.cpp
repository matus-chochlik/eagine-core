/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.log_server;

import std;
import eagine.core;

import :interfaces;

namespace eagine::logs {
//------------------------------------------------------------------------------
enum class json_data_kind {
    none,
    begin,
    declare_state,
    active_state,
    message,
    interval,
    finish
};
//------------------------------------------------------------------------------
// parser
//------------------------------------------------------------------------------
class json_data_parser final : public valtree::object_builder {
public:
    json_data_parser(shared_holder<stream_sink> stream) noexcept
      : _stream{std::move(stream)} {}

    auto should_continue() noexcept -> bool final;

    auto max_token_size() noexcept -> span_size_t final;

    void begin() noexcept final;

    void add(const basic_string_path&, const span<const nothing_t>) noexcept
      final;

    void add(const basic_string_path&, span<const bool>) noexcept final;

    void add(const basic_string_path&, span<const std::int64_t>) noexcept final;

    void add(const basic_string_path&, span<const std::uint64_t>) noexcept final;

    void add(const basic_string_path&, span<const float>) noexcept final;

    void add(const basic_string_path&, span<const double>) noexcept final;

    void add(const basic_string_path&, span<const string_view>) noexcept final;

    void add_object(const basic_string_path&) noexcept final;

    void finish_object(const basic_string_path&) noexcept final;

    virtual auto finish() noexcept -> bool final;

    void failed() noexcept final;

private:
    void _handle_begin_attr(const string_view, const string_view) noexcept;
    void _handle_message_attr(const string_view, const string_view) noexcept;
    void _handle_finish_attr(const string_view, const string_view) noexcept;
    void _handle_entry_attr_s(const string_view, const string_view) noexcept;
    void _handle_entry_attr_f(const string_view, const float) noexcept;
    void _handle_entry_attr_i(const string_view, const std::int64_t) noexcept;
    void _handle_entry_attr_u(const string_view, const std::uint64_t) noexcept;

    begin_info _begin{};
    message_info _message{};
    finish_info _finish{};

    shared_holder<stream_sink> _stream;
    json_data_kind _current{json_data_kind::none};
};
//------------------------------------------------------------------------------
auto json_data_parser::should_continue() noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto json_data_parser::max_token_size() noexcept -> span_size_t {
    return 256;
}
//------------------------------------------------------------------------------
void json_data_parser::begin() noexcept {}
//------------------------------------------------------------------------------
void json_data_parser::_handle_begin_attr(
  const string_view attr,
  const string_view value) noexcept {
    if(attr == "session") {
        assign_to(value, _begin.session);
    } else if(attr == "identity") {
        assign_to(value, _begin.identity);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_message_attr(
  const string_view attr,
  const string_view value) noexcept {
    if(attr == "f") {
        assign_to(value, _message.format);
    } else if(attr == "src") {
        assign_to(value, _message.source);
    } else if(attr == "tag") {
        assign_to(value, _message.tag);
    } else if(attr == "lvl") {
        assign_to(value, _message.severity);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_finish_attr(
  const string_view attr,
  const string_view value) noexcept {
    (void)attr;
    (void)value;
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_entry_attr_s(
  const string_view attr,
  const string_view value) noexcept {
    switch(_current) {
        case json_data_kind::begin:
            _handle_begin_attr(attr, value);
            break;
        case json_data_kind::message:
            _handle_message_attr(attr, value);
            break;
        case json_data_kind::finish:
            _handle_finish_attr(attr, value);
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_entry_attr_f(
  const string_view attr,
  const float value) noexcept {
    if(attr == "ts") {
        switch(_current) {
            case json_data_kind::message:
                _message.offset = std::chrono::duration<float>(value);
                break;
            case json_data_kind::finish:
                _finish.offset = std::chrono::duration<float>(value);
                break;
            default:
                break;
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_entry_attr_i(
  const string_view attr,
  const std::int64_t value) noexcept {
    if(attr == "iid") {
        switch(_current) {
            case json_data_kind::message:
                _message.instance = limit_cast<std::int64_t>(value);
                break;
            default:
                break;
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::_handle_entry_attr_u(
  const string_view attr,
  const std::uint64_t value) noexcept {
    if(attr == "iid") {
        switch(_current) {
            case json_data_kind::message:
                _message.instance = value;
                break;
            default:
                break;
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  const span<const nothing_t> data) noexcept {
    (void)path;
    (void)data;
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const bool> data) noexcept {
    (void)path;
    (void)data;
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const std::int64_t> data) noexcept {
    if(data and path.size() == 2) {
        _handle_entry_attr_i(path.back(), *data);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const std::uint64_t> data) noexcept {
    if(data and path.size() == 2) {
        _handle_entry_attr_u(path.back(), *data);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const float> data) noexcept {
    if(data and path.size() == 2) {
        _handle_entry_attr_f(path.back(), *data);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const double> data) noexcept {
    if(data and path.size() == 2) {
        _handle_entry_attr_f(path.back(), float(*data));
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const string_view> data) noexcept {
    if(data and path.size() == 2) {
        if(path.ends_with("t")) {
            const auto kind{*data};
            if(kind == "m") {
                _current = json_data_kind::message;
            } else if(kind == "begin") {
                _current = json_data_kind::begin;
            } else if(kind == "end") {
                _current = json_data_kind::finish;
            }
        } else {
            _handle_entry_attr_s(path.back(), *data);
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add_object(const basic_string_path& path) noexcept {
    if(path.size() == 1) {
        assert(_current == json_data_kind::none);
    }
}
//------------------------------------------------------------------------------
void json_data_parser::finish_object(const basic_string_path& path) noexcept {
    if(path.size() == 1) {
        switch(_current) {
            case json_data_kind::begin:
                _stream->consume(_begin);
                break;
            case json_data_kind::message:
                _stream->consume(_message);
                break;
            case json_data_kind::finish:
                _finish.clean = true;
                _stream->consume(_finish);
                break;
            default:
                break;
        }
        _current = json_data_kind::none;
    }
}
//------------------------------------------------------------------------------
auto json_data_parser::finish() noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
void json_data_parser::failed() noexcept {}
//------------------------------------------------------------------------------
// make parser
//------------------------------------------------------------------------------
auto make_json_parser(main_ctx& ctx, shared_holder<stream_sink> stream) noexcept
  -> parser_input {
    return valtree::traverse_json_stream(
      {hold<json_data_parser>, std::move(stream)}, ctx.buffers(), ctx.log());
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
