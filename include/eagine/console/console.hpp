/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_CONSOLE_HPP
#define EAGINE_CONSOLE_CONSOLE_HPP

#include "../config/basic.hpp"
#include "../program_args.hpp"
#include "console_opts.hpp"
#include "entry.hpp"
#include <memory>

namespace eagine {

class console {
public:
    console(
      identifier app_id,
      const program_args& args,
      console_options& opts) noexcept
      : _backend{_init_backend(args, opts)}
      , _app_id{app_id} {}

    auto print(
      const identifier source,
      const console_entry_kind kind,
      const string_view format) const noexcept -> console_entry {
        return make_log_entry(source, kind, format);
    }

    auto print(const identifier source, const string_view format) const noexcept
      -> console_entry {
        return make_log_entry(source, console_entry_kind::info, format);
    }

    auto error(const identifier source, const string_view format) const noexcept
      -> console_entry {
        return make_log_entry(source, console_entry_kind::error, format);
    }

    auto warning(const identifier source, const string_view format)
      const noexcept -> console_entry {
        return make_log_entry(source, console_entry_kind::warning, format);
    }

private:
    static auto _init_backend(const program_args&, console_options&)
      -> std::unique_ptr<console_backend>;

    auto make_log_entry(
      const identifier source,
      const console_entry_kind kind,
      const string_view format) const noexcept -> console_entry {
        const auto [backend, entry_id] = _entry_backend(source, kind);
        return {source, 0U, entry_id, kind, format, backend};
    }

    auto _entry_backend(const identifier source, const console_entry_kind kind)
      const noexcept -> std::tuple<console_backend*, console_entry_id_t> {
        if(_backend) [[likely]] {
            return _backend->entry_backend(source, kind);
        }
        return {nullptr, 0};
    }

    std::unique_ptr<console_backend> _backend;
    identifier _app_id;
};

} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/console/console.inl>
#endif

#endif
