/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.main_ctx;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.progress;
import eagine.core.console;

namespace eagine {
//------------------------------------------------------------------------------
auto main_ctx::_single_ptr() noexcept -> main_ctx*& {
    static main_ctx* the_ptr{nullptr};
    return the_ptr;
}
//------------------------------------------------------------------------------
main_ctx::main_ctx(main_ctx_getters& src)
  : _instance_id{src.instance_id()}
  , _source{src}
  , _default_alloc{src.default_allocator()}
  , _args{src.args()}
  , _cmplr_info{src.compiler()}
  , _bld_info{src.build()}
  , _ver_info{src.version()}
  , _cio{src.cio()}
  , _log{src.log()}
  , _progress{src.progress()}
  , _watchdog{src.watchdog()}
  , _app_config{src.config()}
  , _sys_info{src.system()}
  , _usr_info{src.user()}
  , _buffers{src.buffers()}
  , _scratch_space{src.scratch_space()}
  , _compressor{src.compressor()}
  , _workers{src.workers()}
  , _exe_path{src.exe_path()}
  , _app_name{src.app_name()} {
    assert(not _single_ptr());
    _single_ptr() = this;
    _log.configure(_app_config);
    _progress.configure(_app_config);
}
//------------------------------------------------------------------------------
main_ctx::~main_ctx() noexcept {
    assert(_single_ptr());
    _single_ptr() = nullptr;
}
//------------------------------------------------------------------------------
auto main_ctx::preinitialize() noexcept -> main_ctx& {
    _source.preinitialize();
    return *this;
}
//------------------------------------------------------------------------------
auto try_get_main_ctx() noexcept -> main_ctx_getters* {
    return main_ctx::try_get();
}
//------------------------------------------------------------------------------
// main_ctx_object-related
//------------------------------------------------------------------------------
main_ctx_log_backend_getter::main_ctx_log_backend_getter(
  main_ctx_getters& c) noexcept
  : _backend{c.log().backend()} {}
//------------------------------------------------------------------------------
main_ctx_log_backend_getter::main_ctx_log_backend_getter() noexcept
  : _backend{main_ctx::get().log().backend()} {}
//------------------------------------------------------------------------------
auto main_ctx_object::main_context() const noexcept -> main_ctx& {
    return main_ctx::get();
}
//------------------------------------------------------------------------------
auto main_ctx_object::process_instance_id() const noexcept
  -> process_instance_id_t {
    return main_context().instance_id();
}
//------------------------------------------------------------------------------
auto main_ctx_object::app_config() const noexcept -> application_config& {
    return main_context().config();
}
//------------------------------------------------------------------------------
auto main_ctx_object::cio() const noexcept -> const console& {
    return main_context().cio();
}

auto main_ctx_object::cio_print(
  const console_entry_kind kind,
  const string_view format) const noexcept -> console_entry {
    return cio().print(object_id(), kind, format);
}

auto main_ctx_object::cio_print(const string_view format) const noexcept
  -> console_entry {
    return cio().print(object_id(), format);
}

auto main_ctx_object::cio_warning(const string_view format) const noexcept
  -> console_entry {
    return cio().warning(object_id(), format);
}

auto main_ctx_object::cio_error(const string_view format) const noexcept
  -> console_entry {
    return cio().error(object_id(), format);
}
//------------------------------------------------------------------------------
auto main_ctx_object::progress() const noexcept -> const activity_progress& {
    return main_context().progress();
}
//------------------------------------------------------------------------------
auto main_ctx_object::workers() const noexcept -> workshop& {
    return main_context().workers();
}
//------------------------------------------------------------------------------
auto main_ctx_object::locate_service(identifier type_id) const noexcept
  -> std::shared_ptr<main_ctx_service> {
    return main_context().locate_service(type_id);
}
//------------------------------------------------------------------------------
} // namespace eagine
