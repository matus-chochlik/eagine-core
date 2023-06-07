/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:interface;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.valid_if;
import :result;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Abstract base class for deserialization data sources.
/// @ingroup serialization
/// @see data_sink
/// @see deserializer_backend
export struct deserializer_data_source : interface<deserializer_data_source> {

    /// @brief Returns a block covering the specified amount of data of the top.
    /// @see pop
    /// @see scan_until
    /// @see scan_for
    [[nodiscard]] virtual auto top(span_size_t size) noexcept
      -> memory::const_block = 0;

    /// @brief Returns the specified amount of data of the top of the source.
    /// @see top
    virtual void pop(span_size_t size) noexcept = 0;

    /// @brief Returns the position of the first byte where predicate is true.
    /// @see scan_for
    /// @param predicate the function indicating where to stop the scan.
    /// @param max the maximum number of bytes from the top to scan.
    /// @param step how much data should be fetched per scan iteration.
    template <typename Function>
    [[nodiscard]] auto scan_until(
      Function predicate,
      const span_size_t max,
      const span_size_t step = 256) noexcept
      -> valid_if_nonnegative<span_size_t> {
        assert(max > 0);
        assert(step > 0);
        const auto inc{step};
        span_size_t start{0};
        span_size_t total{inc};
        while(const auto blk = top(total)) {
            if(const auto found{find_element_if(skip(blk, start), predicate)}) {
                return {start + *found};
            }
            if(blk.size() < total) {
                break;
            }
            if(max < total) {
                return max;
            }
            start += inc;
            total += inc;
        }
        return {-1};
    }

    /// @brief Returns the position of the first occurrence of the specified byte.
    /// @see scan_until
    /// @param what the searched value.
    /// @param max the maximum number of bytes from the top to scan.
    /// @param step how much data should be fetched per scan iteration.
    [[nodiscard]] auto scan_for(
      const byte what,
      const span_size_t max,
      const span_size_t step = 256) noexcept
      -> valid_if_nonnegative<span_size_t> {
        assert(max > 0);
        assert(step > 0);
        return scan_until(
          [what](const byte b) { return b == what; }, max, step);
    }

    /// @brief Fetches all the remaining data into a buffer.
    void fetch_all(memory::buffer& dst, const span_size_t step = 256) noexcept {
        assert(step > 0);
        span_size_t offs{dst.size()};
        while(const auto blk{top(step)}) {
            dst.enlarge_by(blk.size());
            copy(blk, skip(cover(dst), offs));
            offs += blk.size();
            pop(blk.size());
        }
    }
};
//------------------------------------------------------------------------------
/// @brief Abstract base class for serialization data sinks.
/// @ingroup serialization
/// @see data_source
/// @see serializer_backend
export struct serializer_data_sink : abstract<serializer_data_sink> {

    /// @brief Alias for the operation result type.
    using result = serialization_errors;

    /// @brief Returns the remaining available size for data in this sink.
    [[nodiscard]] virtual auto remaining_size() noexcept -> span_size_t = 0;

    /// @brief Writes a block of data into this sink.
    [[nodiscard]] virtual auto write(memory::const_block data) noexcept
      -> result = 0;

    /// @brief Writes a single string character into this sink.
    [[nodiscard]] auto write(const char chr) noexcept -> result {
        return this->write(as_bytes(view_one(chr)));
    }

    /// @brief Writes a string view into this sink.
    [[nodiscard]] auto write(const string_view str) noexcept -> result {
        return this->write(as_bytes(str));
    }

    /// @brief Writes as much as possible from a split data block.
    [[nodiscard]] auto write_some(memory::const_split_block data) noexcept
      -> serialization_result<memory::const_split_block> {
        const auto before{remaining_size()};
        const auto errors{write(data.tail())};
        if(not errors) {
            return {data.skip_to_end()};
        }
        if(errors.has_at_most(serialization_error_code::incomplete_write)) {
            return {data.advance(before)};
        }
        return {data, errors};
    }

    /// @brief Alias for type indentifying sink transaction that can be rolled back.
    using transaction_handle = std::uintptr_t;

    /// @brief Begins a write transaction on this data sink.
    /// @see commit
    /// @see rollback
    [[nodiscard]] virtual auto begin_work() noexcept -> transaction_handle = 0;

    /// @brief Commits writes done as a part of transaction identified by argument.
    /// @see begin_work
    /// @see rollback
    [[nodiscard]] virtual auto commit(const transaction_handle) noexcept
      -> result = 0;

    /// @brief Rolls-back writes done as a part of transaction identified by argument.
    /// @see begin_work
    /// @see commit
    virtual void rollback(const transaction_handle) noexcept = 0;

    /// @brief Does additional finalization, like compression after serialization operation.
    [[nodiscard]] virtual auto finalize() noexcept -> result = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for deserialization read backends.
/// @ingroup serialization
/// @see serializer_backend
/// @see deserializer_data_source
export struct deserializer_backend : interface<deserializer_backend> {

    /// @brief Alias for the deserialization operations result type.
    using result = deserialization_errors;

    /// @brief Returns a descriptive identifier of the implementation.
    [[nodiscard]] virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the associated data source, if any.
    [[nodiscard]] virtual auto source() noexcept
      -> deserializer_data_source* = 0;

