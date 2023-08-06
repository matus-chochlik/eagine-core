/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <iterator>

export module std:iterator;
export import std_private_iterator_default_sentinel;

export namespace std {
//------------------------------------------------------------------------------
using std::advance;
using std::back_insert_iterator;
using std::back_inserter;
using std::begin;
using std::bidirectional_iterator_tag;
using std::cbegin;
using std::cend;
using std::contiguous_iterator_tag;
using std::crbegin;
using std::crend;
using std::data;
using std::default_sentinel;
using std::default_sentinel_t;
using std::distance;
using std::empty;
using std::end;
using std::forward_iterator;
using std::forward_iterator_tag;
using std::front_insert_iterator;
using std::front_inserter;
using std::input_iterator;
using std::input_iterator_tag;
using std::insert_iterator;
using std::inserter;
using std::iterator;
using std::iterator_traits;
using std::make_reverse_iterator;
using std::next;
using std::output_iterator_tag;
using std::prev;
using std::random_access_iterator_tag;
using std::rbegin;
using std::rend;
using std::reverse_iterator;
using std::size;
using std::ssize;
using std::unreachable_sentinel_t;

using std::operator==;
using std::operator!=;
using std::operator<;
using std::operator>;
using std::operator<=;
using std::operator>=;
using std::operator<=>;
using std::operator-;
using std::operator+;
// using __gnu_cxx::operator==;
// using __gnu_cxx::operator<=>;
//------------------------------------------------------------------------------
} // namespace std
