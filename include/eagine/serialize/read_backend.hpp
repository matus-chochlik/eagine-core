/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_SERIALIZE_READ_BACKEND_HPP
#define EAGINE_SERIALIZE_READ_BACKEND_HPP

#include "../identifier.hpp"
#include "../interface.hpp"
#include "../reflect/decl_name.hpp"
#include "data_source.hpp"
#include "result.hpp"
#include <cstdint>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Interface for deserialization read backends.
/// @ingroup serialization
/// @see serializer_backend
/// @see deserializer_data_source
struct deserializer_backend : interface<deserializer_backend> {

    /// @brief Alias for the deserialization operations result type.
    using result = deserialization_errors;

    /// @brief Returns a descriptive identifier of the implementation.
    virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the associated data source, if any.
    virtual auto source() noexcept -> deserializer_data_source* = 0;

    /// @brie Indicates if the backed stores enumerators as strings (or numeric values).
    virtual auto enum_as_string() noexcept -> bool = 0;

    /// @brief Starts the deserialization of a potentially complex structured value.
    virtual auto begin() noexcept -> result = 0;

    /// @brief Reads boolean values, returns how many were done through second argument.
    virtual auto read(span<bool>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads character values, returns how many were done through second argument.
    virtual auto read(span<char>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads 8-bit int values, returns how many were done through second argument.
    virtual auto read(span<std::int8_t>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads short values, returns how many were done through second argument.
    virtual auto read(span<short>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads int values, returns how many were done through second argument.
    virtual auto read(span<int>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads long values, returns how many were done through second argument.
    virtual auto read(span<long>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads long long values, returns how many were done through second argument.
    virtual auto read(span<long long>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads 8-bit uint values, returns how many were done through second argument.
    virtual auto read(span<std::uint8_t>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads ushort values, returns how many were done through second argument.
    virtual auto read(span<unsigned short>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads uint values, returns how many were done through second argument.
    virtual auto read(span<unsigned>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads ulong values, returns how many were done through second argument.
    virtual auto read(span<unsigned long>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads ulong long values, returns how many were done through second argument.
    virtual auto read(span<unsigned long long>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads float values, returns how many were done through second argument.
    virtual auto read(span<float>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads double values, returns how many were done through second argument.
    virtual auto read(span<double>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads identifier values, returns how many were done through second argument.
    virtual auto read(span<identifier>, span_size_t&) noexcept -> result = 0;

    /// @brief Reads decl_name values, returns how many were done through second argument.
    virtual auto read(span<decl_name_storage>, span_size_t&) noexcept
      -> result = 0;

    /// @brief Reads string values, returns how many were done through second argument.
    virtual auto read(span<std::string>, span_size_t&) noexcept -> result = 0;

    /// @brief Begins the deserialization of a structure instance.
    virtual auto begin_struct(span_size_t& member_count) noexcept -> result = 0;

    /// @brief Begins the deserialization of a structure data member.
    virtual auto begin_member(const string_view name) noexcept -> result = 0;

    /// @brief Finishes the deserialization of a structure data member.
    virtual auto finish_member(const string_view name) noexcept -> result = 0;

    /// @brief Finishes the deserialization of a structure instance.
    virtual auto finish_struct() noexcept -> result = 0;

    /// @brief Begins the deserialization of a container instance.
    virtual auto begin_list(span_size_t& element_count) noexcept -> result = 0;

    /// @brief Begins the deserialization of a container element.
    virtual auto begin_element(const span_size_t index) noexcept -> result = 0;

    /// @brief Finishes the deserialization of a container element.
    virtual auto finish_element(const span_size_t index) noexcept -> result = 0;

    /// @brief Finishes the deserialization of a container instance.
    virtual auto finish_list() noexcept -> result = 0;

    /// @brief Finishes the deserialization of a potentially complex structured value.
    virtual auto finish() noexcept -> result = 0;
};
//------------------------------------------------------------------------------
/// @brief CRTP mixin implementing the common parts of deserializer backends.
/// @ingroup serialization
/// @tparam Derived the derived backend implementation
/// @tparam Source the data source type.
template <typename Derived, typename Source = deserializer_data_source>
class common_deserializer_backend : public deserializer_backend {
public:
    /// @brief Default constructor.
    common_deserializer_backend() noexcept = default;

    /// @brief Construction from a reference to a Source.
    common_deserializer_backend(Source& source) noexcept
      : _source{&source} {}

    /// @brief Sets a reference to a new Source object.
    auto set_source(Source& s) noexcept -> Derived& {
        _source = &s;
        return derived();
    }

    using error_code = deserialization_error_code;
    using result = deserialization_errors;

    auto source() noexcept -> deserializer_data_source* final {
        return _source;
    }

    auto enum_as_string() noexcept -> bool override {
        return false;
    }

    auto begin() noexcept -> result override {
        return {};
    }

    auto read(span<bool> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<char> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<std::int8_t> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<short> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<int> values, span_size_t& done) noexcept -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<long long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<std::uint8_t> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<unsigned short> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<unsigned> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<unsigned long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<unsigned long long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<float> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<double> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<identifier> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<decl_name_storage> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto read(span<std::string> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_read(values, done);
    }

    auto begin_struct(span_size_t&) noexcept -> result override {
        return {};
    }
    auto begin_member(const string_view) noexcept -> result override {
        return {};
    }
    auto finish_member(const string_view) noexcept -> result override {
        return {};
    }
    auto finish_struct() noexcept -> result override {
        return {};
    }
    auto begin_list(span_size_t&) noexcept -> result override {
        return {};
    }
    auto begin_element(const span_size_t) noexcept -> result override {
        return {};
    }
    auto finish_element(const span_size_t) noexcept -> result override {
        return {};
    }
    auto finish_list() noexcept -> result override {
        return {};
    }
    auto finish() noexcept -> result override {
        return {};
    }

protected:
    auto scan_for(
      const byte what,
      const span_size_t max,
      const span_size_t step = 256) noexcept
      -> valid_if_nonnegative<span_size_t> {
        EAGINE_ASSERT(_source);
        return _source->scan_for(what, max, step);
    }

    auto top(span_size_t size) noexcept -> memory::const_block {
        EAGINE_ASSERT(_source);
        return _source->top(size);
    }

    auto top_string(span_size_t size) noexcept {
        return as_chars(top(size));
    }

    auto string_before(
      const char c,
      const span_size_t max,
      const span_size_t step = 256) noexcept {
        auto found = scan_for(byte(c), max, step);
        return top_string(extract_or(found, 0));
    }

    auto top_char() noexcept {
        return top_string(1);
    }

    void pop(span_size_t size) noexcept {
        EAGINE_ASSERT(_source);
        _source->pop(size);
    }

    auto starts_with(string_view what) noexcept -> bool {
        return are_equal(top_string(what.size()), what);
    }

    auto starts_with(char c) noexcept -> bool {
        return starts_with(view_one(c));
    }

    auto consume(string_view what, result& errors) noexcept -> bool {
        errors = {};
        auto top_str = top_string(what.size());
        if(top_str.size() < what.size()) {
            errors |= error_code::not_enough_data;
        } else {
            if(are_equal(top_str, what)) {
                pop(what.size());
                return true;
            } else {
                errors |= error_code::unexpected_data;
            }
        }
        return false;
    }

    auto consume(char what, result& errors) noexcept -> bool {
        return consume(view_one(what), errors);
    }

    template <typename Function>
    void consume_until(
      Function predicate,
      const span_size_t step = 256) noexcept {
        while(const auto pos{_source->scan_until(predicate, step, step)}) {
            if(extract(pos) > 0) {
                pop(extract(pos));
            } else {
                break;
            }
        }
    }

    auto require(string_view what) noexcept -> result {
        result errors{};
        auto top_str = top_string(what.size());
        if(top_str.size() < what.size()) {
            errors |= error_code::not_enough_data;
        } else {
            if(are_equal(top_str, what)) {
                pop(what.size());
            } else {
                errors |= error_code::invalid_format;
            }
        }

        return errors;
    }

    auto require(char what) noexcept -> result {
        return require(view_one(what));
    }

private:
    Source* _source{nullptr};

    auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_SERIALIZE_READ_BACKEND_HPP
