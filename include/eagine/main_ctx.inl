/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/logging/logger.hpp>
#include <eagine/main_ctx_storage.hpp>
#include <eagine/progress/activity.hpp>

namespace eagine {
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx::_single_ptr() noexcept -> main_ctx*& {
    static main_ctx* the_ptr{nullptr};
    return the_ptr;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
main_ctx::main_ctx(main_ctx_getters& src) noexcept
  : _instance_id{src.instance_id()}
  , _source{src}
  , _default_alloc{src.default_allocator()}
  , _args{src.args()}
  , _cmplr_info{src.compiler()}
  , _bld_info{src.build()}
  , _log{src.log()}
  , _progress{src.progress()}
  , _watchdog{src.watchdog()}
  , _app_config{src.config()}
  , _sys_info{src.system()}
  , _usr_info{src.user()}
  , _scratch_space{src.scratch_space()}
  , _compressor{src.compressor()}
  , _exe_path{src.exe_path()}
  , _app_name{src.app_name()} {
    EAGINE_ASSERT(!_single_ptr());
    _single_ptr() = this;
    _log.configure(_app_config);
    _progress.configure(_app_config);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
main_ctx::~main_ctx() noexcept {
    EAGINE_ASSERT(_single_ptr());
    _single_ptr() = nullptr;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx::preinitialize() noexcept -> main_ctx& {
    _source.preinitialize();
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto try_get_main_ctx() noexcept -> main_ctx_getters* {
    return main_ctx::try_get();
}
//------------------------------------------------------------------------------
// main_ctx_object-related
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
main_ctx_log_backend_getter::main_ctx_log_backend_getter(
  main_ctx_getters& c) noexcept
  : _backend{c.log().backend()} {}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
main_ctx_log_backend_getter::main_ctx_log_backend_getter() noexcept
  : _backend{main_ctx::get().log().backend()} {}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx_object::main_context() const noexcept -> main_ctx& {
    return main_ctx::get();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx_object::process_instance_id() const noexcept
  -> process_instance_id_t {
    return main_context().instance_id();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx_object::app_config() const noexcept -> application_config& {
    return main_context().config();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx_object::progress() const noexcept -> const activity_progress& {
    return main_context().progress();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto main_ctx_object::bus() const noexcept -> message_bus& {
    return main_context().bus();
}
//------------------------------------------------------------------------------
} // namespace eagine
