/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:span_algorithm;

import eagine.core.types;
import :span;
import <algorithm>;
import <concepts>;
import <functional>;
import <numeric>;
import <optional>;
import <tuple>;
import <type_traits>;

namespace eagine::memory {
//------------------------------------------------------------------------------
export template <typename T, typename P, typename S, std::integral I>
constexpr auto clamp_span_position(
  const basic_span<T, P, S> s,
  const I p) noexcept -> auto {
    return (p < 0) ? s.begin() : (p > s.size()) ? s.end() : s.begin() + p;
}
//------------------------------------------------------------------------------
export template <
  typename T,
  typename P,
  typename S,
  std::integral B,
  std::integral E>
constexpr auto subspan(const basic_span<T, P, S> s, const B b, const E e) noexcept
  -> basic_span<T, P, S> {
    return {clamp_span_position(s, b), clamp_span_position(s, e)};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span starting at specified index with specified length.
/// @ingroup memory
/// @see head
/// @see tail
/// @see skip
/// @see snip
export template <typename T, typename P, typename S, typename I, typename L>
constexpr auto slice(const basic_span<T, P, S> s, const I i, const L l) noexcept
  -> basic_span<T, P, S> {
    return {clamp_span_position(s, i), clamp_span_position(s, i + l)};
}
//------------------------------------------------------------------------------
/// @brief Skips a specified count of elements from the front of a span.
/// @ingroup memory
/// @see head
/// @see tail
/// @see slice
/// @see snip
/// @see shrink
export template <typename T, typename P, typename S, typename L>
constexpr auto skip(const basic_span<T, P, S> s, const L l) noexcept
  -> basic_span<T, P, S> {
    return slice(s, l, s.size() - l);
}
//------------------------------------------------------------------------------
/// @brief Snips a specified count of elements from the back of a span.
/// @ingroup memory
/// @see head
/// @see tail
/// @see slice
/// @see skip
/// @see shrink
export template <typename T, typename P, typename S, typename L>
constexpr auto snip(const basic_span<T, P, S> s, const L l) noexcept
  -> basic_span<T, P, S> {
    return head(s, s.size() - l);
}
//------------------------------------------------------------------------------
/// @brief Shrinks a span by removing a specified count of elements from both sides.
/// @ingroup memory
/// @see head
/// @see tail
/// @see slice
/// @see skip
/// @see shrink
export template <typename T, typename P, typename S, typename L>
constexpr auto shrink(const basic_span<T, P, S> s, const L l) noexcept
  -> basic_span<T, P, S> {
    return snip(skip(s, l), l);
}
//------------------------------------------------------------------------------
/// @brief Returns the first @p l elements from the front of a span.
/// @ingroup memory
/// @see tail
/// @see slice
/// @see skip
/// @see snip
/// @see shrink
export template <typename T, typename P, typename S, typename L>
constexpr auto head(const basic_span<T, P, S> s, const L l) noexcept
  -> basic_span<T, P, S> {
    return slice(s, S(0), l);
}
//------------------------------------------------------------------------------
/// @brief Returns the head of @p s l.size() elements long.
/// @ingroup memory
/// @see tail
/// @see slice
/// @see skip
/// @see snip
/// @see shrink
export template <
  typename Ts,
  typename Ps,
  typename Ss,
  typename Tl,
  typename Pl,
  typename Sl>
constexpr auto head(
  const basic_span<Ts, Ps, Ss> s,
  const basic_span<Tl, Pl, Sl> l) noexcept -> basic_span<Ts, Ps, Ss> {
    return head(s, l.size());
}
//------------------------------------------------------------------------------
/// @brief Returns the last @p l elements from the back of a span.
/// @ingroup memory
/// @see head
/// @see slice
/// @see skip
/// @see snip
/// @see shrink
export template <typename T, typename P, typename S, typename L>
constexpr auto tail(const basic_span<T, P, S> s, const L l) noexcept
  -> basic_span<T, P, S> {
    return skip(s, s.size() - l);
}
//------------------------------------------------------------------------------
/// @brief Returns the tail of @p s l.size() elements long.
/// @ingroup memory
/// @see head
/// @see slice
/// @see skip
/// @see snip
/// @see shrink
export template <
  typename Ts,
  typename Ps,
  typename Ss,
  typename Tl,
  typename Pl,
  typename Sl>
constexpr auto tail(
  const basic_span<Ts, Ps, Ss> s,
  const basic_span<Tl, Pl, Sl> l) noexcept -> basic_span<Ts, Ps, Ss> {
    return tail(s, l.size());
}
//------------------------------------------------------------------------------
/// @brief Indicates if span @p spn starts with the content of @p with.
/// @ingroup memory
/// @see ends_with
/// @see contains
/// @see strip_prefix
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto starts_with(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> with) -> bool {
    return are_equal(head(spn, with.size()), with);
}
//------------------------------------------------------------------------------
/// @brief Indicates if span @p spn ends with the content of @p with.
/// @ingroup memory
/// @see starts_with
/// @see contains
/// @see strip_suffix
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto ends_with(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> with) -> bool {
    return are_equal(tail(spn, with.size()), with);
}
//------------------------------------------------------------------------------
/// @brief Strips the specified @p prefix from a span.
/// @ingroup memory
/// @see starts_with
/// @see strip_suffix
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto strip_prefix(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> prefix) -> basic_span<T1, P1, S1> {
    return starts_with(spn, prefix) ? skip(spn, prefix.size()) : spn;
}
//------------------------------------------------------------------------------
/// @brief Strips the specified @p suffix from a span.
/// @ingroup memory
/// @see ends_with
/// @see strip_prefix
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto strip_suffix(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> suffix) -> basic_span<T1, P1, S1> {
    return ends_with(spn, suffix) ? snip(spn, suffix.size()) : spn;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a span contains the contents of @p what.
/// @ingroup memory
/// @see starts_with
/// @see ends_with
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto contains(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) noexcept -> S1 {
    for(S1 i = 0; i < spn.size(); ++i) {
        if(starts_with(skip(spn, i), what)) {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the first occurrence of @p what in a span.
/// @ingroup memory
/// @see find
/// @see find_element
/// @see find_element_if
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto find_position(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) noexcept -> std::optional<S1> {
    auto pos = S1(0);
    while(pos < spn.size()) {
        if(starts_with(skip(spn, pos), what)) {
            return {pos};
        }
        ++pos;
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Indicates if span contains the specified element.
/// @ingroup memory
/// @see find
/// @see find_element
/// @see find_element_if
/// @see reverse_find_position
export template <typename T, typename P, typename S, typename E>
constexpr auto has_element(const basic_span<T, P, S> spn, const E& what) noexcept
  -> bool {
    auto pos = S(0);
    while(pos < spn.size()) {
        if(are_equal(spn[pos], what)) {
            return true;
        }
        ++pos;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the first occurrence of @p what in a span.
/// @ingroup memory
/// @see has_element
/// @see find
/// @see find_position
/// @see find_element_if
/// @see reverse_find_position
export template <typename T, typename P, typename S, typename E>
constexpr auto find_element(
  const basic_span<T, P, S> spn,
  const E& what) noexcept -> std::optional<S> {
    auto pos = S(0);
    while(pos < spn.size()) {
        if(are_equal(spn[pos], what)) {
            return {pos};
        }
        ++pos;
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the first element satisfying @p predicate in a span.
/// @ingroup memory
/// @see find
/// @see find_position
/// @see find_element
/// @see reverse_find_position
export template <typename T, typename P, typename S, typename F>
constexpr auto find_element_if(
  const basic_span<T, P, S> spn,
  F predicate) noexcept -> std::optional<S> {
    auto pos = S(0);
    while(pos < spn.size()) {
        if(predicate(spn[pos])) {
            return {pos};
        }
        ++pos;
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns
/// @ingroup memory
/// @see skip_until
/// @pre spn.begin() <= pos and  pos <= spn.end()
export template <typename T, typename P, typename S, typename Pos>
constexpr auto skip_to(const basic_span<T, P, S> spn, Pos pos) noexcept
  -> basic_span<T, P, S>
    requires(std::is_convertible_v<Pos, decltype(spn.end())>)
{
    assert(spn.begin() <= pos and pos <= spn.end());
    return {pos, spn.end()};
}
//------------------------------------------------------------------------------
/// @brief Skips the elements from the front of a span until @p predicate is satisfied.
/// @ingroup memory
/// @see take_until
/// @see skip_to
export template <typename T, typename P, typename S, typename Predicate>
constexpr auto skip_until(
  const basic_span<T, P, S> spn,
  Predicate predicate) noexcept -> basic_span<T, P, S> {
    if(const auto found{find_element_if(spn, predicate)}) {
        using eagine::extract;
        return skip(spn, extract(found));
    }
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Takes the elements from the front of a span until @p predicate is satisfied.
/// @ingroup memory
/// @see skip_until
export template <typename T, typename P, typename S, typename Predicate>
constexpr auto take_until(
  const basic_span<T, P, S> spn,
  Predicate predicate) noexcept -> basic_span<T, P, S> {
    if(const auto found{find_element_if(spn, predicate)}) {
        using eagine::extract;
        return head(spn, extract(found));
    }
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the last occurrence of @p what in a span.
/// @ingroup memory
/// @see find_position
/// @see find_position_if
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
constexpr auto reverse_find_position(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) noexcept -> std::optional<S1> {
    auto pos = spn.size();
    while(pos > S1(0)) {
        --pos;
        if(starts_with(skip(spn, pos), what)) {
            return {pos};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the last occurrence of @p what in a span.
/// @ingroup memory
/// @see find_element
/// @see find_element_if
/// @see find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto find(const basic_span<T1, P1, S1> where, const basic_span<T2, P2, S2> what)
  -> basic_span<T1, P1, S1> {
    if(const auto pos{find_position(where, what)}) {
        using eagine::extract;
        return skip(where, extract(pos));
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span before the first occurrence of @p what.
/// @ingroup memory
/// @see slice_after
/// @see find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto slice_before(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return head(spn, extract_or(find_position(spn, what), spn.size()));
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span after the first occurrence of @p what.
/// @ingroup memory
/// @see slice_before
/// @see find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto slice_after(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return skip(
      spn, extract_or(find_position(spn, what), spn.size()) + what.size());
}
//------------------------------------------------------------------------------
/// @brief Splits a span by the first occurrence of @p what (before and after, what)
/// @ingroup memory
/// @see find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto split_by_first(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what)
  -> std::tuple<basic_span<T1, P1, S1>, basic_span<T1, P1, S1>> {
    const auto pos{extract_or(find_position(spn, what), spn.size())};
    return {head(spn, pos), skip(spn, pos + what.size())};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span before the last occurrence of @p what.
/// @ingroup memory
/// @see reverse_find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto slice_before_last(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return head(spn, extract_or(reverse_find_position(spn, what), spn.size()));
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span after the last occurrence of @p what.
/// @ingroup memory
/// @see reverse_find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto slice_after_last(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return skip(
      spn,
      extract_or(reverse_find_position(spn, what), spn.size()) + what.size());
}
//------------------------------------------------------------------------------
/// @brief Splits a span by the last occurrence of @p what (before and after, what)
/// @ingroup memory
/// @see reverse_find_position
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
auto split_by_last(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what)
  -> std::tuple<basic_span<T1, P1, S1>, basic_span<T1, P1, S1>> {
    const auto pos{extract_or(reverse_find_position(spn, what), spn.size())};
    return {head(spn, pos), skip(spn, pos + what.size())};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span within a pair of brackets.
/// @ingroup memory
/// @see find_element
export template <typename T, typename P, typename S, typename B>
auto slice_inside_brackets(
  basic_span<T, P, S> spn,
  const B left,
  const B right) noexcept -> basic_span<T, P, S> {

    if(const auto found{find_element(spn, left)}) {
        using eagine::extract;
        spn = skip(spn, extract(found));
        int depth = 1;
        auto pos = S(1);
        while((pos < spn.size()) and (depth > 0)) {
            if(are_equal(spn[pos], left)) {
                ++depth;
            } else if(are_equal(spn[pos], right)) {
                --depth;
            }
            ++pos;
        }
        return shrink(head(spn, pos), 1);
    }

    return {};
}
//------------------------------------------------------------------------------
/// @brief Copies the elements from one span to another compatible span.
/// @ingroup memory
/// @see fill
/// @see zero
/// @see generate
export template <
  typename TF,
  typename PF,
  typename SF,
  typename TT,
  typename PT,
  typename ST>
auto copy(const basic_span<TF, PF, SF> from, basic_span<TT, PT, ST> to)
  -> basic_span<TT, PT, ST> {
    assert(from.size() <= to.size());
    std::copy(from.begin(), from.end(), to.begin());
    return head(to, from.size());
}
//------------------------------------------------------------------------------
/// @brief Fills a span with copies of the specified value.
/// @ingroup memory
/// @see copy
/// @see zero
/// @see generate
export template <typename T, typename P, typename S, typename V>
auto fill(const basic_span<T, P, S> spn, const V& v) -> basic_span<T, P, S> {
    std::fill(spn.begin(), spn.end(), v);
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Fills a span with zero value of type T.
/// @ingroup memory
/// @see copy
/// @see fill
/// @see generate
export template <typename T, typename P, typename S>
auto zero(basic_span<T, P, S> spn) -> basic_span<T, P, S>
    requires(std::is_integral_v<T> or std::is_floating_point_v<T>)
{
    std::fill(spn.begin(), spn.end(), T(0));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Reverses the elements in a span.
/// @ingroup memory
/// @see transform
/// @see shuffle
/// @see sort
export template <typename T, typename P, typename S>
auto reverse(basic_span<T, P, S> spn) -> basic_span<T, P, S> {
    std::reverse(spn.begin(), spn.end());
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Transforms the elements of a span with a @p function.
/// @ingroup memory
/// @see reverse
/// @see shuffle
/// @see sort
export template <typename T, typename P, typename S, typename Transform>
auto transform(basic_span<T, P, S> spn, Transform function)
  -> basic_span<T, P, S> {
    std::transform(spn.begin(), spn.end(), spn.begin(), std::move(function));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Fills a span with elements generated by a generator callable.
/// @ingroup memory
/// @see copy
/// @see fill
/// @see zero
export template <typename T, typename P, typename S, typename Generator>
auto generate(basic_span<T, P, S> spn, Generator gen) -> basic_span<T, P, S> {
    std::generate(spn.begin(), spn.end(), std::move(gen));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Shuffles the elements of a span.
/// @ingroup memory
/// @see reverse
/// @see sort
/// @see is_sorted
export template <typename T, typename P, typename S, typename RandGen>
auto shuffle(basic_span<T, P, S> spn, RandGen rg) -> basic_span<T, P, S> {
    std::shuffle(spn.begin(), spn.end(), std::move(rg));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Sorts the elements of a span.
/// @ingroup memory
/// @see reverse
/// @see shuffle
/// @see is_sorted
/// @see make_index
export template <typename T, typename P, typename S>
auto sort(basic_span<T, P, S> spn) -> basic_span<T, P, S> {
    std::sort(spn.begin(), spn.end());
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Sorts the elements of a span according to @p compare.
/// @ingroup memory
/// @see reverse
/// @see shuffle
/// @see is_sorted
/// @see make_index
export template <typename T, typename P, typename S, typename Compare>
auto sort(basic_span<T, P, S> spn, Compare compare) -> basic_span<T, P, S> {
    std::sort(spn.begin(), spn.end(), std::move(compare));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Makes the index of a span according to @p compare, into another span
/// @ingroup memory
/// @see is_sorted
/// @see sort
/// @see shuffle
export template <
  typename T,
  typename P,
  typename S,
  typename I,
  typename PI,
  typename SI,
  typename Compare>
auto make_index(
  const basic_span<T, P, S> spn,
  basic_span<I, PI, SI> idx,
  Compare compare) -> bool {
    if(spn.size() == idx.size()) {
        std::sort(idx.begin(), idx.end(), [spn, &compare](auto& l, auto& r) {
            return compare(spn[l], spn[r]);
        });
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Makes the index of a span, into another span
/// @ingroup memory
/// @see is_sorted
/// @see sort
/// @see shuffle
export template <
  typename T,
  typename P,
  typename S,
  typename I,
  typename PI,
  typename SI>
auto make_index(const basic_span<T, P, S> spn, basic_span<I, PI, SI> idx)
  -> bool {
    return make_index(spn, idx, std::less<T>());
}
//------------------------------------------------------------------------------
/// @brief Tests if the elements in a span are sorted.
/// @ingroup memory
/// @see sort
/// @see shuffle
/// @see make_index
export template <typename T, typename P, typename S>
auto is_sorted(const basic_span<T, P, S> spn) -> bool {
    return std::is_sorted(spn.begin(), spn.end());
}
//------------------------------------------------------------------------------
/// @brief Tests if the elements in a span are sorted according to @p compare.
/// @ingroup memory
/// @see sort
/// @see shuffle
/// @see make_index
export template <typename T, typename P, typename S, typename Compare>
auto is_sorted(const basic_span<T, P, S> spn, Compare compare) -> bool {
    return std::is_sorted(spn.begin(), spn.end(), std::move(compare));
}
//------------------------------------------------------------------------------
/// @brief Indicates if all elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see any_of
/// @see none_of
export template <typename T, typename P, typename S, typename Predicate>
auto all_of(const basic_span<T, P, S> spn, Predicate predicate) -> bool {
    return std::all_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Indicates if any elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see all_of
/// @see none_of
export template <typename T, typename P, typename S, typename Predicate>
auto any_of(const basic_span<T, P, S> spn, Predicate predicate) -> bool {
    return std::any_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Indicates if no elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see all_of
/// @see any_of
export template <typename T, typename P, typename S, typename Predicate>
auto none_of(const basic_span<T, P, S> spn, Predicate predicate) -> bool {
    return std::none_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Scans span for parts split by delimiter, calls a function for each part.
/// @ingroup type_utils
/// @see for_each_chunk
export template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2,
  typename UnaryOperation>
void for_each_delimited(
  basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> delim,
  UnaryOperation unary_op) {
    basic_span<T1, P1, S1> tmp = spn;
    while(const auto pos{find_position(tmp, delim)}) {
        using eagine::extract;
        unary_op(head(tmp, extract(pos)));
        tmp = skip(tmp, extract(pos) + delim.size());
    }
    unary_op(tmp);
}
//------------------------------------------------------------------------------
/// @brief Splits span into parts of equal length, calls a function for each part.
/// @ingroup type_utils
/// @see for_each_delimited
export template <typename T, typename P, typename S, typename UnaryOperation>
void for_each_chunk(
  basic_span<T, P, S> spn,
  const span_size_t len,
  UnaryOperation unary_op) {
    span_size_t pos{0};
    while(pos < spn.size()) {
        unary_op(slice(spn, pos, len));
        pos += len;
    }
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
