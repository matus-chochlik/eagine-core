/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <concepts>

export module std:concepts;

export namespace std {
//------------------------------------------------------------------------------
using std::assignable_from;
using std::common_reference_with;
using std::common_with;
using std::constructible_from;
using std::convertible_to;
using std::copy_constructible;
using std::copyable;
using std::default_initializable;
using std::derived_from;
using std::destructible;
using std::equality_comparable;
using std::equality_comparable_with;
using std::equivalence_relation;
using std::floating_point;
using std::integral;
using std::invocable;
using std::movable;
using std::move_constructible;
using std::predicate;
using std::regular;
using std::regular_invocable;
using std::relation;
using std::same_as;
using std::semiregular;
using std::signed_integral;
using std::strict_weak_order;
using std::swappable;
using std::swappable_with;
using std::totally_ordered;
using std::totally_ordered_with;
using std::unsigned_integral;
//------------------------------------------------------------------------------
} // namespace std
