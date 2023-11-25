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
// extractor interface
//------------------------------------------------------------------------------
template <typename T>
struct extractor_arg {
    const basic_string_path& path;
    T value;
};
//------------------------------------------------------------------------------
struct attrib_extractor : interface<attrib_extractor> {
    virtual void reset() noexcept {}
    virtual void add_arg() noexcept {}
    virtual auto add(const extractor_arg<bool>&) noexcept -> bool {
        return false;
    }
    virtual auto add(const extractor_arg<std::int64_t>&) noexcept -> bool {
        return false;
    }
    virtual auto add(const extractor_arg<std::uint64_t>&) noexcept -> bool {
        return false;
    }
    virtual auto add(const extractor_arg<float>&) noexcept -> bool {
        return false;
    }
    virtual auto add(const extractor_arg<string_view>&) noexcept -> bool {
        return false;
    }
    virtual void consume_by(stream_sink&) noexcept {}
};
//------------------------------------------------------------------------------
// extractor mixins
//------------------------------------------------------------------------------
template <typename Info>
struct info_extractor : attrib_extractor {
    void consume_by(stream_sink& sink) noexcept final {
        sink.consume(info);
    }

    Info info;
};
//------------------------------------------------------------------------------
// time offset
//------------------------------------------------------------------------------
template <typename Base>
struct offset_extractor : Base {
    template <typename T>
    auto _add_offset(const extractor_arg<T>&) noexcept -> bool;

    using Base::add;

    auto add(const extractor_arg<float>& a) noexcept -> bool override {
        return _add_offset(a);
    }

    auto add(const extractor_arg<std::int64_t>& a) noexcept -> bool override {
        return _add_offset(a);
    }

    auto add(const extractor_arg<std::uint64_t>& a) noexcept -> bool override {
        return _add_offset(a);
    }

private:
    const basic_string_path _ofs_pattern{"_/ts"};
};
//------------------------------------------------------------------------------
template <typename Base>
template <typename T>
auto offset_extractor<Base>::_add_offset(const extractor_arg<T>& a) noexcept
  -> bool {
    if(a.path.like(_ofs_pattern)) {
        this->info.offset = float_seconds(float(a.value));
        return true;
    }
    return Base::add(a);
}
//------------------------------------------------------------------------------
// source id
//------------------------------------------------------------------------------
template <typename Base>
struct source_extractor : Base {
    using Base::add;

    auto add(const extractor_arg<string_view>& a) noexcept -> bool override {
        if(a.path.like(_src_pattern)) {
            this->info.source = identifier{a.value};
            return true;
        }
        return Base::add(a);
    }

private:
    const basic_string_path _src_pattern{"_/src"};
};
//------------------------------------------------------------------------------
// instance id
//------------------------------------------------------------------------------
template <typename Base>
struct instance_extractor : Base {
    using Base::add;

    auto add(const extractor_arg<std::uint64_t>& a) noexcept -> bool override {
        if(a.path.like(_iid_pattern)) {
            this->info.instance = a.value;
            return true;
        }
        return Base::add(a);
    }

private:
    const basic_string_path _iid_pattern{"_/iid"};
};
//------------------------------------------------------------------------------
// entry tag
//------------------------------------------------------------------------------
template <typename Base>
struct tag_extractor : Base {

    using Base::add;

    auto add(const extractor_arg<string_view>& a) noexcept -> bool override {
        if(a.path.like(_tag_pattern)) {
            this->info.tag = identifier{a.value};
            return true;
        }
        return Base::add(a);
    }

private:
    const basic_string_path _tag_pattern{"_/tag"};
};
//------------------------------------------------------------------------------
// begin extractor
//------------------------------------------------------------------------------
using begin_extractor_base = info_extractor<begin_info>;
struct begin_extractor : begin_extractor_base {
    auto add(const extractor_arg<string_view>&) noexcept -> bool final;

private:
    const basic_string_path _ssn_pattern{"_/session"};
    const basic_string_path _idy_pattern{"_/identity"};
};
//------------------------------------------------------------------------------
auto begin_extractor::add(const extractor_arg<string_view>& a) noexcept
  -> bool {
    if(a.path.like(_ssn_pattern)) {
        assign_to(a.value, this->info.session);
        return true;
    } else if(a.path.like(_idy_pattern)) {
        assign_to(a.value, this->info.identity);
        return true;
    }
    return begin_extractor_base::add(a);
}
//------------------------------------------------------------------------------
// declare state extractor
//------------------------------------------------------------------------------
using declare_state_extractor_base = instance_extractor<
  source_extractor<offset_extractor<info_extractor<declare_state_info>>>>;