    /// @brie Indicates if the backed stores enumerators as strings (or numeric values).
    [[nodiscard]] virtual auto enum_as_string() noexcept -> bool = 0;

    /// @brief Starts the deserialization of a potentially complex structured value.
    [[nodiscard]] virtual auto begin() noexcept -> result = 0;

    /// @brief Reads boolean values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<bool>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads character values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<char>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads 8-bit int values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<std::int8_t>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads short values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<short>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads int values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<int>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<long>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads long long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<long long>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads 8-bit uint values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<std::uint8_t>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads ushort values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<unsigned short>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads uint values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<unsigned>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads ulong values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<unsigned long>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads ulong long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(
      span<unsigned long long>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Reads float values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<float>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads double values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<double>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads identifier values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<identifier>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads decl_name values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(
      span<decl_name_storage>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Reads string values, returns how many were done through second argument.
    [[nodiscard]] virtual auto read(span<std::string>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Begins the deserialization of a structure instance.
    [[nodiscard]] virtual auto begin_struct(span_size_t& member_count) noexcept
      -> result = 0;

    /// @brief Begins the deserialization of a structure data member.
    [[nodiscard]] virtual auto begin_member(const string_view name) noexcept
      -> result = 0;

    /// @brief Finishes the deserialization of a structure data member.
    [[nodiscard]] virtual auto finish_member(const string_view name) noexcept
      -> result = 0;

    /// @brief Finishes the deserialization of a structure instance.
    [[nodiscard]] virtual auto finish_struct() noexcept -> result = 0;

    /// @brief Begins the deserialization of a container instance.
    [[nodiscard]] virtual auto begin_list(span_size_t& element_count) noexcept
      -> result = 0;

    /// @brief Begins the deserialization of a container element.
    [[nodiscard]] virtual auto begin_element(const span_size_t index) noexcept
      -> result = 0;

    /// @brief Finishes the deserialization of a container element.
    [[nodiscard]] virtual auto finish_element(const span_size_t index) noexcept
      -> result = 0;

    /// @brief Finishes the deserialization of a container instance.
    [[nodiscard]] virtual auto finish_list() noexcept -> result = 0;

    /// @brief Finishes the deserialization of a potentially complex structured value.
    [[nodiscard]] virtual auto finish() noexcept -> result = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for serialization write backends.
/// @ingroup serialization
/// @see common_serializer_backend
/// @see deserializer_backend
/// @see serializer_data_sink
export struct serializer_backend : interface<serializer_backend> {

    /// @brief Alias for the serialization operations result type.
    using result = serialization_errors;

    /// @brief Returns a descriptive identifier of the implementation.
    [[nodiscard]] virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the associated data sink, if any.
    [[nodiscard]] virtual auto sink() noexcept -> serializer_data_sink& = 0;

    /// @brie Indicates if the backed stores enumerators as strings (or numeric values).
    [[nodiscard]] virtual auto enum_as_string() noexcept -> bool = 0;

    /// @brief Starts the serialization of a potentially complex structured value.
    [[nodiscard]] virtual auto begin() noexcept -> result = 0;

    /// @brief Writes boolean values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const bool>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes character values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const char>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes 8-bit int values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const std::int8_t>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes short int values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const short>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes int values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const int>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const long>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes long long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const long long>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes 8-bit uint values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const std::uint8_t>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes ushort values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const unsigned short>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes uint values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const unsigned>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes ulong values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const unsigned long>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes ulong long values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const unsigned long long>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes float values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const float>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes double values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(span<const double>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Writes identifier values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const identifier>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes decl_name values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const decl_name>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Writes string values, returns how many were done through second argument.
    [[nodiscard]] virtual auto write(
      span<const string_view>,
      span_size_t&) noexcept -> result = 0;

    /// @brief Begins the serialization of a structure instance.
    [[nodiscard]] virtual auto begin_struct(
      const span_size_t member_count) noexcept -> result = 0;

    /// @brief Begins the serialization of a structure data member.
    [[nodiscard]] virtual auto begin_member(const string_view name) noexcept
      -> result = 0;

    /// @brief Finishes the serialization of a structure data member.
    [[nodiscard]] virtual auto finish_member(const string_view name) noexcept
      -> result = 0;

    /// @brief Finishes the serialization of a structure instance.
    [[nodiscard]] virtual auto finish_struct() noexcept -> result = 0;

    /// @brief Begins the serialization of a container instance.
    [[nodiscard]] virtual auto begin_list(
      const span_size_t element_count) noexcept -> result = 0;

    /// @brief Begins the serialization of a container element.
    [[nodiscard]] virtual auto begin_element(const span_size_t index) noexcept
      -> result = 0;

    /// @brief Finishes the serialization of a container element.
    [[nodiscard]] virtual auto finish_element(const span_size_t index) noexcept
      -> result = 0;

    /// @brief Finishes the serialization of a container instance.
    [[nodiscard]] virtual auto finish_list() noexcept -> result = 0;

    /// @brief Finishes the serialization of a potentially complex structured value.
    [[nodiscard]] virtual auto finish() noexcept -> result = 0;
};
//------------------------------------------------------------------------------
} // namespace eagine
