/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:buffer;

import eagine.core.build_config;
import eagine.core.types;
export import :span;
import :address;
import :span_algorithm;
import :byte_allocator;
import std;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Reallocatable owning byte buffer.
/// @ingroup memory
/// @see block
export class buffer {
public:
    /// @brief Buffer size type.
    using size_type = typename block::size_type;

    /// @brief Pointer type.
    using pointer = typename block::pointer;

    /// @brief Default constructor.
    constexpr buffer() noexcept = default;

    /// @brief Constructor with explicit alignment specification.
    explicit buffer(const span_size_t align) noexcept
      : _align{align} {}

    /// @brief Constructor with injected allocator.
    explicit buffer(shared_byte_allocator alloc) noexcept
      : _alloc{std::move(alloc)} {}

    /// @brief Constructor with explicit alignment specification and allocator.
    buffer(const span_size_t align, shared_byte_allocator alloc) noexcept
      : _align{align}
      , _alloc{std::move(alloc)} {}

    /// @brief Constructor with explicit size, alignment and allocator.
    buffer(
      const span_size_t size,
      const span_size_t align,
      shared_byte_allocator alloc) noexcept
      : _storage{alloc.allocate(size, align)}
      , _size{size}
      , _align{align}
      , _alloc{std::move(alloc)} {}

    /// @brief Constructor with explicit size and alignment.
    buffer(const span_size_t size, const span_size_t align) noexcept
      : buffer{size, align, default_shared_allocator()} {}

    /// @brief Move constructor.
    buffer(buffer&& temp) noexcept
      : _storage{std::move(temp._storage)}
      , _size{std::exchange(temp._size, 0)}
      , _align{temp._align}
      , _alloc{std::move(temp._alloc)} {}

    /// @brief Move assignment operator.
    auto operator=(buffer&& temp) noexcept -> buffer& {
        _storage = std::move(temp._storage);
        _size = std::exchange(temp._size, 0);
        _align = temp._align;
        _alloc = std::move(temp._alloc);
        return *this;
    }

    /// @brief Not copyable.
    buffer(const buffer&) = delete;

    /// @brief Not copy assignable.
    auto operator=(const buffer&) = delete;

    ~buffer() noexcept {
        free();
    }

    /// @brief Returns the memory address of the start of the allocated space.
    auto addr() const noexcept {
        return _storage.addr();
    }

    /// @brief Returns the pointer to the start of the allocated space.
    auto data() const noexcept -> pointer {
        return _storage.data();
    }

    /// @brief Returns the size of the buffer in bytes.
    /// @see empty
    /// @see capacity
    constexpr auto size() const noexcept -> span_size_t {
        return _size;
    }

    /// @brief Indicates that the buffer is empty.
    /// @see size
    /// @see capacity
    constexpr auto empty() const noexcept {
        return size() == 0;
    }

    /// @brief Returns the capacity of this buffer.
    /// @see reserve
    /// @see resize
    constexpr auto capacity() const noexcept -> span_size_t {
        return _storage.size();
    }

    /// @brief Pre-allocate the specified number of bytes.
    /// @see capacity
    /// @see resize
    auto reserve(const span_size_t new_size) noexcept -> auto& {
        if(capacity() < new_size) {
            _reallocate(new_size);
        }
        assert(_is_ok());
        return *this;
    }

    /// @brief Resizes the buffer to the specified number of bytes.
    /// @see size
    /// @see ensure
    /// @see reserve
    auto resize(const span_size_t new_size) noexcept -> auto& {
        if(capacity() < new_size) [[unlikely]] {
            _reallocate(new_size);
        }
        _size = new_size;
        assert(_is_ok());
        return *this;
    }

    /// @brief Ensure that the buuffer has at least the specified size in bytes.
    /// @see size
    /// @see resize
    /// @see reserve
    /// @see enlarge_by
    auto ensure(const span_size_t new_size) noexcept -> auto& {
        if(size() < new_size) [[unlikely]] {
            return resize(new_size);
        }
        assert(_is_ok());
        return *this;
    }

    /// @brief Enlarges the buffer by the specified number of bytes.
    /// @see size
    /// @see resize
    /// @see reserve
    /// @see ensure
    auto enlarge_by(const span_size_t inc_size) noexcept -> auto& {
        return resize(safe_add(size(), inc_size));
    }

    /// @brief Clears the buffer.
    /// @see size
    /// @see resize
    /// @see reserve
    /// @see free
    /// @post empty()
    auto clear() noexcept -> auto& {
        return resize(0);
    }

    /// @brief Deallocates the buffer.
    /// @see size
    /// @see resize
    /// @see reserve
    /// @see clear
    /// @post empty() and  capacity() == 0
    void free() noexcept {
        _alloc.deallocate(std::move(_storage), _align);
        _size = 0;
    }

    /// @brief Implicit conversion to block.
    operator block() noexcept {
        assert(_is_ok());
        return {_storage.data(), _size};
    }

