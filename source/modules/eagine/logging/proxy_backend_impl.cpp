/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>
#include <cerrno>

module eagine.core.logging;

import std;
import eagine.core.build_config;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.reflection;
import eagine.core.identifier;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
class proxy_log_backend final : public logger_backend {
public:
    proxy_log_backend(log_stream_info info) noexcept
      : _info{std::move(info)} {}

    auto configure(basic_config_intf&) -> bool final;

    auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;
    auto type_id() noexcept -> identifier final;

    void make_more_verbose() noexcept final;
    void make_less_verbose() noexcept final;

    void begin_log() noexcept final;

    auto register_time_interval(
      const identifier tag,
      const logger_instance_id) noexcept -> time_interval_id final;

    void time_interval_begin(const time_interval_id) noexcept final;

    void time_interval_end(const time_interval_id) noexcept final;

    void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view name,
      const string_view desc) noexcept final;

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

    void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept final;

    void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id value) noexcept final;

    void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept final;

    void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept final;

    void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept final;

    void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept final;

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
      const string_view value) noexcept final;

    void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept final;

    void finish_message() noexcept final;

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final;

    void heartbeat() noexcept final;

    void finish_log() noexcept final;

private:
    unique_holder<logger_backend> _delegate;
    unique_holder<std::vector<std::function<void()>>> _delayed{
      default_selector};
    log_stream_info _info;
};
//------------------------------------------------------------------------------
auto proxy_log_backend::entry_backend(const log_event_severity severity) noexcept
  -> logger_backend* {
    if(_delegate) [[likely]] {
        return _delegate->entry_backend(severity);
    }
    return this;
}
//------------------------------------------------------------------------------
auto proxy_log_backend::allocator() noexcept -> memory::shared_byte_allocator {
    if(_delegate) [[likely]] {
        return _delegate->allocator();
    }
    return memory::default_byte_allocator();
}
//------------------------------------------------------------------------------
auto proxy_log_backend::type_id() noexcept -> identifier {
    if(_delegate) [[likely]] {
        return _delegate->type_id();
    }
    return "Proxy";
}
//------------------------------------------------------------------------------
void proxy_log_backend::make_more_verbose() noexcept {
    if(_delegate) [[likely]] {
        _delegate->make_more_verbose();
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::make_less_verbose() noexcept {
    if(_delegate) [[likely]] {
        _delegate->make_less_verbose();
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::begin_log() noexcept {
    if(_delegate) {
        _delegate->begin_log();
    } else if(_delayed) {
        assert(not _delegate);
        _delayed->emplace_back([this]() { _delegate->begin_log(); });
    }
}
//------------------------------------------------------------------------------
auto proxy_log_backend::register_time_interval(
  const identifier tag,
  const logger_instance_id log_id) noexcept -> time_interval_id {
    if(_delegate) [[likely]] {
        return _delegate->register_time_interval(tag, log_id);
    }
    return 0U;
}
//------------------------------------------------------------------------------
void proxy_log_backend::time_interval_begin(
  const time_interval_id int_id) noexcept {
    if(_delegate) [[likely]] {
        _delegate->time_interval_begin(int_id);
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::time_interval_end(
  const time_interval_id int_id) noexcept {
    if(_delegate) [[likely]] {
        _delegate->time_interval_end(int_id);
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::set_description(
  const identifier source,
  const logger_instance_id instance,
  const string_view name,
  const string_view desc) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this,
                                source,
                                instance,
                                name{to_string(name)},
                                desc{to_string(desc)}]() {
            _delegate->set_description(source, instance, name, desc);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::declare_state(
  const identifier source,
  const identifier state_tag,
  const identifier begin_tag,
  const identifier end_tag) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, source, state_tag, begin_tag, end_tag]() {
            _delegate->declare_state(source, state_tag, begin_tag, end_tag);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::active_state(
  const identifier source,
  const identifier state_tag) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, source, state_tag]() {
            _delegate->active_state(source, state_tag);
        });
    }
}
//------------------------------------------------------------------------------
auto proxy_log_backend::begin_message(
  const identifier source,
  const identifier tag,
  const logger_instance_id instance,
  const log_event_severity severity,
  const string_view format) noexcept -> bool {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back(
          [this, source, tag, instance, severity, format{to_string(format)}]() {
              _delegate->begin_message(source, tag, instance, severity, format);
          });
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_nothing(
  const identifier arg,
  const identifier tag) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back(
          [this, arg, tag]() { _delegate->add_nothing(arg, tag); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_identifier(
  const identifier arg,
  const identifier tag,
  const identifier value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_identifier(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_message_id(
  const identifier arg,
  const identifier tag,
  const message_id value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_message_id(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_bool(
  const identifier arg,
  const identifier tag,
  const bool value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back(
          [this, arg, tag, value]() { _delegate->add_bool(arg, tag, value); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_integer(
  const identifier arg,
  const identifier tag,
  const std::intmax_t value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_integer(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_unsigned(
  const identifier arg,
  const identifier tag,
  const std::uintmax_t value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_unsigned(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_float(
  const identifier arg,
  const identifier tag,
  const float value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back(
          [this, arg, tag, value]() { _delegate->add_float(arg, tag, value); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_float(
  const identifier arg,
  const identifier tag,
  const float min,
  const float value,
  const float max) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, min, value, max]() {
            _delegate->add_float(arg, tag, min, value, max);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_duration(
  const identifier arg,
  const identifier tag,
  const std::chrono::duration<float> value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_duration(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_string(
  const identifier arg,
  const identifier tag,
  const string_view value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, arg, tag, value{to_string(value)}]() {
            _delegate->add_string(arg, tag, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::add_blob(
  const identifier arg,
  const identifier tag,
  const memory::const_block value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back(
          [this,
           arg,
           tag,
           value{std::vector<byte>(value.begin(), value.end())}]() {
              _delegate->add_blob(arg, tag, view(value));
          });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::finish_message() noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this]() { _delegate->finish_message(); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::log_chart_sample(
  const identifier source,
  const logger_instance_id instance,
  const identifier series,
  const float value) noexcept {
    if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this, source, instance, series, value]() {
            _delegate->log_chart_sample(source, instance, series, value);
        });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::heartbeat() noexcept {
    if(_delegate) [[likely]] {
        return _delegate->heartbeat();
    } else if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this]() { _delegate->heartbeat(); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::finish_log() noexcept {
    if(_delegate) [[likely]] {
        return _delegate->finish_log();
    } else if(_delayed) [[likely]] {
        assert(not _delegate);
        _delayed->emplace_back([this]() { _delegate->finish_log(); });
    }
}
//------------------------------------------------------------------------------
auto proxy_log_choose_backend(
  basic_config_intf& config,
  const std::string& name,
  const log_stream_info& info) -> unique_holder<logger_backend> {
    const auto lock_type{[&] {
        std::string temp;
        if(config.fetch_string("log.lock_type", temp)) {
            if(const auto val{from_string<log_backend_lock>(temp)}) {
                return *val;
            }
        }
        return log_backend_lock::mutex;
    }()};
    const auto log_format{[&] {
        std::string temp;
        if(config.fetch_string("log.data_format", temp)) {
            if(const auto val{from_string<log_data_format>(temp)}) {
                return *val;
            }
        }
        return log_data_format::json;
    }()};

    if((name == "null") or (name == "none")) {
        return make_null_log_backend();
    } else if(name == "cerr") {
        return make_ostream_log_backend(std::cerr, info, log_format, lock_type);
    } else if(name == "syslog") {
        return make_syslog_log_backend(info, lock_type);
    } else if(name == "network") {
        std::string nw_addr;
        config.fetch_string("log.network.address", nw_addr);
        return make_asio_tcpipv4_log_backend(
          nw_addr, info, log_format, lock_type);
    } else if(name == "local") {
        std::string path;
        config.fetch_string("log.local.address", path);
        return make_asio_local_log_backend(path, info, log_format, lock_type);
    }

    if constexpr(debug_build) {
        try {
            return make_asio_local_log_backend({}, info, log_format, lock_type);
        } catch(const std::system_error& err) {
            if(err.code().value() != ENOENT) {
                throw;
            }
        }
        try {
            std::string nw_addr;
            config.fetch_string("log.network.address", nw_addr);
            return make_asio_tcpipv4_log_backend(
              nw_addr, info, log_format, lock_type);
        } catch(const std::system_error& err) {
            if(err.code().value() != ENOENT) {
                throw;
            }
        }
    }

    return make_ostream_log_backend(std::cerr, info, log_format, lock_type);
}
//------------------------------------------------------------------------------
auto proxy_log_backend::configure(basic_config_intf& config) -> bool {
    std::string backend_name;
    config.fetch_string("log.backend", backend_name);
    config.fetch("log.severity", _info.min_severity);
    try {
        _delegate = proxy_log_choose_backend(config, backend_name, _info);
        if(_delegate) {
            _delegate->configure(config);
            if(_delayed) {
                for(auto& op : *_delayed) {
                    op();
                }
                _delayed.reset();
            }
            return true;
        }
    } catch(std::system_error& rte) {
        throw log_backend_error(rte);
    }
    return false;
}
//------------------------------------------------------------------------------
auto make_proxy_log_backend(log_stream_info info)
  -> unique_holder<logger_backend> {
    return {hold<proxy_log_backend>, std::move(info)};
}
//------------------------------------------------------------------------------
} // namespace eagine
