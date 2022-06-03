/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:array_size;

namespace eagine {

/// @brief Returns the number of elements in the array passed as argument.
/// @ingroup type_utils
template <typename T, std::size_t N>
static constexpr auto array_size(const T (&)[N]) noexcept {
    return span_size(N);
}

} // namespace eagine
