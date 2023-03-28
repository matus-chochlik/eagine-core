/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:entry_arg;

import std;
import eagine.core.types;
import eagine.core.reflection;
import eagine.core.identifier;

namespace eagine {
//------------------------------------------------------------------------------
export template <default_mapped_enum T>
constexpr auto adapt_entry_arg(const identifier name, const T value) noexcept {
    return [=](auto& backend) {
        backend.add_string(name, "enum", enumerator_name(value));
    };
}
//------------------------------------------------------------------------------
export template <default_mapped_enum T>
constexpr auto adapt_entry_arg(
  const identifier name,
  const bitfield<T> bf) noexcept {
    return [=](auto& backend) {
        const auto func = [&backend, name, bf](const auto& info) {
            if(bf.has(static_cast<T>(info.value))) {
                backend.add_string(name, "bitfield", info.name);
            }
        };
        for_each_enumerator(func, std::type_identity<T>{});
    };
}
//------------------------------------------------------------------------------
export template <typename T>
concept adapted_for_log_entry = requires(identifier id, std::decay_t<T> value) {
    adapt_entry_arg(id, value);
};

export template <typename T, typename E>
concept argument_of_log =
  requires(E entry, identifier id, identifier tag, std::decay_t<T> value) {
      entry.arg(id, tag, value);
  };
//------------------------------------------------------------------------------
} // namespace eagine

