/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_ENTRY_ARG_HPP
#define EAGINE_LOGGING_ENTRY_ARG_HPP

#include "../bitfield.hpp"
#include "../reflect/map_enumerators.hpp"
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Helper class used in implementation of has_entry_adapter_t.
/// @ingroup logging
template <typename T>
struct does_have_entry_adapter {
private:
    template <
      typename X,
      typename = decltype(
        adapt_entry_arg(std::declval<identifier>(), std::declval<X>()))>
    static auto _test(X*) -> std::true_type;
    static auto _test(...) -> std::false_type;

public:
    // NOLINTNEXTLINE(hicpp-vararg)
    using type = decltype(_test(static_cast<T*>(nullptr)));
};

/// @brief Trait indicating if there is a log/console entry adapter for type T.
/// @ingroup logging
/// @see has_entry_adapter_v
template <typename T>
using has_entry_adapter_t = typename does_have_entry_adapter<T>::type;

/// @brief Trait indicating if there is a log/console entry adapter for type T.
/// @ingroup logging
/// @see has_entry_adapter_t
template <typename T>
constexpr const bool has_entry_adapter_v = has_entry_adapter_t<T>::value;
//------------------------------------------------------------------------------
template <typename T>
static constexpr auto adapt_entry_arg(
  const identifier name,
  const T value) noexcept requires(has_enumerator_mapping_v<T>) {
    return [=](auto& backend) {
        backend.add_string(name, EAGINE_ID(enum), enumerator_name(value));
    };
}
//------------------------------------------------------------------------------
template <typename T>
static constexpr auto adapt_entry_arg(
  const identifier name,
  const bitfield<T> bf) noexcept requires(has_enumerator_mapping_v<T>) {
    return [=](auto& backend) {
        const auto func = [&backend, name, bf](const auto& info) {
            if(bf.has(static_cast<T>(info.value))) {
                backend.add_string(name, EAGINE_ID(bitfield), info.name);
            }
        };
        for_each_enumerator(func, type_identity<T>{});
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif
