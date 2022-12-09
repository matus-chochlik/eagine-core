/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:string_literal;

import eagine.core.types;
import :string_span;
import <array>;
import <cstdint>;
import <utility>;

namespace eagine {
namespace memory {
//------------------------------------------------------------------------------
/// @brief Structural type wrapper for string literals.
/// @ingroup string_utils
/// @see string_view
export template <std::size_t N>
struct string_literal : std::array<const char, N> {

    template <std::size_t... I>
    constexpr string_literal(
      const char (&s)[N],
      std::index_sequence<I...>) noexcept
      : std::array<const char, N>{{s[I]...}} {}

    /// @brief Construction from a string literal (or char array).
    constexpr string_literal(const char (&s)[N]) noexcept
      : string_literal{s, std::make_index_sequence<N>()} {}

    /// @brief Conversion to string view.
    constexpr operator string_view() const noexcept {
        return string_view{this->data(), span_size(this->size()) - 1};
    }
};
//------------------------------------------------------------------------------
} // namespace memory
using memory::string_literal;
} // namespace eagine

