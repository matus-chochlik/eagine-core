/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_BUFFER_DATA_HPP
#define EAGINE_BUFFER_DATA_HPP

#include "buffer_size.hpp"
#include "memory/block.hpp"
#include "types.hpp"

namespace eagine {

template <typename S>
class buffer_data_spec {
private:
    buffer_size<S> _size{};
    const void* _data{nullptr};

public:
    buffer_data_spec() noexcept = default;

    buffer_data_spec(const memory::block& blk) noexcept
      : _size(S(blk.size()))
      , _data(blk.data()) {}

    template <typename T, span_size_t N>
    buffer_data_spec(const T (&data)[N]) noexcept
      : _size(type_identity<T>(), N)
      , _data(static_cast<const void*>(data)) {}

    template <typename T>
    buffer_data_spec(const T* data, span_size_t n) noexcept
      : _size(type_identity<T>(), n)
      , _data(data) {}

    template <typename T>
    buffer_data_spec(span<T> av) noexcept
      : _size(av)
      , _data(av.data()) {}

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
};

} // namespace eagine

#endif // EAGINE_BUFFER_DATA_HPP