struct declare_state_extractor : declare_state_extractor_base {
    auto add(const extractor_arg<string_view>&) noexcept -> bool final;

private:
    const basic_string_path _bgn_pattern{"_/bgn"};
    const basic_string_path _end_pattern{"_/end"};
};
//------------------------------------------------------------------------------
auto declare_state_extractor::add(const extractor_arg<string_view>& a) noexcept
  -> bool {
    if(a.path.like(_bgn_pattern)) {
        this->info.begin_tag = identifier{a.value};
        return true;
    } else if(a.path.like(_end_pattern)) {
        this->info.end_tag = identifier{a.value};
        return true;
    }
    return declare_state_extractor_base::add(a);
}
//------------------------------------------------------------------------------
// active state extractor
//------------------------------------------------------------------------------
using active_state_extractor = tag_extractor<instance_extractor<
  source_extractor<offset_extractor<info_extractor<active_state_info>>>>>;
//------------------------------------------------------------------------------
// message extractor
//------------------------------------------------------------------------------
using message_extractor_base = tag_extractor<instance_extractor<
  source_extractor<offset_extractor<info_extractor<message_info>>>>>;
//------------------------------------------------------------------------------
struct message_extractor : message_extractor_base {

    void reset() noexcept final;

    auto add(const extractor_arg<bool>&) noexcept -> bool final;
    auto add(const extractor_arg<std::int64_t>&) noexcept -> bool final;
    auto add(const extractor_arg<std::uint64_t>&) noexcept -> bool final;
    auto add(const extractor_arg<float>&) noexcept -> bool final;
    auto add(const extractor_arg<string_view>&) noexcept -> bool final;

    void add_arg() noexcept final;

private:
    template <typename T>
    auto _add_value(const extractor_arg<T>&) noexcept -> bool;

    basic_string_path _fmt_pattern{"_/f"};
    basic_string_path _lvl_pattern{"_/lvl"};
    basic_string_path _atr_pattern{"_/a/_/*"};
};
//------------------------------------------------------------------------------
void message_extractor::reset() noexcept {
    this->info.args.clear();
}
//------------------------------------------------------------------------------
template <typename T>
auto message_extractor::_add_value(const extractor_arg<T>& a) noexcept -> bool {
    if(a.path.like(_atr_pattern)) {
        if(a.path.ends_with("v")) {
            this->info.args.back().value = a.value;
            return true;
        }
        if constexpr(std::is_arithmetic_v<T>) {
            if(a.path.ends_with("min")) {
                this->info.args.back().min = a.value;
                return true;
            }
            if(a.path.ends_with("max")) {
                this->info.args.back().max = a.value;
                return true;
            }
        }
    }
    return message_extractor_base::add(a);
}
//------------------------------------------------------------------------------
auto message_extractor::add(const extractor_arg<bool>& a) noexcept -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
auto message_extractor::add(const extractor_arg<std::int64_t>& a) noexcept
  -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
auto message_extractor::add(const extractor_arg<std::uint64_t>& a) noexcept
  -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
auto message_extractor::add(const extractor_arg<float>& a) noexcept -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
auto message_extractor::add(const extractor_arg<string_view>& a) noexcept
  -> bool {
    if(a.path.like(_fmt_pattern)) {
        assign_to(a.value, this->info.format);
        return true;
    } else if(a.path.like(_lvl_pattern)) {
        this->info.severity =
          from_string(
            a.value, std::type_identity<log_event_severity>{}, default_selector)
            .value_or(log_event_severity::info);
        return true;
    } else if(a.path.like(_atr_pattern)) {
        if(a.path.ends_with("n")) {
            this->info.args.back().name = identifier{a.value};
            return true;
        } else if(a.path.ends_with("t")) {
            this->info.args.back().tag = identifier{a.value};
            return true;
        } else if(a.path.ends_with("v")) {
            this->info.args.back().value = to_string(a.value);
            return true;
        }
    } else {
        return message_extractor_base::add(a);
    }
    return false;
}
//------------------------------------------------------------------------------
void message_extractor::add_arg() noexcept {
    this->info.args.emplace_back();
}
//------------------------------------------------------------------------------
// interval extractor
//------------------------------------------------------------------------------
using interval_extractor_base =
  instance_extractor<tag_extractor<info_extractor<interval_info>>>;
//------------------------------------------------------------------------------
struct interval_extractor : interval_extractor_base {
    auto add(const extractor_arg<std::int64_t>&) noexcept -> bool final;
    auto add(const extractor_arg<std::uint64_t>&) noexcept -> bool final;

private:
    template <typename T>
    auto _add_value(const extractor_arg<T>&) noexcept -> bool;

