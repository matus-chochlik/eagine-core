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

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.serialization;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.progress;
import eagine.core.console;
import :interface;

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
  , _scheduler{src.scheduler()}
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
auto main_ctx::update() noexcept -> main_ctx& {
    _source.update();
    return *this;
}
//------------------------------------------------------------------------------
auto try_get_main_ctx() noexcept -> optional_reference<main_ctx_getters> {
    return main_ctx::try_get();
}
//------------------------------------------------------------------------------
static void main_ctx_do_scramble(memory::block unscrambled) noexcept {
    assert(unscrambled.size() % 8 == 0);
    std::array<byte, 8U> temp{};
    while(not unscrambled.empty()) {
        auto dst{head(unscrambled, 8)};
        copy(dst, cover(temp));
        zero(dst);
        const auto src{view(temp)};
        for(const auto by : integer_range(span_size(8))) {
            for(const auto bi : integer_range(span_size(8))) {
                dst[by] = dst[by] | (((src[bi] >> by) & 0x01U) << bi);
            }
        }
        unscrambled = skip(unscrambled, 8);
    }
}
//------------------------------------------------------------------------------
static auto main_ctx_do_unscramble(memory::block scrambled) noexcept -> bool {
    if(scrambled.size() % 8 != 0) {
        return false;
    }
    std::array<byte, 8U> temp{};
    while(not scrambled.empty()) {
        auto dst{head(scrambled, 8)};
        copy(dst, cover(temp));
        zero(dst);
        const auto src{view(temp)};
        for(const auto by : integer_range(span_size(8))) {
            for(const auto bi : integer_range(span_size(8))) {
                dst[by] = dst[by] | (((src[bi] >> by) & 0x01U) << bi);
            }
        }
        scrambled = skip(scrambled, 8);
    }
    return true;
}
//------------------------------------------------------------------------------
static auto main_ctx_scramble_encrypted(
  memory::const_block input,
  memory::buffer& output) noexcept -> memory::const_block {
    const span_size_t new_size{
      ((input.size() / 8) + ((input.size() % 8 != 0) ? 1 : 0)) * 8};
    output.resize(new_size + 24);

    const auto p{pad_and_store_data_with_size(input, new_size, cover(output))};
    main_ctx_do_scramble(std::get<1>(p));

    return std::get<0>(p);
}
//------------------------------------------------------------------------------
static auto main_ctx_unscramble_encrypted(
  memory::block input,
  memory::buffer& output) noexcept -> bool {
    auto p{get_padded_data_with_size(input)};
    if(not main_ctx_do_unscramble(std::get<0>(p))) {
        return false;
    }
    memory::copy_into(std::get<1>(p), output);
    return true;
}
//------------------------------------------------------------------------------
static auto main_ctx_encrypt_shared_with(
  memory::const_block nonce,
  const memory::const_block key,
  const memory::const_block input,
  memory::buffer& output) noexcept -> bool {
    memory::buffer temp;
    auto src{main_ctx_scramble_encrypted(input, temp)};
    output.resize(src.size());
    auto dst{cover(output)};
    const auto n{nonce.size()};
    const auto m{key.size()};
    assert(src.size() == dst.size());
    for(auto i : index_range(src)) {
        dst[i] = (src[i] ^ key[i % m] ^ nonce[i % n]);
    }
    return true;
}
//------------------------------------------------------------------------------
static auto main_ctx_decrypt_shared_with(
  memory::const_block nonce,
  const memory::const_block key,
  const memory::const_block input,
  memory::buffer& output) noexcept -> bool {
    auto src{input};
    memory::buffer temp;
    temp.resize(src.size());
    auto dst{cover(temp)};
    const auto n{nonce.size()};
    const auto m{key.size()};
    assert(src.size() == dst.size());
    for(auto i : index_range(src)) {
        dst[i] = (src[i] ^ key[i % m] ^ nonce[i % n]);
    }
    return main_ctx_unscramble_encrypted(dst, output);
}
//------------------------------------------------------------------------------
auto main_ctx::encrypt_shared(
  memory::const_block nonce,
  memory::const_block input,
  memory::buffer& output) noexcept -> bool {
    if(not nonce.empty()) {
        memory::buffer key{};
        if(config().fetch("main_key", key)) {
            if(not key.empty()) {
                return main_ctx_encrypt_shared_with(
                  nonce, view(key), input, output);
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto main_ctx::decrypt_shared(
  memory::const_block nonce,
  memory::const_block input,
  memory::buffer& output) noexcept -> bool {
    if(not nonce.empty()) {
        memory::buffer key{};
        if(config().fetch("main_key", key)) {
            if(not key.empty()) {
                return main_ctx_decrypt_shared_with(
                  nonce, view(key), input, output);
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto main_ctx::encrypt_shared(
  memory::const_block nonce,
  string_view input,
  memory::buffer& output) noexcept -> bool {
    return encrypt_shared(nonce, as_bytes(input), output);
}
//------------------------------------------------------------------------------
auto main_ctx::decrypt_shared(
  memory::const_block nonce,
  memory::const_block input,
  std::string& output) noexcept -> bool {
    memory::buffer temp;
    if(decrypt_shared(nonce, input, temp)) {
        output.resize(std_size(temp.size()));
        auto src{as_chars(view(temp))};
        std::copy(src.begin(), src.end(), output.begin());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
auto main_ctx::encrypt_shared_password(
  memory::const_block nonce,
  const string_view key,
  const string_view tag,
  memory::buffer& encrypted) noexcept -> bool {
    std::string passwd;
    if(config().fetch(key, passwd, tag)) {
        return encrypt_shared(nonce, passwd, encrypted);
    }
    return false;
}
//------------------------------------------------------------------------------
auto main_ctx::matches_encrypted_shared_password(
  memory::const_block nonce,
  const string_view key,
  const string_view tag,
  memory::const_block encrypted) noexcept -> bool {
    std::string received;
    if(decrypt_shared(nonce, encrypted, received)) {
        std::string passwd;
        if(config().fetch(key, passwd, tag)) {
            return received == passwd;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
// main_ctx_object-related
//------------------------------------------------------------------------------
main_ctx_log_backend_getter::main_ctx_log_backend_getter(
  main_ctx_getters& c) noexcept {
    const auto new_ref{c.log().backend()};
    if(_backend_ref()) [[likely]] {
        assert(not new_ref or _backend_ref().refers_to(new_ref));
    } else if(new_ref) {
        _backend_ref() = new_ref;
    }
}
//------------------------------------------------------------------------------
main_ctx_log_backend_getter::main_ctx_log_backend_getter() noexcept {
    const auto new_ref{main_ctx::get().log().backend()};
    if(_backend_ref()) [[likely]] {
        assert(not new_ref or _backend_ref().refers_to(new_ref));
    } else if(new_ref) {
        _backend_ref() = new_ref;
    }
}
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
auto main_ctx_object::scheduler() const noexcept -> action_scheduler& {
    return main_context().scheduler();
}
//------------------------------------------------------------------------------
auto main_ctx_object::workers() const noexcept -> workshop& {
    return main_context().workers();
}
//------------------------------------------------------------------------------
auto main_ctx_object::locate_service(identifier type_id) const noexcept
  -> optional_reference<main_ctx_service> {
    return main_context().locate_service(type_id);
}
//------------------------------------------------------------------------------
auto main_ctx_object::share_service(identifier type_id) const noexcept
  -> shared_holder<main_ctx_service> {
    return main_context().share_service(type_id);
}
//------------------------------------------------------------------------------
} // namespace eagine
