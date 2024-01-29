/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:block;

import std;
import eagine.core.types;
import eagine.core.memory;
import :result;
import :interface;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Serialization data sink backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see block_data_source
/// @see packed_block_data_sink
export class block_data_sink : public serializer_data_sink {
public:
    /// @brief Default constructor.
    /// @post free().empty()
    constexpr block_data_sink() noexcept = default;

    /// @brief Constructor setting the backing block.
    block_data_sink(const memory::block dst) noexcept
      : _dst{dst} {}

    /// @brief Resets the backing block.
    /// @see replace_with
    void reset(const memory::block dst) {
        _dst = dst;
        _done = 0;
        _save_points.clear();
    }

    /// @brief Returns the part of the backing block already written to.
    /// @see free
    auto done() const noexcept -> memory::block {
        return head(_dst, _done);
    }

    /// @brief Returns the free part of the backing block.
    /// @see done
    /// @see remaining_size
    auto free() const noexcept -> memory::block {
        return skip(_dst, _done);
    }

    /// @brief Returns the free part of the backing block and marks it as used.
    /// @see done
    /// @see remaining_size
    /// @see free
    auto mark_used(const span_size_t size) noexcept -> auto& {
        assert(size <= remaining_size());
        _done += size;
        return *this;
    }

    /// @brief Returns the size of the free part of the backing block.
    /// @see free
    /// @see done
    auto remaining_size() noexcept -> span_size_t final {
        return free().size();
    }

    using serializer_data_sink::write;

    auto write(memory::const_block blk) noexcept -> serialization_errors final {
        auto dst = free();
        if(dst.size() < blk.size()) {
            copy(head(blk, dst.size()), dst);
            _done += dst.size();
            return {serialization_error_code::incomplete_write};
        }
        copy(blk, dst);
        _done += blk.size();
        return {};
    }

    /// @brief Replaces the content of the backing block with the content of the argument.
    /// @see reset
    auto replace_with(const memory::const_block blk) noexcept
      -> serialization_errors {
        if(_dst.size() < blk.size()) {
            return {serialization_error_code::too_much_data};
        }
        copy(blk, _dst);
        _done = blk.size();
        return {};
    }

    auto begin_work() noexcept -> transaction_handle final {
        _save_points.push_back(_done);
        return transaction_handle(_save_points.size());
    }

    auto commit([[maybe_unused]] const transaction_handle th) noexcept
      -> serialization_errors final {
        assert(th == transaction_handle(_save_points.size()));
        _save_points.pop_back();
        return {};
    }

    void rollback([[maybe_unused]] const transaction_handle th) noexcept final {
        assert(th == transaction_handle(_save_points.size()));
        _done = _save_points.back();
        _save_points.pop_back();
    }

    auto finalize() noexcept -> serialization_errors override {
        return {};
    }

private:
    memory::block _dst{};
    span_size_t _done{0};
    std::vector<span_size_t> _save_points{};
};
//------------------------------------------------------------------------------
/// @brief Deserialization data source backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see block_data_sink
/// @see packed_block_data_source
export class block_data_source : public deserializer_data_source {
public:
    /// @brief Default constructor.
    block_data_source() noexcept = default;

    /// @brief Constructor setting the backing block.
    block_data_source(const memory::const_block src) noexcept
      : _src{src} {}

    /// @brief Resets the backing block.
    /// @see replace_with
    void reset(const memory::const_block src) noexcept {
        _src = src;
        _done = 0;
    }

    auto top(span_size_t req_size) noexcept -> memory::const_block final {
        return head(skip(_src, _done), req_size);
    }

    void pop(span_size_t del_size) noexcept final {
        _done += del_size;
    }

    auto remaining() const noexcept -> memory::const_block {
        return skip(_src, _done);
    }

private:
    memory::const_block _src{};
    span_size_t _done{0};
};
//------------------------------------------------------------------------------
} // namespace eagine