    basic_string_path _tns_pattern{"_/tns"};
};
//------------------------------------------------------------------------------
template <typename T>
auto interval_extractor::_add_value(const extractor_arg<T>& a) noexcept
  -> bool {
    if(a.path.like(_tns_pattern)) {
        this->info.duration = std::chrono::nanoseconds{a.value};
        return true;
    }
    return interval_extractor_base::add(a);
}
//------------------------------------------------------------------------------
auto interval_extractor::add(const extractor_arg<std::int64_t>& a) noexcept
  -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
auto interval_extractor::add(const extractor_arg<std::uint64_t>& a) noexcept
  -> bool {
    return _add_value(a);
}
//------------------------------------------------------------------------------
// heartbeat extractor
//------------------------------------------------------------------------------
using heartbeat_extractor = offset_extractor<info_extractor<heartbeat_info>>;
//------------------------------------------------------------------------------
// finish extractor
//------------------------------------------------------------------------------
using finish_extractor = offset_extractor<info_extractor<finish_info>>;
//------------------------------------------------------------------------------
// extractor
//------------------------------------------------------------------------------
class json_data_extractor final : public valtree::object_builder {
public:
    json_data_extractor(shared_holder<stream_sink> stream) noexcept;

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

    flat_map<string_view, unique_holder<attrib_extractor>, str_view_less>
      _extractors;
    optional_reference<attrib_extractor> _current_extractor{};

    shared_holder<stream_sink> _stream;
    const basic_string_path _ent_pattern{"_"};
    const basic_string_path _typ_pattern{"_/t"};
    const basic_string_path _arg_pattern{"_/a/_"};
};
//------------------------------------------------------------------------------
json_data_extractor::json_data_extractor(
  shared_holder<stream_sink> stream) noexcept
  : _stream{std::move(stream)} {
    _extractors["begin"].emplace_derived(hold<begin_extractor>);
    _extractors["m"].emplace_derived(hold<message_extractor>);
    _extractors["i"].emplace_derived(hold<interval_extractor>);
    _extractors["ds"].emplace_derived(hold<declare_state_extractor>);
    _extractors["as"].emplace_derived(hold<active_state_extractor>);
    _extractors["hb"].emplace_derived(hold<heartbeat_extractor>);
    _extractors["end"].emplace_derived(hold<finish_extractor>);
}
//------------------------------------------------------------------------------
auto json_data_extractor::should_continue() noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
auto json_data_extractor::max_token_size() noexcept -> span_size_t {
    return 256;
}
//------------------------------------------------------------------------------
void json_data_extractor::begin() noexcept {}
//------------------------------------------------------------------------------
template <typename T>
void json_data_extractor::_add(
  const basic_string_path& path,
  span<const T> data) noexcept {
    if(data and _current_extractor) {
        _current_extractor->add(extractor_arg<T>{.path = path, .value = *data});
    }
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  [[maybe_unused]] const basic_string_path& path,
  [[maybe_unused]] const span<const nothing_t> data) noexcept {}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const bool> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const std::int64_t> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const std::uint64_t> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const float> data) noexcept {
    _add(path, data);
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const double> data) noexcept {
    if(data and _current_extractor) {
        _current_extractor->add(
          extractor_arg<float>{.path = path, .value = float(*data)});
    }
}
//------------------------------------------------------------------------------
void json_data_extractor::add(
  const basic_string_path& path,
  span<const string_view> data) noexcept {
    if(data) {
        const string_view value{*data};
        if(path.like(_typ_pattern)) {
            _current_extractor = _extractors[value];
            if(_current_extractor) {
                _current_extractor->reset();
            }
        } else if(_current_extractor) {
            _current_extractor->add(
              extractor_arg<string_view>{.path = path, .value = value});
        }
    }
}
//------------------------------------------------------------------------------
void json_data_extractor::add_object(const basic_string_path& path) noexcept {
    if(_current_extractor) {
        if(path.like(_arg_pattern)) {
            _current_extractor->add_arg();
        }
    }
}
//------------------------------------------------------------------------------
void json_data_extractor::finish_object(const basic_string_path& path) noexcept {
    if(_current_extractor) {
        if(path.like(_ent_pattern)) {
            _current_extractor->consume_by(*_stream);
        }
    }
}
//------------------------------------------------------------------------------
auto json_data_extractor::finish() noexcept -> bool {
    return true;
}
//------------------------------------------------------------------------------
void json_data_extractor::failed() noexcept {}
//------------------------------------------------------------------------------
// make extractor
//------------------------------------------------------------------------------
auto make_json_parser(main_ctx& ctx, shared_holder<stream_sink> stream) noexcept
  -> parser_input {
    return valtree::traverse_json_stream(
      {hold<json_data_extractor>, std::move(stream)}, ctx.buffers(), ctx.log());
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
