/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_CONSOLE_HPP
#define EAGINE_CONSOLE_CONSOLE_HPP

#include "entry.hpp"

namespace eagine {

class console {
public:
    console(console_backend& backend) noexcept
      : _backend{backend} {}

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

private:
    auto make_log_entry(
      const identifier source,
      const console_entry_kind kind,
      const string_view format) const noexcept -> console_entry {
        return {source, kind, format, _entry_backend(source, kind)};
    }

    auto _entry_backend(const identifier source, const console_entry_kind kind)
      const noexcept -> console_backend* {
        return _backend.entry_backend(source, kind);
    }

    console_backend& _backend;
};

} // namespace eagine
#endif
