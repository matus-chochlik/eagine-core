/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MEMORY_SPAN_ALGO_HPP
#define EAGINE_MEMORY_SPAN_ALGO_HPP

#include "span.hpp"
#include <algorithm>
#include <concepts>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

EAGINE_DIAG_PUSH()
#if defined(__clang__)
EAGINE_DIAG_OFF(unused-template)
#endif

namespace eagine::memory {
//------------------------------------------------------------------------------
template <typename T, typename P, typename S>
static constexpr auto clamp_span_iterator(
  const basic_span<T, P, S> s,
  P p) noexcept -> P {
    return (p < s.begin()) ? s.begin() : (p > s.end()) ? s.end() : p;
}
//------------------------------------------------------------------------------
template <typename T, typename P, typename S, std::integral I>
static constexpr auto clamp_span_position(
  const basic_span<T, P, S> s,
  const I p) noexcept -> P {
    return clamp_span_iterator(s, s.begin() + p);
}
//------------------------------------------------------------------------------
template <typename T, typename P, typename S, std::integral B, std::integral E>
static constexpr auto subspan(
  const basic_span<T, P, S> s,
  const B b,
  const E e) noexcept -> basic_span<T, P, S> {
    return {clamp_span_position(s, b), clamp_span_position(s, e)};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span starting at specified index with specified length.
/// @ingroup memory
/// @see head
/// @see tail
/// @see skip
/// @see snip
template <typename T, typename P, typename S, typename I, typename L>
static constexpr auto slice(
  const basic_span<T, P, S> s,
  const I i,
  const L l) noexcept -> basic_span<T, P, S> {
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
template <typename T, typename P, typename S, typename L>
static constexpr auto skip(const basic_span<T, P, S> s, const L l) noexcept
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
template <typename T, typename P, typename S, typename L>
static constexpr auto snip(const basic_span<T, P, S> s, const L l) noexcept
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
template <typename T, typename P, typename S, typename L>
static constexpr auto shrink(const basic_span<T, P, S> s, const L l) noexcept
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
template <typename T, typename P, typename S, typename L>
static constexpr auto head(const basic_span<T, P, S> s, const L l) noexcept
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
template <
  typename Ts,
  typename Ps,
  typename Ss,
  typename Tl,
  typename Pl,
  typename Sl>
static constexpr auto head(
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
template <typename T, typename P, typename S, typename L>
static constexpr auto tail(const basic_span<T, P, S> s, const L l) noexcept
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
template <
  typename Ts,
  typename Ps,
  typename Ss,
  typename Tl,
  typename Pl,
  typename Sl>
static constexpr auto tail(
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto starts_with(
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto ends_with(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> with) -> bool {
    return are_equal(tail(spn, with.size()), with);
}
//------------------------------------------------------------------------------
/// @brief Strips the specified @p prefix from a span.
/// @ingroup memory
/// @see starts_with
/// @see strip_suffix
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto strip_prefix(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> prefix) -> basic_span<T1, P1, S1> {
    return starts_with(spn, prefix) ? skip(spn, prefix.size()) : spn;
}
//------------------------------------------------------------------------------
/// @brief Strips the specified @p suffix from a span.
/// @ingroup memory
/// @see ends_with
/// @see strip_prefix
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto strip_suffix(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> suffix) -> basic_span<T1, P1, S1> {
    return ends_with(spn, suffix) ? snip(spn, suffix.size()) : spn;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a span contains the contents of @p what.
/// @ingroup memory
/// @see starts_with
/// @see ends_with
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto contains(
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto find_position(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) noexcept -> optionally_valid<S1> {
    auto pos = S1(0);
    while(pos < spn.size()) {
        if(starts_with(skip(spn, pos), what)) {
            return {pos, true};
        }
        ++pos;
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the first occurrence of @p what in a span.
/// @ingroup memory
/// @see find
/// @see find_position
/// @see find_element_if
/// @see reverse_find_position
template <typename T, typename P, typename S, typename E>
static constexpr auto find_element(
  const basic_span<T, P, S> spn,
  const E& what) noexcept -> optionally_valid<S> {
    auto pos = S(0);
    while(pos < spn.size()) {
        if(are_equal(spn[pos], what)) {
            return {pos, true};
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
template <typename T, typename P, typename S, typename F>
static constexpr auto find_element_if(
  const basic_span<T, P, S> spn,
  F predicate) noexcept -> optionally_valid<S> {
    auto pos = S(0);
    while(pos < spn.size()) {
        if(predicate(spn[pos])) {
            return {pos, true};
        }
        ++pos;
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns
/// @ingroup memory
/// @see skip_until
/// @pre spn.begin() <= pos && pos <= spn.end()
template <typename T, typename P, typename S, typename Pos>
static constexpr auto skip_to(const basic_span<T, P, S> spn, Pos pos) noexcept
  -> basic_span<T, P, S> requires(std::is_convertible_v<Pos, P>) {
    EAGINE_ASSERT(spn.begin() <= pos && pos <= spn.end());
    return {pos, spn.end()};
}
//------------------------------------------------------------------------------
/// @brief Skips the elements from the front of a span until @p predicate is satisfied.
/// @ingroup memory
/// @see take_until
/// @see skip_to
template <typename T, typename P, typename S, typename Predicate>
static constexpr auto skip_until(
  const basic_span<T, P, S> spn,
  Predicate predicate) noexcept -> basic_span<T, P, S> {
    if(const auto found{find_element_if(spn, predicate)}) {
        return skip(spn, extract(found));
    }
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Takes the elements from the front of a span until @p predicate is satisfied.
/// @ingroup memory
/// @see skip_until
template <typename T, typename P, typename S, typename Predicate>
static constexpr auto take_until(
  const basic_span<T, P, S> spn,
  Predicate predicate) noexcept -> basic_span<T, P, S> {
    if(const auto found{find_element_if(spn, predicate)}) {
        return head(spn, extract(found));
    }
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Finds the position of the last occurrence of @p what in a span.
/// @ingroup memory
/// @see find_position
/// @see find_position_if
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static constexpr auto reverse_find_position(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) noexcept -> optionally_valid<S1> {
    auto pos = spn.size();
    while(pos > S1(0)) {
        --pos;
        if(starts_with(skip(spn, pos), what)) {
            return {pos, true};
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto find(
  const basic_span<T1, P1, S1> where,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    if(const auto pos{find_position(where, what)}) {
        return skip(where, extract(pos));
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span before the first occurrence of @p what.
/// @ingroup memory
/// @see slice_after
/// @see find_position
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto slice_before(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return head(spn, extract_or(find_position(spn, what), spn.size()));
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span after the first occurrence of @p what.
/// @ingroup memory
/// @see slice_before
/// @see find_position
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto slice_after(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return skip(
      spn, extract_or(find_position(spn, what), spn.size()) + what.size());
}
//------------------------------------------------------------------------------
/// @brief Splits a span by the first occurrence of @p what (before and after, what)
/// @ingroup memory
/// @see find_position
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto split_by_first(
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto slice_before_last(
  const basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> what) -> basic_span<T1, P1, S1> {
    return head(spn, extract_or(reverse_find_position(spn, what), spn.size()));
}
//------------------------------------------------------------------------------
/// @brief Returns a slice of span after the last occurrence of @p what.
/// @ingroup memory
/// @see reverse_find_position
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto slice_after_last(
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
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2>
static inline auto split_by_last(
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
template <typename T, typename P, typename S, typename B>
static inline auto slice_inside_brackets(
  basic_span<T, P, S> spn,
  const B left,
  const B right) noexcept -> basic_span<T, P, S> {

    if(const auto found{find_element(spn, left)}) {
        spn = skip(spn, extract(found));
        int depth = 1;
        auto pos = S(1);
        while((pos < spn.size()) && (depth > 0)) {
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
template <
  typename TF,
  typename PF,
  typename SF,
  typename TT,
  typename PT,
  typename ST>
static inline auto copy(
  const basic_span<TF, PF, SF> from,
  basic_span<TT, PT, ST> to) -> basic_span<TT, PT, ST> {
    EAGINE_ASSERT(from.size() <= to.size());
    std::copy(from.begin(), from.end(), to.begin());
    return head(to, from.size());
}
//------------------------------------------------------------------------------
/// @brief Fills a span with copies of the specified value.
/// @ingroup memory
/// @see copy
/// @see zero
/// @see generate
template <typename T, typename P, typename S, typename V>
static inline auto fill(const basic_span<T, P, S> spn, const V& v)
  -> basic_span<T, P, S> {
    std::fill(spn.begin(), spn.end(), v);
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Fills a span with zero value of type T.
/// @ingroup memory
/// @see copy
/// @see fill
/// @see generate
template <typename T, typename P, typename S>
static inline auto zero(basic_span<T, P, S> spn)
  -> basic_span<T, P, S> requires(
    std::is_integral_v<T> || std::is_floating_point_v<T>) {
    std::fill(spn.begin(), spn.end(), T(0));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Reverses the elements in a span.
/// @ingroup memory
/// @see transform
/// @see shuffle
/// @see sort
template <typename T, typename P, typename S>
static inline auto reverse(basic_span<T, P, S> spn) -> basic_span<T, P, S> {
    std::reverse(spn.begin(), spn.end());
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Transforms the elements of a span with a @p function.
/// @ingroup memory
/// @see reverse
/// @see shuffle
/// @see sort
template <typename T, typename P, typename S, typename Transform>
static inline auto transform(basic_span<T, P, S> spn, Transform function)
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
template <typename T, typename P, typename S, typename Generator>
static inline auto generate(basic_span<T, P, S> spn, Generator gen)
  -> basic_span<T, P, S> {
    std::generate(spn.begin(), spn.end(), std::move(gen));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Shuffles the elements of a span.
/// @ingroup memory
/// @see reverse
/// @see sort
/// @see is_sorted
template <typename T, typename P, typename S, typename RandGen>
static inline auto shuffle(basic_span<T, P, S> spn, RandGen rg)
  -> basic_span<T, P, S> {
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
template <typename T, typename P, typename S>
static inline auto sort(basic_span<T, P, S> spn) -> basic_span<T, P, S> {
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
template <typename T, typename P, typename S, typename Compare>
static inline auto sort(basic_span<T, P, S> spn, Compare compare)
  -> basic_span<T, P, S> {
    std::sort(spn.begin(), spn.end(), std::move(compare));
    return spn;
}
//------------------------------------------------------------------------------
/// @brief Makes the index of a span according to @p compare, into another span
/// @ingroup memory
/// @see is_sorted
/// @see sort
/// @see shuffle
template <
  typename T,
  typename P,
  typename S,
  typename I,
  typename PI,
  typename SI,
  typename Compare>
static inline auto make_index(
  const basic_span<T, P, S> spn,
  basic_span<I, PI, SI> idx,
  Compare compare) -> bool {
    if(spn.size() == idx.size()) {
        std::sort(idx.begin(), idx.end(), [spn, &compare](auto& l, auto& r) {
            return compare(spn[l], spn[r]);
        });
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Makes the index of a span, into another span
/// @ingroup memory
/// @see is_sorted
/// @see sort
/// @see shuffle
template <typename T, typename P, typename S, typename I, typename PI, typename SI>
static inline auto make_index(
  const basic_span<T, P, S> spn,
  basic_span<I, PI, SI> idx) -> bool {
    return make_index(spn, idx, std::less<T>());
}
//------------------------------------------------------------------------------
/// @brief Tests if the elements in a span are sorted.
/// @ingroup memory
/// @see sort
/// @see shuffle
/// @see make_index
template <typename T, typename P, typename S>
static inline auto is_sorted(const basic_span<T, P, S> spn) -> bool {
    return std::is_sorted(spn.begin(), spn.end());
}
//------------------------------------------------------------------------------
/// @brief Tests if the elements in a span are sorted according to @p compare.
/// @ingroup memory
/// @see sort
/// @see shuffle
/// @see make_index
template <typename T, typename P, typename S, typename Compare>
static inline auto is_sorted(const basic_span<T, P, S> spn, Compare compare)
  -> bool {
    return std::is_sorted(spn.begin(), spn.end(), std::move(compare));
}
//------------------------------------------------------------------------------
/// @brief Indicates if all elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see any_of
/// @see none_of
template <typename T, typename P, typename S, typename Predicate>
static inline auto all_of(const basic_span<T, P, S> spn, Predicate predicate)
  -> bool {
    return std::all_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Indicates if any elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see all_of
/// @see none_of
template <typename T, typename P, typename S, typename Predicate>
static inline auto any_of(const basic_span<T, P, S> spn, Predicate predicate)
  -> bool {
    return std::any_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Indicates if no elements in a span satisfy @p predicate.
/// @ingroup type_utils
/// @see all_of
/// @see any_of
template <typename T, typename P, typename S, typename Predicate>
static inline auto none_of(const basic_span<T, P, S> spn, Predicate predicate)
  -> bool {
    return std::none_of(spn.begin(), spn.end(), std::move(predicate));
}
//------------------------------------------------------------------------------
/// @brief Scans span for parts split by delimiter, calls a function for each part.
/// @ingroup type_utils
/// @see for_each_chunk
template <
  typename T1,
  typename P1,
  typename S1,
  typename T2,
  typename P2,
  typename S2,
  typename UnaryOperation>
static inline void for_each_delimited(
  basic_span<T1, P1, S1> spn,
  const basic_span<T2, P2, S2> delim,
  UnaryOperation unary_op) {
    basic_span<T1, P1, S1> tmp = spn;
    while(auto pos = find_position(tmp, delim)) {
        unary_op(head(tmp, extract(pos)));
        tmp = skip(tmp, extract(pos) + delim.size());
    }
    unary_op(tmp);
}
//------------------------------------------------------------------------------
/// @brief Splits span into parts of equal length, calls a function for each part.
/// @ingroup type_utils
/// @see for_each_delimited
template <typename T, typename P, typename S, typename UnaryOperation>
static inline void for_each_chunk(
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
/// @brief Calculates the edit distance of @p s1 and @p s2 according to get_distance.
/// @ingroup type_utils
template <
  typename Result,
  typename T,
  typename P1,
  typename S1,
  typename P2,
  typename S2,
  typename BinaryFunction>
static auto basic_edit_distance(
  const basic_span<const T, P1, S1> s1,
  const basic_span<const T, P2, S2> s2,
  BinaryFunction get_distance) -> Result {
    const auto l1 = s1.size();
    const auto l2 = s2.size();

    std::vector<Result> column;
    column.resize(integer(l1 + 1));
    std::iota(column.begin(), column.end(), Result(0));

    for(span_size_t x = 1; x <= l2; x++) {
        column[0] = Result(x);
        auto prev_diagonal = Result(x) - 1;

        for(span_size_t y = 1; y <= l1; y++) {
            const auto curr_diagonal = column[y];
            const auto prev_min = std::min(column[y] + 1, column[y - 1] + 1);
            const auto new_dist =
              prev_diagonal + Result(get_distance(s1[y - 1], s2[x - 1]));
            column[y] = std::min(prev_min, new_dist);
            prev_diagonal = curr_diagonal;
        }
    }
    return column[l1];
}
//------------------------------------------------------------------------------
template <typename T, typename P1, typename S1, typename P2, typename S2>
static auto default_edit_distance(
  const basic_span<const T, P1, S1> s1,
  const basic_span<const T, P2, S2> s2) -> span_size_t {
    return basic_edit_distance<span_size_t>(
      s1, s2, [](auto e1, auto e2) { return e1 == e2 ? 0 : 1; });
}
//------------------------------------------------------------------------------
EAGINE_DIAG_POP()

} // namespace eagine::memory
#endif // EAGINE_MEMORY_SPAN_ALGO_HPP
