/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:implementation;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.valid_if;
import :result;
import :interface;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief CRTP mixin implementing the common parts of deserializer backends.
/// @ingroup serialization
/// @tparam Derived the derived backend implementation
/// @tparam Source the data source type.
export template <typename Derived, typename Source = deserializer_data_source>
class common_deserializer_backend : public deserializer_backend {
public:
    /// @brief Default constructor.
    [[nodiscard]] common_deserializer_backend() noexcept = default;

    /// @brief Construction from a reference to a Source.
    [[nodiscard]] common_deserializer_backend(Source& source) noexcept
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
        assert(_source);
        return _source->scan_for(what, max, step);
    }

    auto top(span_size_t size) noexcept -> memory::const_block {
        assert(_source);
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
        return top_string(found.value_or(0));
    }

    auto top_char() noexcept {
        return top_string(1);
    }

    void pop(span_size_t size) noexcept {
        assert(_source);
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
/// @brief CRTP mixin implementing the common parts of serializer backends.
/// @ingroup serialization
/// @tparam Derived the derived backend implementation
/// @tparam Sink the data sink type.
template <typename Derived, typename Sink = serializer_data_sink>
class common_serializer_backend : public serializer_backend {
public:
    /// @brief Default constructor.
    [[nodiscard]] common_serializer_backend() noexcept = default;

    /// @brief Construction from a reference to a Sink.
    [[nodiscard]] common_serializer_backend(Sink& s) noexcept
      : _sink{&s} {}

    /// @brief Sets a reference to a new Sink object.
    auto set_sink(Sink& s) noexcept -> auto& {
        _sink = &s;
        return derived();
    }

    auto sink() noexcept -> Sink* final {
        return _sink;
    }

    auto enum_as_string() noexcept -> bool override {
        return false;
    }

    auto begin() noexcept -> result override {
        return {};
    }

    auto write(span<const bool> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const char> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const std::int8_t> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const short> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const int> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const long long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const std::uint8_t> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const unsigned short> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const unsigned> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const unsigned long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const unsigned long long> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const float> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const double> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const identifier> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const decl_name> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto write(span<const string_view> values, span_size_t& done) noexcept
      -> result override {
        return derived().do_write(values, done);
    }

    auto begin_struct(const span_size_t) noexcept -> result override {
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
    auto begin_list(const span_size_t) noexcept -> result override {
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
    auto remaining_size() const noexcept -> span_size_t {
        assert(_sink);
        return _sink->remaining_size();
    }

    auto do_sink(const memory::const_block b) const noexcept -> result {
        assert(_sink);
        return _sink->write(b);
    }

    auto do_sink(const string_view s) const noexcept -> result {
        assert(_sink);
        return _sink->write(s);
    }

    auto do_sink(const char c) const noexcept -> result {
        assert(_sink);
        return _sink->write(c);
    }

private:
    Sink* _sink{nullptr};

    auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
