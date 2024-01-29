/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.utility;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable>
class json_log_backend : public logger_backend {
    template <std::size_t N>
    void _add(const char (&str)[N]) {
        _buffer.append(static_cast<const char*>(str), N - 1);
    }

    void _add(const bool data);

    void _add(const std::string& data) {
        _buffer.append(data);
    }

    void _add(const std::string_view data) {
        _buffer.append(data);
    }

    void _add(const memory::basic_string_span<const char> data) {
        _buffer.append(data.std_view());
    }

    void _add(const decl_name& data) {
        _buffer.append(data.std_view());
    }

    template <auto M>
    void _add(const identifier_name<M>& data) {
        _add(data.view());
    }

    void _add(const identifier data) {
        _add(data.name());
    }

    void _add(const message_id data) {
        _add(data.class_().name());
        _add(".");
        _add(data.method().name());
    }

    void _add(const memory::const_block data);

    template <typename T>
    void _add(const T& value);

    void _flush(bool force = false) noexcept;

public:
    json_log_backend(
      const log_stream_info&,
      shared_holder<log_output_stream>) noexcept;

    json_log_backend(json_log_backend&&) = delete;
    json_log_backend(const json_log_backend&) = delete;
    auto operator=(json_log_backend&&) = delete;
    auto operator=(const json_log_backend&) = delete;

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return _alloc;
    }

    auto type_id() noexcept -> identifier override {
        return "FastJSON";
    }

    auto entry_backend(const log_event_severity) noexcept
      -> logger_backend* final;

    void begin_log() noexcept final;

    auto register_time_interval(
      const identifier tag,
      const logger_instance_id log_id) noexcept -> time_interval_id final;

    auto _get_interval(const time_interval_id int_id) noexcept -> auto*;

    void time_interval_begin(const time_interval_id int_id) noexcept final;

    void time_interval_end(const time_interval_id int_id) noexcept final;

    void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view display_name,
      const string_view description) noexcept final;

    void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) noexcept final;

    void active_state(
      const identifier source,
      const identifier state_tag) noexcept final;

    auto begin_message(
      const identifier source,
      const identifier tag,
      const logger_instance_id instance,
      const log_event_severity severity,
      const string_view format) noexcept -> bool final;

    void add_nothing(const identifier arg, const identifier tag) noexcept final;

    void do_add_arg_u(
      const identifier arg,
      const identifier tag,
      const auto value) noexcept;

    void do_add_arg_q(
      const identifier arg,
      const identifier tag,
      const auto value) noexcept;

    void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept final {
        do_add_arg_q(arg, tag, value);
    }

    void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id value) noexcept final {
        do_add_arg_q(arg, tag, value);
    }

    void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept final;

    void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept final {
        do_add_arg_u(arg, tag, value);
    }

    void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept final {
        do_add_arg_u(arg, tag, value);
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept final {
        do_add_arg_u(arg, tag, value);
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept final;

    void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept final;

    void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept final {
        do_add_arg_q(arg, tag, value);
    }

    void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept final {
        do_add_arg_q(arg, tag, value);
    }

    void finish_message() noexcept final;

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final;

    void heartbeat() noexcept final;
    void finish_log() noexcept final;

private:
    Lockable _lockable{};
    shared_holder<log_output_stream> _output;
    const std::string _session_identity;
    const std::string _log_identity;
    const log_event_severity _min_severity;
    const std::chrono::steady_clock::time_point _start;
    std::string _buffer;
    std::string _b64lob;
    memory::shared_byte_allocator _alloc{memory::default_byte_allocator()};

    struct _interval_info {
        identifier tag;
        logger_instance_id log_id;
        std::chrono::steady_clock::time_point start{};
        std::chrono::steady_clock::duration duration{};
        std::size_t count{0};
    };

    std::map<std::tuple<identifier_t, logger_instance_id>, _interval_info>
      _intervals;
};
//------------------------------------------------------------------------------
// implementation
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::_add(const bool data) {
    if(data) {
        _buffer.append("true");
    } else {
        _buffer.append("false");
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::_add(const memory::const_block data) {
    _output->write(_buffer);
    _buffer.clear();
    base64dump(data).apply([this](const char c) {
        _b64lob.push_back(c);
        if(_b64lob.size() >= 1024) {
            _output->write(_b64lob);
            _b64lob.clear();
        }
    });
    _output->write(_b64lob);
    _b64lob.clear();
}
//------------------------------------------------------------------------------
template <typename Lockable>
template <typename T>
void json_log_backend<Lockable>::_add(const T& value) {
    std::array<char, 48> temp{};
    const auto conv{
      std::to_chars(temp.data(), temp.data() + temp.size(), value)};
    if(conv.ec == std::error_code{}) {
        _buffer.append(temp.data(), std::distance(temp.data(), conv.ptr));
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::_flush(bool force) noexcept {
    _add("\n");
    _output->write(_buffer);
    _output->flush(force);
    _buffer.clear();
}
//------------------------------------------------------------------------------
template <typename Lockable>
json_log_backend<Lockable>::json_log_backend(
  const log_stream_info& info,
  shared_holder<log_output_stream> output) noexcept
  : _output{std::move(output)}
  , _session_identity{info.session_identity}
  , _log_identity{info.log_identity}
  , _min_severity{info.min_severity}
  , _start{std::chrono::steady_clock::now()} {
    _buffer.reserve(1024);
}
//------------------------------------------------------------------------------
template <typename Lockable>
auto json_log_backend<Lockable>::entry_backend(
  const log_event_severity severity) noexcept -> logger_backend* {
    if(severity >= _min_severity) {
        return this;
    }
    return nullptr;
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::begin_log() noexcept {
    try {
        const auto now_sys{std::chrono::system_clock::now()};
        const auto now_sdy{std::chrono::steady_clock::now()};
        const auto now{now_sys - (now_sdy - _start)};
        const std::string start_tse{std::format("{:%Y-%m-%d %T}", now)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add("[");
        _add(R"({"t":"begin","time":")");
        _add(start_tse);
        if(not _session_identity.empty()) {
            _add(R"(","session":")");
            _add(_session_identity);
        }
        if(not _log_identity.empty()) {
            _add(R"(","identity":")");
            _add(_log_identity);
        }
        _add(R"("})");
        _flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
auto json_log_backend<Lockable>::register_time_interval(
  const identifier tag,
  const logger_instance_id log_id) noexcept -> time_interval_id {
    try {
        const auto key{std::make_tuple(tag.value(), log_id)};
        const std::lock_guard<Lockable> lock{_lockable};
        auto pos{_intervals.find(key)};
        if(pos == _intervals.end()) {
            pos = _intervals
                    .emplace(key, _interval_info{.tag = tag, .log_id = log_id})
                    .first;
        }
        return reinterpret_cast<time_interval_id>(&(pos->second));
    } catch(...) {
        return 0U;
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
auto json_log_backend<Lockable>::_get_interval(
  const time_interval_id int_id) noexcept -> auto* {
    return static_cast<_interval_info*>(reinterpret_cast<void*>(int_id));
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::time_interval_begin(
  const time_interval_id int_id) noexcept {
    try {
        if(const auto info{_get_interval(int_id)}) {
            info->start = std::chrono::steady_clock::now();
        }
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::time_interval_end(
  const time_interval_id int_id) noexcept {
    try {
        if(const auto info{_get_interval(int_id)}) {
            const auto now{std::chrono::steady_clock::now()};
            info->count++;
            info->duration += (now - info->start);

            const auto batch{1'000U};
            if(info->count >= batch) {
                std::chrono::nanoseconds avg{(info->duration / batch)};
                const std::lock_guard<Lockable> lock{_lockable};
                _add(R"(,{"t":"i","iid":)");
                _add(info->log_id);
                _add(R"(,"tag":")");
                _add(info->tag);
                _add(R"(","tns":)");
                _add(avg.count());
                _add(R"(})");
                _flush();
                info->count = 0U;
                info->duration = {};
            }
        }
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::set_description(
  const identifier source,
  const logger_instance_id instance,
  const string_view display_name,
  const string_view description) noexcept {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add(R"(,{"t":"d","src":")");
        _add(source.name());
        _add(R"(","iid":)");
        _add(instance);
        _add(R"(,"ts":)");
        _add(sec.count());
        _add(R"(,"dn":")");
        _add(display_name);
        _add(R"(","desc":")");
        _add(description);
        _add(R"("})");
        _flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::declare_state(
  const identifier source,
  const identifier state_tag,
  const identifier begin_tag,
  const identifier end_tag) noexcept {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add(R"(,{"t":"ds","src":")");
        _add(source.name());
        _add(R"(","tag":")");
        _add(state_tag.name());
        _add(R"(","ts":)");
        _add(sec.count());
        _add(R"(,"bgn":")");
        _add(begin_tag.name());
        _add(R"(","end":")");
        _add(end_tag.name());
        _add(R"("})");
        _flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::active_state(
  const identifier source,
  const identifier state_tag) noexcept {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add(R"(,{"t":"as","src":")");
        _add(source.name());
        _add(R"(","tag":")");
        _add(state_tag.name());
        _add(R"(","ts":)");
        _add(sec.count());
        _add(R"(})");
        _flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
auto json_log_backend<Lockable>::begin_message(
  const identifier source,
  const identifier tag,
  const logger_instance_id instance,
  const log_event_severity severity,
  const string_view format) noexcept -> bool {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const auto severity_name{enumerator_name(severity)};
        const auto source_name{source.name()};
        const auto tag_name{tag.name()};
        _lockable.lock();
        _add(R"(,{"t":"m","lvl":")");
        _add(severity_name);
        _add(R"(","src":")");
        _add(source_name);
        if(tag) {
            _add(R"(","tag":")");
            _add(tag_name);
        }
        _add(R"(","iid":)");
        _add(instance);
        _add(R"(,"ts":)");
        _add(sec.count());
        _add(R"(,"f":")");
        _add(format);
        _add(R"(","a":[null)");
    } catch(...) {
    }
    return true;
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::add_nothing(
  const identifier arg,
  const identifier tag) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"("})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::do_add_arg_u(
  const identifier arg,
  const identifier tag,
  const auto value) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"(","v":)");
        _add(value);
        _add(R"(})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::do_add_arg_q(
  const identifier arg,
  const identifier tag,
  const auto value) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"(","v":")");
        _add(value);
        _add(R"("})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::add_bool(
  const identifier arg,
  const identifier tag,
  const bool value) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"(","v":)");
        _add(value);
        _add(R"(})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::add_float(
  const identifier arg,
  const identifier tag,
  const float min,
  const float value,
  const float max) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"(","v":)");
        _add(value);
        _add(R"(,"min":)");
        _add(min);
        _add(R"(,"max":)");
        _add(max);
        _add(R"(})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::add_duration(
  const identifier arg,
  const identifier tag,
  const std::chrono::duration<float> value) noexcept {
    try {
        _add(R"(,{"n":")");
        _add(arg.name());
        _add(R"(","t":")");
        _add(tag.name());
        _add(R"(","u":"s","v":)");
        _add(value.count());
        _add(R"(})");
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::finish_message() noexcept {
    try {
        _add("]}");
        _flush(true);
        _lockable.unlock();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::log_chart_sample(
  const identifier source,
  const logger_instance_id instance,
  const identifier series,
  const float value) noexcept {
    try {
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::heartbeat() noexcept {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add(R"(,{"t":"hb")");
        _add(R"(,"ts":)");
        _add(sec.count());
        _add(R"(})");
        _flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
template <typename Lockable>
void json_log_backend<Lockable>::finish_log() noexcept {
    try {
        const auto now{std::chrono::steady_clock::now()};
        const auto sec{std::chrono::duration<float>(now - _start)};
        const std::lock_guard<Lockable> lock{_lockable};
        _add(R"(,{"t":"end")");
        _add(R"(,"ts":)");
        _add(sec.count());
        _add(R"(}])");
        _flush(true);
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------
auto make_json_log_backend(
  const log_stream_info& info,
  shared_holder<log_output_stream> out,
  const log_backend_lock lock_type) -> unique_holder<logger_backend> {
    switch(lock_type) {
        case log_backend_lock::none:
            return {hold<json_log_backend<no_lock>>, info, std::move(out)};
        case log_backend_lock::mutex:
            return {hold<json_log_backend<std::mutex>>, info, std::move(out)};
        case log_backend_lock::spinlock:
            return {hold<json_log_backend<spinlock>>, info, std::move(out)};
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
