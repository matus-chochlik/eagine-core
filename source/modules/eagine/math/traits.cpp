/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:traits;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
struct is_known_vector_type : std::false_type {};

export template <typename T>
constexpr const bool is_known_vector_type_v = is_known_vector_type<T>::value;

export template <typename T, span_size_t N>
struct is_known_vector_type<T[N]> : std::is_scalar<T> {};

export template <typename T>
struct is_known_matrix_type : std::false_type {};
//------------------------------------------------------------------------------
export template <typename T>
constexpr const bool is_known_matrix_type_v = is_known_matrix_type<T>::value;

export template <typename T, span_size_t C, span_size_t R>
struct is_known_matrix_type<T[C][R]> : std::is_scalar<T>{};
//------------------------------------------------------------------------------
export template <typename T>
struct canonical_compound_type : nothing_t {};

export template <typename T>
using canonical_compound_type_t = typename canonical_compound_type<T>::type;

export template <typename C, typename CT>
struct has_canonical_type : std::is_same<canonical_compound_type_t<C>, CT> {};

export template <typename T, span_size_t N>
struct canonical_compound_type<T[N]>
  : std::type_identity<std::remove_cv_t<T[N]>> {};

export template <typename T, span_size_t C, span_size_t R>
struct canonical_compound_type<T[C][R]>
  : std::type_identity<std::remove_cv_t<T[C][R]>>{};
//------------------------------------------------------------------------------
export template <typename T>
struct compound_element_type : std::type_identity<std::remove_cv_t<T>> {};

export template <typename T, span_size_t N>
struct compound_element_type<T[N]> : std::type_identity<std::remove_cv_t<T>> {};

export template <typename T, span_size_t C, span_size_t R>
struct compound_element_type<T[C][R]>
  : std::type_identity<std::remove_cv_t<T>>{};

export template <typename T>
struct canonical_element_type
  : compound_element_type<canonical_compound_type_t<T>> {};

export template <typename T>
using canonical_element_type_t = typename canonical_element_type<T>::type;
//------------------------------------------------------------------------------
export template <typename C>
struct compound_view_maker;

export template <typename C>
[[nodiscard]] auto element_view(const C& c) noexcept {
    return compound_view_maker<C>()(c);
}

export template <typename T, span_size_t N>
struct compound_view_maker<T[N]> {
    auto operator()(T (&v)[N]) const noexcept -> memory::span<T> {
        return {static_cast<T*>(v), N};
    }
};

export template <typename T, span_size_t N>
[[nodiscard]] auto element_view(T (&v)[N]) noexcept {
    return compound_view_maker<T[N]>()(v);
}
//------------------------------------------------------------------------------
export template <typename C>
struct is_row_major : std::false_type {};
//------------------------------------------------------------------------------
} // namespace eagine
