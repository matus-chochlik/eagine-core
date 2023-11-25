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
template <typename T>
using id_t = std::type_identity<T>;
//------------------------------------------------------------------------------
// parser interface
//------------------------------------------------------------------------------
struct attrib_parser : interface<attrib_parser> {
    virtual void reset() noexcept {}
    virtual void add_arg() noexcept {}
    virtual auto add(const basic_string_path&, bool, id_t<bool>) noexcept
      -> bool {
        return false;
    }
    virtual auto add(
      const basic_string_path&,
      std::int64_t,
      id_t<std::int64_t>) noexcept -> bool {
        return false;
    }
    virtual auto add(
      const basic_string_path&,
      std::uint64_t,
      id_t<std::uint64_t>) noexcept -> bool {
        return false;
    }
    virtual auto add(const basic_string_path&, float, id_t<float>) noexcept
      -> bool {
        return false;
    }
    virtual auto add(
      const basic_string_path&,
      string_view,
      id_t<string_view>) noexcept -> bool {
        return false;
    }
    virtual void consume_by(stream_sink&) noexcept {}
};
//------------------------------------------------------------------------------
// parser mixins
//------------------------------------------------------------------------------
template <typename Info>
struct info_parser : attrib_parser {
    void consume_by(stream_sink& sink) noexcept final {
        sink.consume(info);
    }

    Info info;
};
//------------------------------------------------------------------------------
// time offset
//------------------------------------------------------------------------------
template <typename Base>
struct offset_parser : Base {
    template <typename T>
    auto _add_offset(const basic_string_path& path, T value, id_t<T> tid) noexcept
      -> bool;

    using Base::add;

    auto add(
      const basic_string_path& path,
      float value,
      id_t<float> tid) noexcept -> bool override {
        return _add_offset(path, value, tid);
    }

    auto add(
      const basic_string_path& path,
      std::int64_t value,
      id_t<std::int64_t> tid) noexcept -> bool override {
        return _add_offset(path, value, tid);
    }

    auto add(
      const basic_string_path& path,
      std::uint64_t value,
      id_t<std::uint64_t> tid) noexcept -> bool override {
        return _add_offset(path, value, tid);
    }

private:
    const basic_string_path _ofs_pattern{"_/ts"};
};
//------------------------------------------------------------------------------
template <typename Base>
template <typename T>
auto offset_parser<Base>::_add_offset(
  const basic_string_path& path,
  T value,
  id_t<T> tid) noexcept -> bool {
    if(path.like(_ofs_pattern)) {
        this->info.offset = float_seconds(float(value));
        return true;
    }
    return Base::add(path, value, tid);
}
//------------------------------------------------------------------------------
// source id
//------------------------------------------------------------------------------
template <typename Base>
struct source_parser : Base {
    using Base::add;

    auto add(
      const basic_string_path& path,
      string_view value,
      id_t<string_view> tid) noexcept -> bool override {
        if(path.like(_src_pattern)) {
            this->info.source = identifier{value};
            return true;
        }
        return Base::add(path, value, tid);
    }

private:
    const basic_string_path _src_pattern{"_/src"};
};
//------------------------------------------------------------------------------
// source id
//------------------------------------------------------------------------------
template <typename Base>
struct instance_parser : Base {
    using Base::add;

    auto add(
      const basic_string_path& path,
      std::uint64_t value,
      id_t<std::uint64_t> tid) noexcept -> bool override {
        if(path.like(_iid_pattern)) {
            this->info.instance = value;
            return true;
        }
        return Base::add(path, value, tid);
    }

private:
    const basic_string_path _iid_pattern{"_/iid"};
};
//------------------------------------------------------------------------------
// entry tag
//------------------------------------------------------------------------------
template <typename Base>
struct tag_parser : Base {

    using Base::add;

