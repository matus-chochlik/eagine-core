/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:chrono;
export import <chrono>;

export namespace std::chrono {
//------------------------------------------------------------------------------
using std::chrono::abs;
using std::chrono::ceil;
using std::chrono::days;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::floor;
using std::chrono::hours;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::minutes;
using std::chrono::months;
using std::chrono::nanoseconds;
using std::chrono::round;
using std::chrono::seconds;
using std::chrono::steady_clock;
using std::chrono::sys_days;
using std::chrono::sys_seconds;
using std::chrono::sys_time;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::time_point_cast;
using std::chrono::weeks;
using std::chrono::years;

using std::chrono::operator+;
using std::chrono::operator-;
using std::chrono::operator*;
using std::chrono::operator/;
using std::chrono::operator%;

using std::chrono::operator==;
using std::chrono::operator<;
using std::chrono::operator>;
using std::chrono::operator<=;
using std::chrono::operator>=;
using std::chrono::operator<=>;
using std::chrono::operator<<;
//------------------------------------------------------------------------------
} // namespace std::chrono
