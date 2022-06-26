/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.debug:type_name;

import <string>;
import <typeinfo>;

namespace eagine {

auto demangle_type_name(const char*) noexcept -> std::string;

/// @brief Returns the demangled name for type T.
/// @ingroup type_utils
/// @note The result is implementation dependent.
export template <typename T>
auto type_name(const T&) noexcept -> std::string {
    return demangle_type_name(typeid(T).name());
}

/// @brief Returns the demangled name for type T.
/// @ingroup type_utils
/// @note The result is implementation dependent.
export template <typename T>
auto type_name() noexcept -> std::string {
    return demangle_type_name(typeid(T).name());
}

} // namespace eagine
