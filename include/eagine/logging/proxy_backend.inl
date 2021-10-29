/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/app_config.hpp>
#include <eagine/logging/asio_backend.hpp>
#include <eagine/logging/null_backend.hpp>
#include <eagine/logging/ostream_backend.hpp>
#include <eagine/logging/syslog_backend.hpp>

namespace eagine {
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto proxy_log_backend::entry_backend(
  const identifier source,
  const log_event_severity severity) noexcept -> logger_backend* {
    if(EAGINE_LIKELY(_delegate)) {
        return _delegate->entry_backend(source, severity);
    }
    return this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto proxy_log_backend::allocator() noexcept -> memory::shared_byte_allocator {
    if(EAGINE_LIKELY(_delegate)) {
        return _delegate->allocator();
    }
    return memory::default_byte_allocator();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto proxy_log_backend::type_id() noexcept -> identifier {
    if(EAGINE_LIKELY(_delegate)) {
        return _delegate->type_id();
    }
    return EAGINE_ID(Proxy);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::enter_scope(const identifier source) noexcept {
    if(_delayed) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, source]() { _delegate->enter_scope(source); });
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::leave_scope(const identifier source) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, source]() { _delegate->leave_scope(source); });
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::set_description(
  const identifier source,
  const logger_instance_id instance,
  const string_view name,
  const string_view desc) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
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
EAGINE_LIB_FUNC
auto proxy_log_backend::begin_message(
  const identifier source,
  const identifier tag,
  const logger_instance_id instance,
  const log_event_severity severity,
  const string_view format) noexcept -> bool {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, source, tag, instance, severity, format{to_string(format)}]() {
              _delegate->begin_message(source, tag, instance, severity, format);
          });
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_nothing(
  const identifier arg,
  const identifier tag) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, arg, tag]() { _delegate->add_nothing(arg, tag); });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_identifier(
  const identifier arg,
  const identifier tag,
  const identifier value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_identifier(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_message_id(
  const identifier arg,
  const identifier tag,
  const message_id value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_message_id(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_bool(
  const identifier arg,
  const identifier tag,
  const bool value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, arg, tag, value]() { _delegate->add_bool(arg, tag, value); });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_integer(
  const identifier arg,
  const identifier tag,
  const std::intmax_t value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_integer(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_unsigned(
  const identifier arg,
  const identifier tag,
  const std::uintmax_t value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_unsigned(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_float(
  const identifier arg,
  const identifier tag,
  const float value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back(
          [this, arg, tag, value]() { _delegate->add_float(arg, tag, value); });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_float(
  const identifier arg,
  const identifier tag,
  const float min,
  const float value,
  const float max) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, min, value, max]() {
            _delegate->add_float(arg, tag, min, value, max);
        });
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_duration(
  const identifier arg,
  const identifier tag,
  const std::chrono::duration<float> value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value]() {
            _delegate->add_duration(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_string(
  const identifier arg,
  const identifier tag,
  const string_view value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, arg, tag, value{to_string(value)}]() {
            _delegate->add_string(arg, tag, value);
        });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::add_blob(
  const identifier arg,
  const identifier tag,
  const memory::const_block value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
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
EAGINE_LIB_FUNC
void proxy_log_backend::finish_message() noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this]() { _delegate->finish_message(); });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::finish_log() noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this]() { _delegate->finish_log(); });
    }
}

//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void proxy_log_backend::log_chart_sample(
  const identifier source,
  const logger_instance_id instance,
  const identifier series,
  const float value) noexcept {
    if(EAGINE_LIKELY(_delayed)) {
        EAGINE_ASSERT(!_delegate);
        _delayed->emplace_back([this, source, instance, series, value]() {
            _delegate->log_chart_sample(source, instance, series, value);
        });
    }
}
//------------------------------------------------------------------------------
auto proxy_log_choose_backend(
  application_config& config,
  const std::string& name,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    if((name == "null") || (name == "none")) {
        return std::make_unique<null_log_backend>();
    } else if(name == "cerr") {
        return std::make_unique<ostream_log_backend<>>(std::cerr, info);
    } else if(name == "cout") {
        return std::make_unique<ostream_log_backend<>>(std::cout, info);
    } else if(name == "syslog") {
        return std::make_unique<syslog_log_backend<>>(info);
    } else if(name == "network") {
        std::string nw_addr;
        config.fetch("log.network.address", nw_addr);
        return std::make_unique<asio_tcpipv4_ostream_log_backend<>>(
          nw_addr, info);
#if EAGINE_HAS_ASIO_LOCAL_LOG_BACKEND
    } else if(name == "local") {
        return std::make_unique<asio_local_ostream_log_backend<>>(info);
#endif
    }

#if EAGINE_DEBUG
#if EAGINE_HAS_ASIO_LOCAL_LOG_BACKEND
    try {
        return std::make_unique<asio_local_ostream_log_backend<>>(info);
    } catch(const std::system_error& err) {
        if(err.code().value() != ENOENT) {
            throw;
        }
    }
#endif
    try {
        return std::make_unique<asio_tcpipv4_ostream_log_backend<>>(info);
    } catch(const std::system_error& err) {
        if(err.code().value() != ENOENT) {
            throw;
        }
    }
#endif

    return std::make_unique<ostream_log_backend<>>(std::clog, info);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto proxy_log_backend::configure(application_config& config) -> bool {
    std::string backend_name;
    config.fetch("log.backend", backend_name);
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
} // namespace eagine