    /// @brief Implicit conversion to const_block.
    operator const_block() const noexcept {
        assert(_is_ok());
        return {_storage.data(), _size};
    }

private:
    owned_block _storage{};
    span_size_t _size{0};
    span_size_t _align{alignof(long double)};
    shared_byte_allocator _alloc{default_shared_allocator()};

    auto _is_ok() const noexcept -> bool {
        return bool(_alloc) and size() <= capacity();
    }

    void _reallocate(const span_size_t new_size) noexcept {
        _alloc.reallocate_inplace(_storage, new_size, _align);
    }
};
//------------------------------------------------------------------------------
/// @brief Copies the content of source block to destination block.
/// @ingroup memory
/// @see const_block
/// @see block
export auto copy(const const_block source, block dest) noexcept -> block {
    assert(dest.size() >= source.size());
    std::memcpy(dest.data(), source.data(), integer(source.size()));
    return {dest.data(), source.size()};
}

/// @brief Copies the content of source block to destination buffer.
/// @ingroup memory
/// @see append_to
/// @see const_block
/// @see buffer
export auto copy_into(const const_block source, buffer& dest) noexcept
  -> block {
    return copy(source, cover(dest.resize(source.size())));
}

/// @brief Appends the content of source block to destination buffer.
/// @ingroup memory
/// @see copy_into
/// @see const_block
/// @see buffer
export auto append_to(const const_block source, buffer& dest) noexcept
  -> block {
    const auto old_size{dest.size()};
    return copy(source, skip(cover(dest.enlarge_by(source.size())), old_size));
}
//------------------------------------------------------------------------------
// buffer pool
//------------------------------------------------------------------------------
export class buffer_pool;
export class buffer_pool_stats {
public:
    auto number_of_gets() const noexcept {
        return _gets;
    }

    auto number_of_hits() const noexcept {
        return _hits;
    }

    auto number_of_eats() const noexcept {
        return _eats;
    }

    auto number_of_discards() const noexcept {
        return _dscs;
    }

    auto max_buffer_count() const noexcept {
        return _maxc;
    }

    auto max_buffer_size() const noexcept {
        return _maxs;
    }

private:
    friend class buffer_pool;
    std::uintmax_t _hits{0};
    std::uintmax_t _gets{0};
    std::uintmax_t _eats{0};
    std::uintmax_t _dscs{0};
    std::size_t _maxc{0};
    std::size_t _maxs{0};
};
//------------------------------------------------------------------------------
/// @brief Class storing multiple reusable memory buffer instances.
/// @ingroup memory
/// @see buffer
export class buffer_pool {
public:
    /// @brief Default constructors.
    buffer_pool() noexcept = default;

    /// @brief Constructor initializing the maximum number of buffers in the pool.
    explicit buffer_pool(const std::size_t max) noexcept
      : _max{max} {}

    /// @brief Gets a buffer with the specified required size.
    /// @param req_size The returned buffer will have at least this number of bytes.
    /// @see eat
    auto get(const span_size_t req_size = 0) -> memory::buffer {
        memory::buffer result{};
        if constexpr(not low_profile_build) {
            _stats._maxc = std::max(_stats._maxc, _pool.size());
            ++_stats._gets;
        }
        if(not _pool.empty()) [[likely]] {
            if constexpr(not low_profile_build) {
                ++_stats._hits;
            }
            result = std::move(_pool.back());
            _pool.pop_back();
        }
        result.resize(req_size);
        return result;
    }

    /// @brief Returns the specified buffer back to the pool for further reuse.
    /// @see get
    void eat(memory::buffer used) noexcept {
        if(const auto old_cap{std_size(used.capacity())}; old_cap >= 16)
          [[likely]] {
            if(_pool.size() < _max) [[likely]] {
                try {
                    _pool.emplace_back(std::move(used));
                } catch(...) {
                }
            } else {
                if constexpr(not low_profile_build) {
                    ++_stats._dscs;
                }
            }
            if constexpr(not low_profile_build) {
                ++_stats._eats;
                _stats._maxs = std::max(_stats._maxs, old_cap);
            }
        } else {
            if constexpr(not low_profile_build) {
                ++_stats._dscs;
            }
        }
    }

    auto eat_later(memory::buffer& buf) noexcept {
        struct _cleanup {
            _cleanup(buffer_pool& pool, memory::buffer& buf) noexcept
              : _pool{pool}
              , _buf{buf} {}
            _cleanup(_cleanup&&) noexcept = default;
            _cleanup(const _cleanup&) = delete;
            auto operator=(_cleanup&&) = delete;
            auto operator=(const _cleanup&) = delete;
            ~_cleanup() noexcept {
                _pool.eat(std::move(_buf));
            }

            buffer_pool& _pool;
            memory::buffer& _buf;
        };
        return _cleanup{*this, buf};
    }

    auto stats() const noexcept -> optional_reference<const buffer_pool_stats> {
        if constexpr(not low_profile_build) {
            return {_stats};
        } else {
            return {nothing};
        }
    }

private:
    const std::size_t _max{1024};
    std::vector<memory::buffer> _pool;
    [[no_unique_address]] not_in_low_profile<buffer_pool_stats> _stats{};
};
//------------------------------------------------------------------------------
} // namespace eagine::memory
