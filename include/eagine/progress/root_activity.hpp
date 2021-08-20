/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_ROOT_ACTIVITY_HPP
#define EAGINE_PROGRESS_ROOT_ACTIVITY_HPP

#include "activity.hpp"

namespace eagine {
//------------------------------------------------------------------------------
class root_activity : public activity_progress {
public:
    root_activity() noexcept
      : activity_progress{{}} {}
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_ROOT_ACTIVITY_HPP
