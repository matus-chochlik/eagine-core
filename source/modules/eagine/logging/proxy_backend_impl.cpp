/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.logging;

import eagine.core.build_config;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;
import eagine.core.identifier;
import :null_backend;
import :ostream_backend;
import :syslog_backend;
import :asio_backend;
import <chrono>;
import <functional>;
import <iostream>;
import <memory>;
import <mutex>;
import <string>;
import <vector>;

namespace eagine {
//------------------------------------------------------------------------------
class proxy_log_backend final : public logger_backend {
public:
    proxy_log_backend(log_stream_info info) noexcept
      : _info{std::move(info)} {}

    auto configure(basic_config&) -> bool final;

    auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;
    auto type_id() noexcept -> identifier final;

    void enter_scope(const identifier source) noexcept final;
    void leave_scope(const identifier source) noexcept final;

    void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view name,
      const string_view desc) noexcept final;

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

    void finish_log() noexcept final;

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final;

private:
    std::unique_ptr<logger_backend> _delegate;
    std::unique_ptr<std::vector<std::function<void()>>> _delayed{
      std::make_unique<std::vector<std::function<void()>>>()};
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
void proxy_log_backend::enter_scope(const identifier source) noexcept {
    if(_delayed) {
        assert(!_delegate);
        _delayed->emplace_back(
          [this, source]() { _delegate->enter_scope(source); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::leave_scope(const identifier source) noexcept {
    if(_delayed) [[likely]] {
        assert(!_delegate);
        _delayed->emplace_back(
          [this, source]() { _delegate->leave_scope(source); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::set_description(
  const identifier source,
  const logger_instance_id instance,
  const string_view name,
  const string_view desc) noexcept {
    if(_delayed) [[likely]] {
        assert(!_delegate);
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
auto proxy_log_backend::begin_message(
  const identifier source,
  const identifier tag,
  const logger_instance_id instance,
  const log_event_severity severity,
  const string_view format) noexcept -> bool {
    if(_delayed) [[likely]] {
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
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
        assert(!_delegate);
        _delayed->emplace_back([this]() { _delegate->finish_message(); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::finish_log() noexcept {
    if(_delayed) [[likely]] {
        assert(!_delegate);
        _delayed->emplace_back([this]() { _delegate->finish_log(); });
    }
}
//------------------------------------------------------------------------------
void proxy_log_backend::log_chart_sample(
  const identifier source,
  const logger_instance_id instance,
  const identifier series,
  const float value) noexcept {
    if(_delayed) [[likely]] {
        assert(!_delegate);
        _delayed->emplace_back([this, source, instance, series, value]() {
            _delegate->log_chart_sample(source, instance, series, value);
        });
    }
}
//------------------------------------------------------------------------------
auto proxy_log_choose_backend(
  basic_config& config,
  const std::string& name,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    if((name == "null") || (name == "none")) {
        return make_null_log_backend();
    } else if(name == "cerr") {
        return std::make_unique<ostream_log_backend<std::mutex>>(
          std::cerr, info);
    } else if(name == "cout") {
        return std::make_unique<ostream_log_backend<std::mutex>>(
          std::cout, info);
    } else if(name == "syslog") {
        return std::make_unique<syslog_log_backend<std::mutex>>(info);
    } else if(name == "network") {
        std::string nw_addr;
        config.fetch_string("log.network.address", nw_addr);
        return make_asio_tcpipv4_ostream_log_backend(nw_addr, info);
    } else if(name == "local") {
        return make_asio_local_ostream_log_backend(info);
    }

    if constexpr(debug_build) {
        try {
            return make_asio_local_ostream_log_backend(info);
        } catch(const std::system_error& err) {
            if(err.code().value() != ENOENT) {
                throw;
            }
        }
        try {
            std::string nw_addr;
            config.fetch_string("log.network.address", nw_addr);
            return make_asio_tcpipv4_ostream_log_backend(nw_addr, info);
        } catch(const std::system_error& err) {
            if(err.code().value() != ENOENT) {
                throw;
            }
        }
    }

    return std::make_unique<ostream_log_backend<std::mutex>>(std::clog, info);
}
//------------------------------------------------------------------------------
auto proxy_log_backend::configure(basic_config& config) -> bool {
    std::string backend_name;
    config.fetch_string("log.backend", backend_name);
    config.fetch("log.severity", _info.min_severity);
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
    return false;
}
//------------------------------------------------------------------------------
auto make_proxy_log_backend(log_stream_info info)
  -> std::unique_ptr<logger_backend> {
    return std::make_unique<proxy_log_backend>(std::move(info));
}
//------------------------------------------------------------------------------
} // namespace eagine
