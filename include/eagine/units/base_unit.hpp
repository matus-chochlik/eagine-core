/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_UNITS_BASE_UNIT_HPP
#define EAGINE_UNITS_BASE_UNIT_HPP

#include "base_dim.hpp"
#include "scales.hpp"

namespace eagine::units::base {

template <typename Dimension, typename Derived>
struct unit {
    using dimension = Dimension;
    using scale = scales::one;
    using type = Derived;
};

} // namespace eagine::units::base

#endif // EAGINE_UNITS_BASE_UNIT_HPP
