/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_OSTREAM_BACKEND_HPP
#define EAGINE_LOGGING_OSTREAM_BACKEND_HPP

#include "../base64dump.hpp"
#include "../memory/aligned_block.hpp"
#include "../memory/stack_alloc.hpp"
#include "backend.hpp"
#include <mutex>
#include <ostream>

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable = std::mutex>
class ostream_log_backend : public logger_backend {
public:
    ostream_log_backend(
      std::ostream& out,
      log_event_severity min_severity) noexcept
      : _out{out}
      , _min_severity{min_severity}
      , _start{std::chrono::steady_clock::now()} {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _out << "<?xml version='1.0' encoding='UTF-8'?>\n";
            _out << "<log start='" << _start.time_since_epoch().count()
                 << "'>\n";
        } catch(...) {
        }
    }

    ostream_log_backend(ostream_log_backend&&) = delete;
    ostream_log_backend(const ostream_log_backend&) = delete;
    auto operator=(ostream_log_backend&&) = delete;
    auto operator=(const ostream_log_backend&) = delete;

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return EAGINE_ID(OutStream);
    }

    auto entry_backend(
      const identifier,
      const log_event_severity severity) noexcept -> logger_backend* final {
        if(severity >= _min_severity) {
            return this;
        }
        return nullptr;
    }

    void enter_scope(const identifier scope) noexcept final {
        try {
            _lockable.lock();
            _out << "<s name='" << scope.name() << "'>\n";
        } catch(...) {
        }
    }

    void leave_scope(const identifier) noexcept final {
        try {
            _lockable.lock();
            _out << "</s>\n";
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
            _out << "<d";
            _out << " src='" << source.name() << "'";
            _out << " iid='" << instance << "'";
            _out << " dn='" << display_name << "'";
            _out << " desc='" << description << "'";
            _out << "/>\n";
            flush();
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
            _out << "<m";
            _out << " lvl='" << severity_name << "'";
            _out << " src='" << source_name << "'";
            if(tag) {
                _out << " tag='" << tag_name << "'";
            }
            _out << " iid='" << instance << "'";
            _out << " ts='" << sec.count() << "'";
            _out << ">";
            _out << "<f>" << format << "</f>";
        } catch(...) {
        }
        return true;
    }

    void add_nothing(const identifier arg, const identifier tag) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'/>";
        } catch(...) {
        }
    }

    void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << value.name() << "</a>";
        } catch(...) {
        }
    }

    void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id msg_id) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << msg_id.class_().name() << "." << msg_id.method().name()
                 << "</a>";
        } catch(...) {
        }
    }

    void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << (value ? "true" : "false") << "</a>";
        } catch(...) {
        }
    }

    void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << value << "</a>";
        } catch(...) {
        }
    }

    void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << value << "</a>";
        } catch(...) {
        }
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << value << "</a>";
        } catch(...) {
        }
    }

    void add_float(
      const identifier arg,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "' min='"
                 << min << "' max='" << max << "'>" << value << "</a>";
        } catch(...) {
        }
    }

    void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name()
                 << "' u='s'>" << value.count() << "</a>";
        } catch(...) {
        }
    }

    void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name()
                 << "' blob='true'>" << base64dump(value) << "</a>";
        } catch(...) {
        }
    }

    void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept final {
        try {
            _out << "<a n='" << arg.name() << "' t='" << tag.name() << "'>"
                 << value << "</a>";
        } catch(...) {
        }
    }

    void finish_message() noexcept final {
        try {
            _out << "</m>\n";
            flush();
            _lockable.unlock();
        } catch(...) {
        }
    }

    void finish_log() noexcept final {
        try {
            const std::lock_guard<Lockable> lock{_lockable};
            _out << "</log>\n" << std::flush;
            flush();
        } catch(...) {
        }
    }

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final {
        try {
            const auto now = std::chrono::steady_clock::now();
            const auto sec = std::chrono::duration<float>(now - _start);
            const auto source_name{source.name()};
            const auto series_name{series.name()};
            const std::lock_guard<Lockable> lock{_lockable};
            _out << "<c";
            _out << " src='" << source_name << "'";
            _out << " iid='" << instance << "'";
            _out << " ser='" << series_name << "'";
            _out << " ts='" << sec.count() << "'";
            _out << " v='" << value << "'";
            _out << "/>\n";
        } catch(...) {
        }
    }

    ~ostream_log_backend() noexcept override {
        finish_log();
    }

protected:
    virtual void flush() noexcept {}

private:
    Lockable _lockable{};
    std::ostream& _out;
    const log_event_severity _min_severity;
    const std::chrono::steady_clock::time_point _start;
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_OSTREAM_BACKEND_HPP
