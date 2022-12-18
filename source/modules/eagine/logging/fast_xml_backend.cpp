/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <iterator>
#include <system_error>
module;

#include <cassert>

export module eagine.core.logging:fast_xml_backend;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.utility;
import :backend;
import <array>;
import <charconv>;
import <chrono>;
import <mutex>;
import <string>;
import <string_view>;
import <vector>;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable, typename Derived>
class fast_xml_log_backend : public logger_backend {
    auto self() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    template <std::size_t N>
    void _add(const char (&str)[N]) {
        _buffer.append(static_cast<const char*>(str), N - 1);
    }

    void _add(const bool data) {
        if(data) {
            _buffer.append("true");
        } else {
            _buffer.append("false");
        }
    }

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

    void _add(const memory::const_block data) {
        self().write(_buffer);
        _buffer.clear();
        base64dump(data).apply([this](const char c) {
            _b64lob.push_back(c);
            if(_b64lob.size() >= 512) {
                self().write(_b64lob);
                _b64lob.clear();
            }
        });
        self().write(_b64lob);
        _b64lob.clear();
    }

    template <typename T>
    void _add(const T& value) {
        std::array<char, 48> temp{};
        const auto conv{
          std::to_chars(temp.data(), temp.data() + temp.size(), value)};
        if(conv.ec == std::error_code{}) {
            _buffer.append(temp.data(), std::distance(temp.data(), conv.ptr));
        }
    }

    void _flush(bool force = false) noexcept {
        self().write(_buffer);
        self().flush(force);
        _buffer.clear();
    }

public:
    fast_xml_log_backend(const log_stream_info& info) noexcept
      : _log_identity{info.log_identity}
      , _min_severity{info.min_severity}
      , _start{std::chrono::steady_clock::now()} {
        _buffer.reserve(1024);
    }

    fast_xml_log_backend(fast_xml_log_backend&&) = delete;
    fast_xml_log_backend(const fast_xml_log_backend&) = delete;
    auto operator=(fast_xml_log_backend&&) = delete;
    auto operator=(const fast_xml_log_backend&) = delete;

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return _alloc;
    }

    auto type_id() noexcept -> identifier override {
        return "FastXML";
    }

    auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* final {
        if(severity >= _min_severity) {
            return this;
        }
        return nullptr;
    }

    void begin_log() noexcept final {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _add("<?xml version='1.0' encoding='UTF-8'?>\n");
            const auto start_tse{
              std::chrono::duration_cast<
                std::chrono::duration<std::int64_t, std::micro>>(
                _start.time_since_epoch())
                .count()};

            if(_log_identity.empty()) {
                _add("<log start_tse_us='");
                _add(start_tse);
                _add("'>\n");
            } else {
                _add("<log start_tse_us='");
                _add(start_tse);
                _add("' identity='");
                _add(_log_identity);
                _add("'>\n");
            }
            _flush();
        } catch(...) {
        }
    }

    void time_interval_begin(
      const identifier label,
      const logger_instance_id log_id,
      const time_interval_id int_id) noexcept final {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _intervals.emplace_back(
              int_id, log_id, label, std::chrono::steady_clock::now());
        } catch(...) {
        }
    }

    void time_interval_end(
      const identifier label,
      const logger_instance_id log_id,
      const time_interval_id int_id) noexcept final {
        try {
            const auto end{std::chrono::steady_clock::now()};
            const std::lock_guard<Lockable> lock{_lockable};
            const auto pos{std::find_if(
              _intervals.rbegin(),
              _intervals.rend(),
              [int_id](const auto& entry) {
                  return std::get<0>(entry) == int_id;
              })};
            assert(pos != _intervals.rend());
            _add("<i iid='");
            _add(std::get<1>(*pos));
            _add("' lbl='");
            _add(std::get<2>(*pos).name());
            _add("' tns='");
            _add(std::chrono::nanoseconds(end - std::get<3>(*pos)).count());
            _add("'/>\n");
            _flush();
            _intervals.erase(std::next(pos).base());
        } catch(...) {
        }
    }

    void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view display_name,
      const string_view description) noexcept final {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _add("<d src='");
            _add(source.name());
            _add("' iid='");
            _add(instance);
            _add(" dn='");
            _add(display_name);
            _add(" desc='");
            _add(description);
            _add("'/>\n");
            _flush();
        } catch(...) {
        }
    }

    auto begin_message(
      const identifier source,
      const identifier tag,
      const logger_instance_id instance,
      const log_event_severity severity,
      const string_view format) noexcept -> bool final {
        try {
            const auto now = std::chrono::steady_clock::now();
            const auto sec = std::chrono::duration<float>(now - _start);
            const auto severity_name{enumerator_name(severity)};
            const auto source_name{source.name()};
            const auto tag_name{tag.name()};
            _lockable.lock();
            _add("<m lvl='");
            _add(severity_name);
            _add("' src='");
            _add(source_name);
            if(tag) {
                _add("' tag='");
                _add(tag_name);
            }
            _add("' iid='");
            _add(instance);
            _add("' ts='");
            _add(sec.count());
            _add("'><f>");
            _add(format);
            _add("</f>");
        } catch(...) {
        }
        return true;
    }

    void add_nothing(const identifier arg, const identifier tag) noexcept final {
        try {
            _add("<a n='");
            _add(arg.name());
            _add("' t='");
            _add(tag.name());
            _add("'/>");
        } catch(...) {
        }
    }

    void do_add_arg(
      const identifier arg,
      const identifier tag,
      const auto value) noexcept {
        try {
            _add("<a n='");
            _add(arg.name());
            _add("' t='");
            _add(tag.name());
            _add("'>");
            _add(value);
            _add("</a>");
        } catch(...) {
        }
    }

    void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept final {
        try {
            _add("<a n='");
            _add(arg.name());
            _add("' t='");
            _add(tag.name());
            _add("' min='");
            _add(min);
            _add("' max='");
            _add(max);
            _add("'>");
            _add(value);
            _add("</a>");
        } catch(...) {
        }
    }

    void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept final {
        try {
            _add("<a n='");
            _add(arg.name());
            _add("' t='");
            _add(tag.name());
            _add("' u='s'>");
            _add(value.count());
            _add("</a>");
        } catch(...) {
        }
    }

    void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept final {
        do_add_arg(arg, tag, value);
    }

    void finish_message() noexcept final {
        try {
            _add("</m>\n");
            _flush(true);
            _lockable.unlock();
        } catch(...) {
        }
    }

    void finish_log() noexcept final {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _add("</log>\n");
            _flush(true);
        } catch(...) {
        }
    }

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final {
        try {
        } catch(...) {
        }
    }

private:
    Lockable _lockable{};
    const std::string _log_identity;
    const log_event_severity _min_severity;
    const std::chrono::steady_clock::time_point _start;
    std::string _buffer;
    std::string _b64lob;
    memory::shared_byte_allocator _alloc{memory::default_byte_allocator()};
    std::vector<std::tuple<
      time_interval_id,
      logger_instance_id,
      identifier,
      std::chrono::steady_clock::time_point>>
      _intervals;
};
//------------------------------------------------------------------------------
} // namespace eagine
