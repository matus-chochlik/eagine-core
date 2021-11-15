/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_BUFFER_DATA_HPP
#define EAGINE_BUFFER_DATA_HPP

#include "memory/block.hpp"
#include "span.hpp"
#include "type_identity.hpp"
#include "types.hpp"
#include <type_traits>

namespace eagine {

template <typename S>
class buffer_size {
public:
    constexpr buffer_size() noexcept = default;

    explicit constexpr buffer_size(const S v) noexcept
      : _v{v} {}

    template <typename T>
    constexpr buffer_size(
      const type_identity<T>,
      const span_size_t count) noexcept
      : _v{S(span_size_of<T>() * count)} {}

    template <typename T, typename P, typename Z>
    constexpr buffer_size(const memory::basic_span<T, P, Z> s) noexcept
      : _v{S(span_size_of<T>() * span_size(s.size()))} {}

    constexpr auto get() const noexcept -> S {
        return _v;
    }

    constexpr operator S() const noexcept {
        return _v;
    }

    template <typename T>
    explicit constexpr operator T() const {
        return T(_v);
    }

    friend constexpr auto operator+(buffer_size a, buffer_size b) noexcept
      -> buffer_size {
        return {a._v + b._v};
    }

private:
    static_assert(std::is_integral_v<S>);
    S _v{0};
};

template <typename S>
class buffer_data_spec {
public:
    buffer_data_spec() noexcept = default;

    buffer_data_spec(const memory::block& blk) noexcept
      : _size{S(blk.size())}
      , _data{blk.data()} {}

    template <typename T, span_size_t N>
    buffer_data_spec(const T (&data)[N]) noexcept
      : _size{type_identity<T>(), N}
      , _data{static_cast<const void*>(data)} {}

    template <typename T>
    buffer_data_spec(const T* data, span_size_t n) noexcept
      : _size(type_identity<T>(), n)
      , _data(data) {}

    template <typename T>
    buffer_data_spec(span<T> av) noexcept
      : _size{av}
      , _data{av.data()} {}

    auto empty() const noexcept -> bool {
        return _size.get() == 0;
    }

    auto size() const noexcept -> buffer_size<S> {
        return _size;
    }

    auto data() const noexcept -> const void* {
        return _data;
    }

    auto view() const noexcept -> span<const byte> {
        return {
          static_cast<const byte*>(_data), static_cast<std::ptrdiff_t>(_size)};
    }

private:
    buffer_size<S> _size{};
    const void* _data{nullptr};
};

} // namespace eagine

#endif // EAGINE_BUFFER_DATA_HPP
