/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_CONFIG_HPP
#define EAGINE_REFLECT_CONFIG_HPP

#ifndef EAGINE_CXX_REFLECTION
#define EAGINE_CXX_REFLECTION 0
#endif

#if EAGINE_CXX_REFLECTION

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-member-function"
#pragma clang diagnostic ignored "-Wshadow-uncaptured-local"

#include <experimental/compiler>
#include <experimental/meta>

template <typename Iterator>
consteval auto _meta_iter_advance(Iterator it, std::size_t n) {
    while(n-- > 0) {
        ++it;
    }
    return it;
}

template <typename Range>
consteval auto _meta_range_at(Range range, std::size_t n) {
    return *_meta_iter_advance(range.begin(), n);
}

#pragma clang diagnostic pop

#endif

#endif // EAGINE_REFLECT_CONFIG_HPP
