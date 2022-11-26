/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:string_set;

import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
export template <memory::string_literal... Strs>
class static_string_set {
public:
    /// @brief Indicates if this string set contains the specified string.
    [[nodiscard]] constexpr auto contains(const string_view str) const noexcept {
        return (... || (string_view{Strs} == str));
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