    auto add(
      const basic_string_path& path,
      string_view value,
      id_t<string_view> tid) noexcept -> bool override {
        if(path.like(_tag_pattern)) {
            this->info.tag = identifier{value};
            return true;
        }
        return Base::add(path, value, tid);
    }

private:
    const basic_string_path _tag_pattern{"_/tag"};
};
//------------------------------------------------------------------------------
// begin parser
//------------------------------------------------------------------------------
using begin_parser_base = info_parser<begin_info>;
struct begin_parser : begin_parser_base {
    auto add(const basic_string_path&, string_view, id_t<string_view>) noexcept
      -> bool final;

private:
    const basic_string_path _ssn_pattern{"_/session"};
    const basic_string_path _idy_pattern{"_/identity"};
};
//------------------------------------------------------------------------------
auto begin_parser::add(
  const basic_string_path& path,
  string_view value,
  id_t<string_view> tid) noexcept -> bool {
    if(path.like(_ssn_pattern)) {
        assign_to(value, this->info.session);
        return true;
    } else if(path.like(_idy_pattern)) {
        assign_to(value, this->info.identity);
        return true;
    }
    return begin_parser_base::add(path, value, tid);
}
//------------------------------------------------------------------------------
// declare state parser
//------------------------------------------------------------------------------
using declare_state_parser_base =
  instance_parser<source_parser<offset_parser<info_parser<declare_state_info>>>>;
struct declare_state_parser : declare_state_parser_base {
    auto add(const basic_string_path&, string_view, id_t<string_view>) noexcept
      -> bool final;

private:
    const basic_string_path _bgn_pattern{"_/bgn"};
    const basic_string_path _end_pattern{"_/end"};
};
//------------------------------------------------------------------------------
auto declare_state_parser::add(
  const basic_string_path& path,
  string_view value,
  id_t<string_view> tid) noexcept -> bool {
    if(path.like(_bgn_pattern)) {
        this->info.begin_tag = identifier{value};
        return true;
    } else if(path.like(_end_pattern)) {
        this->info.end_tag = identifier{value};
        return true;
    }
    return declare_state_parser_base::add(path, value, tid);
}
//------------------------------------------------------------------------------
// message parser
//------------------------------------------------------------------------------
using message_parser_base = tag_parser<
  instance_parser<source_parser<offset_parser<info_parser<message_info>>>>>;
//------------------------------------------------------------------------------
struct message_parser : message_parser_base {

    void reset() noexcept final;

    template <typename T>
    auto _add_value(const basic_string_path&, T, id_t<T>) noexcept -> bool;

    auto add(const basic_string_path&, bool, id_t<bool>) noexcept -> bool final;

    auto add(const basic_string_path&, std::int64_t, id_t<std::int64_t>) noexcept
      -> bool final;

    auto add(
      const basic_string_path&,
      std::uint64_t,
      id_t<std::uint64_t>) noexcept -> bool final;

    auto add(const basic_string_path&, float, id_t<float>) noexcept
      -> bool final;

    auto add(const basic_string_path&, string_view, id_t<string_view>) noexcept
      -> bool final;

