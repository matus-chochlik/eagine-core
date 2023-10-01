/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:stdexcept;
export import <exception>;
export import <stdexcept>;

export namespace std {
//------------------------------------------------------------------------------
using std::bad_exception;
using std::current_exception;
using std::domain_error;
using std::exception;
using std::exception_ptr;
using std::get_terminate;
using std::invalid_argument;
using std::length_error;
using std::logic_error;
using std::make_exception_ptr;
using std::nested_exception;
using std::out_of_range;
using std::overflow_error;
using std::range_error;
using std::rethrow_exception;
using std::rethrow_if_nested;
using std::runtime_error;
using std::set_terminate;
using std::terminate;
using std::terminate_handler;
using std::throw_with_nested;
using std::uncaught_exception;
using std::uncaught_exceptions;
using std::underflow_error;
//------------------------------------------------------------------------------
} // namespace std
