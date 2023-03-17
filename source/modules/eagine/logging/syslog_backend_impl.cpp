/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<sys/types.h>) && \
	__has_include(<syslog.h>) && \
	__has_include(<unistd.h>)
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 1
#endif
#else
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 0
#endif
#endif

module eagine.core.logging;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.identifier;
import eagine.core.container;
import eagine.core.string;
import std;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable>
class syslog_log_backend final : public logger_backend {
public:
    syslog_log_backend(const log_stream_info& info)
      : _min_severity{info.min_severity} {}

    auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* final {
        if(severity >= _min_severity) {
            return this;
        }
        return nullptr;
    }

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return "Syslog";
    }

    void begin_log() noexcept final {
#if EAGINE_POSIX
        ::openlog(
          "EAGine",
          LOG_CONS | LOG_NDELAY | LOG_PID, // NOLINT(hicpp-signed-bitwise)
          LOG_LOCAL0);                     // NOLINT(hicpp-signed-bitwise)
#endif
    }

    void time_interval_begin(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final {}

    void time_interval_end(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final {}

    void set_description(
      [[maybe_unused]] const identifier src,
      const logger_instance_id inst,
      const string_view name,
      const string_view desc) noexcept final {
#if EAGINE_POSIX
        const auto idn{src.name()};
        ::syslog( // NOLINT(hicpp-vararg)
          _translate(log_event_severity::info),
          sizeof(logger_instance_id) > 4 ? "%16lx|%10.*s|name=%s, desc=%s"
                                         : "%8lx|%10.*s|name=%s, desc=%s",
          static_cast<unsigned long>(inst),
          int(idn.size()),
          idn.data(),
          c_str(name).c_str(),
          c_str(desc).c_str());
#endif
    }

    auto begin_message(
      const identifier src,
      const identifier tag,
      const logger_instance_id inst,
      const log_event_severity,
      const string_view format) noexcept -> bool final {

        auto& msg = _new_message();
        msg.source = src;
        msg.tag = tag;
        msg.instance = inst;
        msg.format.assign(
          sizeof(logger_instance_id) > 4 ? "%16lx|%10.*s|%10.*s|"
                                         : "%8lx|%10.*s|%10.*s|");
        _translate(format, msg);
        return true;
    }

    void do_add_arg(const identifier arg, const string_view value) noexcept {
        auto& msg = _get_message();
        const auto aidv = arg.value();
        bool found = false;
        for(auto [idx, idv] : msg.arg_idx) {
            if(idv == aidv) {
                found = true;
                break;
            }
        }
        if(found) {
            msg.arg_map[aidv].assign(value);
        } else {
            msg.format.append(", ");
            msg.format.append(arg.name().view());
            msg.format.append("=");
            msg.format.append(value);
        }
    }

    void add_nothing(const identifier, const identifier) noexcept final {}

    void add_identifier(
      const identifier arg,
      const identifier,
      const identifier value) noexcept final {
        do_add_arg(arg, value.name().view());
    }

    void add_message_id(
      const identifier arg,
      const identifier,
      const message_id value) noexcept final {
        std::string temp;
        temp.reserve(21);
        temp.append(value.class_().name().view());
        temp.append(".");
        temp.append(value.method().name().view());
        do_add_arg(arg, temp);
    }

    void add_bool(
      const identifier arg,
      const identifier,
      const bool value) noexcept final {
        do_add_arg(arg, value ? string_view{"True"} : string_view{"False"});
    }

    void add_integer(
      const identifier arg,
      const identifier,
      const std::intmax_t value) noexcept final {
        using std::to_string;
        do_add_arg(arg, to_string(value));
    }

    void add_unsigned(
      const identifier arg,
      const identifier,
      const std::uintmax_t value) noexcept final {
        using std::to_string;
        do_add_arg(arg, to_string(value));
    }

    void add_float(
      const identifier arg,
      const identifier,
      const float value) noexcept final {
        using std::to_string;
        do_add_arg(arg, to_string(value));
    }

    void add_float(
      const identifier arg,
      const identifier,
      const float,
      const float value,
      const float) noexcept final {
        using std::to_string;
        do_add_arg(arg, to_string(value));
    }

    void add_duration(
      const identifier arg,
      const identifier,
      const std::chrono::duration<float> value) noexcept final {
        using std::to_string;
        std::string temp(to_string(value.count()));
        temp.append("[s]");
        do_add_arg(arg, temp);
    }

    void add_string(
      const identifier arg,
      const identifier,
      const string_view value) noexcept final {
        using std::to_string;
        do_add_arg(arg, value);
    }

    void add_blob(
      const identifier,
      const identifier,
      const memory::const_block) noexcept final {}

    void finish_message() noexcept final {
        _do_log(_get_message());
        _del_message();
    }

    void finish_log() noexcept final {
#if EAGINE_POSIX
        ::closelog();
#endif
    }

    void log_chart_sample(
      const identifier,
      const logger_instance_id,
      const identifier,
      const float) noexcept final {}

private:
    Lockable _lockable{};
    log_event_severity _min_severity;

    struct _message_state {
        identifier source;
        identifier tag;
        logger_instance_id instance{0};
        std::string format;
        flat_map<std::size_t, identifier_t> arg_idx;
        flat_map<identifier_t, std::string> arg_map;
        std::size_t arg_count{0};
    };

    _message_state _the_message{};

    auto _new_message() noexcept -> auto& {
        _lockable.lock();
        _the_message.arg_count = 0;
        for(auto& entry : _the_message.arg_idx) {
            std::get<1>(entry) = 0;
        }
        return _the_message;
    }

    auto _get_message() noexcept -> auto& {
        return _the_message;
    }

    void _del_message() noexcept {
        _lockable.unlock();
    }

    static constexpr auto _translate(
      [[maybe_unused]] const log_event_severity severity) noexcept -> int {
#if EAGINE_POSIX
        switch(severity) {
            case log_event_severity::fatal:
                return LOG_CRIT;
            case log_event_severity::error:
                return LOG_ERR;
            case log_event_severity::warning:
                return LOG_WARNING;
            case log_event_severity::info:
            case log_event_severity::stat:
                return LOG_INFO;
            case log_event_severity::debug:
                return LOG_DEBUG;
            case log_event_severity::trace:
                return LOG_NOTICE;
            case log_event_severity::backtrace:
                break;
        }
#endif
        return 0;
    }

    static void _translate(string_view format, _message_state& msg) {
        while(auto pos{find_position(format, string_view{"${"})}) {
            const string_view prev{head(format, extract(pos))};
            msg.format.append(prev);
            format = skip(format, extract(pos) + 2);
            if(const auto end{find_position(format, string_view{"}"})}) {
                msg.arg_idx[msg.arg_count++] =
                  identifier(head(format, extract(end))).value();
                msg.format.append("%s");
                format = skip(format, extract(end) + 1);
            }
        }
        msg.format.append(format);
    }

    template <std::size_t... I>
    void _do_log_I(_message_state& msg, const std::index_sequence<I...>) {
        const auto source_name = msg.source.name();
        const auto tag_name = msg.tag ? msg.tag.name().str() : std::string();
        ::syslog( // NOLINT(hicpp-vararg)
          _translate(log_event_severity::info),
          msg.format.c_str(),
          static_cast<unsigned long>(msg.instance),
          int(source_name.size()),
          source_name.data(),
          int(tag_name.size()),
          tag_name.data(),
          msg.arg_map[msg.arg_idx[I]].c_str()...);
    }

    template <std::size_t N>
    auto _do_log_N(_message_state& msg) -> bool {
        if constexpr(N > 0) {
            if(_do_log_N<N - 1>(msg)) {
                return true;
            }
        }
        if(msg.arg_count == N) {
            _do_log_I(msg, std::make_index_sequence<N>());
            return true;
        }
        return false;
    }

    void _do_log(_message_state& msg) {
        _do_log_N<12>(msg);
    }
};
//------------------------------------------------------------------------------
auto make_syslog_log_backend_mutex(const log_stream_info& info)
  -> std::unique_ptr<logger_backend> {
    return std::make_unique<syslog_log_backend<std::mutex>>(info);
}

auto make_syslog_log_backend_spinlock(const log_stream_info& info)
  -> std::unique_ptr<logger_backend> {
    return std::make_unique<syslog_log_backend<spinlock>>(info);
}
//------------------------------------------------------------------------------
} // namespace eagine