    void add_arg() noexcept final;

private:
    basic_string_path _fmt_pattern{"_/f"};
    basic_string_path _lvl_pattern{"_/lvl"};
    basic_string_path _atr_pattern{"_/a/_/*"};
};
//------------------------------------------------------------------------------
void message_parser::reset() noexcept {
    this->info.args.clear();
}
//------------------------------------------------------------------------------
template <typename T>
auto message_parser::_add_value(
  const basic_string_path& path,
  T value,
  id_t<T> tid) noexcept -> bool {
    if(path.like(_atr_pattern)) {
        if(path.ends_with("v")) {
            this->info.args.back().value = value;
            return true;
        }
        if constexpr(std::is_arithmetic_v<T>) {
            if(path.ends_with("min")) {
                this->info.args.back().min = value;
                return true;
            }
            if(path.ends_with("max")) {
                this->info.args.back().max = value;
                return true;
            }
        }
    }
    return message_parser_base::add(path, value, tid);
}
//------------------------------------------------------------------------------
auto message_parser::add(
  const basic_string_path& path,
  bool value,
  id_t<bool> tid) noexcept -> bool {
    return _add_value(path, value, tid);
}
//------------------------------------------------------------------------------
auto message_parser::add(
  const basic_string_path& path,
  std::int64_t value,
  id_t<std::int64_t> tid) noexcept -> bool {
    return _add_value(path, value, tid);
}
//------------------------------------------------------------------------------
auto message_parser::add(
  const basic_string_path& path,
  std::uint64_t value,
  id_t<std::uint64_t> tid) noexcept -> bool {
    return _add_value(path, value, tid);
}
//------------------------------------------------------------------------------
auto message_parser::add(
  const basic_string_path& path,
  float value,
  id_t<float> tid) noexcept -> bool {
    return _add_value(path, value, tid);
}
//------------------------------------------------------------------------------
auto message_parser::add(
  const basic_string_path& path,
  string_view value,
  id_t<string_view> tid) noexcept -> bool {
    if(path.like(_fmt_pattern)) {
        assign_to(value, this->info.format);
        return true;
    } else if(path.like(_lvl_pattern)) {
        this->info.severity =
          from_string(value, id_t<log_event_severity>{}, default_selector)
            .value_or(log_event_severity::info);
        return true;
    } else if(path.like(_atr_pattern)) {
        if(path.ends_with("n")) {
            this->info.args.back().name = identifier{value};
            return true;
        } else if(path.ends_with("t")) {
            this->info.args.back().tag = identifier{value};
            return true;
        } else if(path.ends_with("v")) {
            this->info.args.back().value = to_string(value);
            return true;
        }
    } else {
        return message_parser_base::add(path, value, tid);
    }
    return false;
}
//------------------------------------------------------------------------------
void message_parser::add_arg() noexcept {
    this->info.args.emplace_back();
}
//------------------------------------------------------------------------------
// finish parser
//------------------------------------------------------------------------------
using finish_parser = offset_parser<info_parser<finish_info>>;
//------------------------------------------------------------------------------
// parser
//------------------------------------------------------------------------------
class json_data_parser final : public valtree::object_builder {
public:
    json_data_parser(shared_holder<stream_sink> stream) noexcept;

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
    template <typename T>
    void _add(const basic_string_path&, span<const T>) noexcept;

    flat_map<string_view, unique_holder<attrib_parser>, str_view_less> _parsers;
    optional_reference<attrib_parser> _current_parser{};

    shared_holder<stream_sink> _stream;
    const basic_string_path _ent_pattern{"_"};
    const basic_string_path _typ_pattern{"_/t"};
    const basic_string_path _arg_pattern{"_/a/_"};
};
//------------------------------------------------------------------------------
json_data_parser::json_data_parser(shared_holder<stream_sink> stream) noexcept
  : _stream{std::move(stream)} {
    _parsers["begin"].emplace_derived(hold<begin_parser>);
    _parsers["m"].emplace_derived(hold<message_parser>);
    _parsers["ds"].emplace_derived(hold<declare_state_parser>);
    _parsers["end"].emplace_derived(hold<finish_parser>);
}
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
template <typename T>
void json_data_parser::_add(
  const basic_string_path& path,
  span<const T> data) noexcept {
    if(data and _current_parser) {
        _current_parser->add(path, *data, id_t<T>{});
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  [[maybe_unused]] const basic_string_path& path,
  [[maybe_unused]] const span<const nothing_t> data) noexcept {}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const bool> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const std::int64_t> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const std::uint64_t> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const float> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const double> data) noexcept {
    if(data and _current_parser) {
        _current_parser->add(path, float(*data), id_t<float>{});
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add(
  const basic_string_path& path,
  span<const string_view> data) noexcept {
    if(data) {
        const string_view value{*data};
        if(path.like(_typ_pattern)) {
            _current_parser = _parsers[value];
            if(_current_parser) {
                _current_parser->reset();
            }
        } else if(_current_parser) {
            _current_parser->add(path, value, id_t<string_view>{});
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::add_object(const basic_string_path& path) noexcept {
    if(_current_parser) {
        if(path.like(_arg_pattern)) {
            _current_parser->add_arg();
        }
    }
}
//------------------------------------------------------------------------------
void json_data_parser::finish_object(const basic_string_path& path) noexcept {
    if(_current_parser) {
        if(path.like(_ent_pattern)) {
            _current_parser->consume_by(*_stream);
        }
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
