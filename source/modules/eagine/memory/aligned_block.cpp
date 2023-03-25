/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:aligned_block;

import eagine.core.types;
export import :span;
export import :span_algorithm;
import std;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Class containing store of the specified size convertible to block.
/// @tparam Size the size in bytes of the internal storage in the instantiation.
/// @ingroup memory
/// @see block
export template <std::size_t Size>
class aligned_block {
public:
    /// @brief Returns a view of the internal storage as a block.
    auto get() noexcept -> block {
        return as_bytes(cover_one(_storage));
    }

    /// @brief Returns a view of the internal storage as a const block.
    auto get() const noexcept -> const_block {
        return as_bytes(view_one(_storage));
    }

    /// @brief Implicit conversion to block.
    /// @see get
    operator block() noexcept {
        return get();
    }

    /// @brief Implicit conversion to const block.
    /// @see get
    operator const_block() const noexcept {
        return get();
    }

private:
    std::aligned_storage_t<Size> _storage{};
};
//------------------------------------------------------------------------------
} // namespace eagine::memory
