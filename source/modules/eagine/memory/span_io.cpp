/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:span_io;

import eagine.core.types;
import :span;
import <iostream>;
import <optional>;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Helper function for pretty-printing spans as lists into output streams.
/// @ingroup memory
/// @see write_to_stream
/// @relates basic_span
export template <typename T, typename P, typename S, typename Output>
auto list_to_stream(Output& out, const basic_span<T, P, S> s) -> Output& {
    out << '[';
    bool first = true;
    for(const auto& e : s) {
        if(first) [[unlikely]] {
            first = false;
        } else {
            out << ',';
        }
        out << e;
    }
    return out << ']';
}
//------------------------------------------------------------------------------
/// @brief Helper function for raw-reading spans from output streams.
/// @ingroup memory
/// @see write_to_stream
/// @relates basic_span
export template <typename Input, typename T, typename P, typename S>
auto read_from_stream(Input& in, basic_span<T, P, S> s) -> auto& {
    return in.read(
      reinterpret_cast<char*>(s.data()), limit_cast<std::streamsize>(s.size()));
}
//------------------------------------------------------------------------------
/// @brief Helper function for raw-reading spans into output streams.
/// @ingroup memory
/// @see read_from_stream
/// @see list_to_stream
/// @relates basic_span
export template <typename Output, typename T, typename P, typename S>
auto write_to_stream(Output& out, const basic_span<T, P, S> s) -> auto& {
    return out.write(
      reinterpret_cast<const char*>(s.data()),
      limit_cast<std::streamsize>(s.size()));
}
//------------------------------------------------------------------------------
/// @brief Operator for printing generic element spans into output streams.
/// @ingroup memory
/// @see list_to_stream
/// @relates basic_span
export template <typename T, typename P, typename S>
    requires(not std::is_same_v<std::remove_const_t<T>, char>)
auto operator<<(std::ostream& out, const basic_span<T, P, S> s)
  -> std::ostream& {
    return list_to_stream(out, s);
}
//------------------------------------------------------------------------------
/// @brief Operator for printing spans of string characters into output streams.
/// @ingroup memory
/// @see write_to_stream
/// @relates basic_span
export template <typename T, typename P, typename S>
    requires(std::is_same_v<std::remove_const_t<T>, char>)
auto operator<<(std::ostream& out, const basic_span<T, P, S> s)
  -> std::ostream& {
    return write_to_stream(out, absolute(s));
}
//------------------------------------------------------------------------------
/// @brief Makes a callable that returns consecutive span elements starting at i.
/// @ingroup memory
/// @see make_span_putter
/// @relates basic_span
///
/// The constructed callable object does not take any arguments in the call
/// operator and returns optional values of T.
export template <typename T, typename P, typename S>
auto make_span_getter(span_size_t& i, const basic_span<T, P, S> spn) noexcept {
    return [&i, spn]() -> std::optional<std::remove_const_t<T>> {
        if(i < spn.size()) {
            return {spn[i++]};
        }
        return {};
    };
}
//------------------------------------------------------------------------------
export template <typename Src>
auto make_span_getter(span_size_t& i, const Src& src) noexcept {
    return make_span_getter(i, view(src));
}
//------------------------------------------------------------------------------
/// @brief Makes a callable getting consecutive, transformed span elements starting at i.
/// @ingroup memory
/// @see make_span_putter
/// @relates basic_span
///
/// The constructed callable object does not take any arguments in the call
/// operator and returns the result of the transform function.
/// The transform function takes a single optional value of T.
export template <typename T, typename P, typename S, typename Transform>
auto make_span_getter(
  span_size_t& i,
  const basic_span<T, P, S> spn,
  const Transform transform) noexcept {
    return [&i, spn, transform]() -> decltype(transform(std::declval<T>())) {
        if(i < spn.size()) {
            return transform(spn[i++]);
        }
        return {};
    };
}
//------------------------------------------------------------------------------
export template <typename Src, typename Transform>
auto make_span_getter(
  span_size_t& i,
  const Src& src,
  Transform transform) noexcept {
    return make_span_getter(i, view(src), std::move(transform));
}
//------------------------------------------------------------------------------
/// @brief Makes a callable setting consecutive elements of a span starting at i.
/// @ingroup memory
/// @see make_span_getter
/// @relates basic_span
///
/// The constructed callable takes a single value explicitly convertible to T.
export template <typename T, typename P, typename S>
auto make_span_putter(span_size_t& i, basic_span<T, P, S> spn) noexcept {
    return [&i, spn](auto value) mutable -> bool {
        if(i < spn.size()) {
            spn[i++] = T(std::move(value));
            return true;
        }
        return false;
    };
}
//------------------------------------------------------------------------------
export template <typename Dst>
auto make_span_putter(span_size_t& o, Dst& dst) noexcept {
    return make_span_putter(o, cover(dst));
}
//------------------------------------------------------------------------------
/// @brief Makes a callable setting consecutive elements of a span starting at i.
/// @ingroup memory
/// @param transform transformation operation to be applied
/// @see make_span_getter
/// @relates basic_span
/// The constructed callable takes a single value explicitly convertible to the
/// argument of the transform function.
export template <typename T, typename P, typename S, typename Transform>
auto make_span_putter(
  span_size_t& i,
  basic_span<T, P, S> spn,
  Transform transform) noexcept {
    return [&i, spn, transform](auto value) mutable -> bool {
        if(i < spn.size()) {
            if(auto transformed{transform(value)}) {
                spn[i++] = T(std::move(extract(transformed)));
                return true;
            }
        }
        return false;
    };
}
//------------------------------------------------------------------------------
export template <typename Dst, typename Transform>
auto make_span_putter(span_size_t& o, Dst& dst, Transform transform) noexcept {
    return make_span_putter(o, cover(dst), std::move(transform));
}
//------------------------------------------------------------------------------
} // namespace eagine::memory

