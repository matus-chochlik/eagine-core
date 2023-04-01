/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<dlfcn.h>)
#include <dlfcn.h>
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 1
#endif
#else
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 0
#endif
#endif

module eagine.core.runtime;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;

namespace eagine {
//------------------------------------------------------------------------------
#if EAGINE_POSIX
class posix_executable_module : public executable_module {
public:
    using handle_type = void*;

    posix_executable_module(module_load_options opts)
      : _handle{::dlopen(nullptr, _opts_to_flags(opts))} {
        if(not _handle) {
            _message.assign(::dlerror());
        }
    }

    posix_executable_module(const string_view filename, module_load_options opts)
      : _handle{::dlopen(c_str(filename), _opts_to_flags(opts))} {
        if(not _handle) {
            _message.assign(::dlerror());
        }
    }

    /// @brief Closes the handle.
    ~posix_executable_module() noexcept {
        if(_handle) {
            ::dlclose(_handle);
        }
    }

    auto is_open() const noexcept -> bool final {
        return bool(_handle);
    }

    auto exports(const string_view name) noexcept -> bool final {
        ::dlerror();
        [[maybe_unused]] void* result = ::dlsym(_handle, c_str(name));
        if(const auto error{::dlerror()}) {
            _message.assign(error);
            return {};
        }
        return true;
    }

    auto find_object(const string_view name) -> std::optional<void*> final {
        ::dlerror();
        void* result = ::dlsym(_handle, c_str(name));
        if(const auto error{::dlerror()}) {
            _message.assign(error);
            return {};
        }
        return {result};
    }

    auto find_function(const string_view name)
      -> std::optional<void (*)()> final {
        ::dlerror();
        void* result = ::dlsym(_handle, c_str(name));
        if(const auto error{::dlerror()}) {
            _message.assign(error);
            return {};
        }
        return {reinterpret_cast<void (*)()>(result)};
    }

    auto error_message() const noexcept -> string_view final {
        return {_message};
    }

private:
    static auto _opts_to_flags(module_load_options opts) noexcept -> int {
        int result = 0;
        // TODO: other options
        if(opts.has(module_load_option::load_lazy)) {
            result |= RTLD_LAZY;
        } else if(opts.has(module_load_option::load_immediate)) {
            result |= RTLD_NOW;
        }
        if((result & (RTLD_LAZY | RTLD_NOW)) == 0) {
            result |= RTLD_LAZY;
        }
        return result;
    }

    handle_type _handle{nullptr};
    std::string _message{};
};
//------------------------------------------------------------------------------
shared_executable_module::shared_executable_module(
  nothing_t,
  module_load_options opts) noexcept
  : _module{std::make_shared<posix_executable_module>(opts)} {}
//------------------------------------------------------------------------------
shared_executable_module::shared_executable_module(
  const string_view filename,
  module_load_options opts) noexcept
  : _module{std::make_shared<posix_executable_module>(filename, opts)} {}
//------------------------------------------------------------------------------
#else
//------------------------------------------------------------------------------
shared_executable_module::shared_executable_module(
  module_load_options) noexcept {}
//------------------------------------------------------------------------------
shared_executable_module::shared_executable_module(
  const string_view,
  module_load_options) noexcept {}
#endif
//------------------------------------------------------------------------------
} // namespace eagine
