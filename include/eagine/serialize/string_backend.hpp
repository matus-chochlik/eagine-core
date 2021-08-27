/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_SERIALIZE_STRING_BACKEND_HPP
#define EAGINE_SERIALIZE_STRING_BACKEND_HPP

#include "read_backend.hpp"
#include "write_backend.hpp"
#include <cstdio>

#ifdef __clang__
EAGINE_DIAG_PUSH()
EAGINE_DIAG_OFF(format-nonliteral)
#endif

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Cross-platform implementation of serializer backend using ASCII-only strings.
/// @ingroup serialization
/// @see string_deserializer_backend
class string_serializer_backend
  : public serializer_backend_id<
      common_serializer_backend<string_serializer_backend>,
      EAGINE_ID_V(String)> {
    using base = serializer_backend_id<
      common_serializer_backend<string_serializer_backend>,
      EAGINE_ID_V(String)>;

public:
    using base::base;
    using base::remaining_size;
    using base::sink;
    using error_code = serialization_error_code;
    using result = serialization_errors;

    template <typename T>
    auto do_write(const span<const T> values, span_size_t& done) -> result {
        done = 0;
        result errors{};
        for(const auto& val : values) {
            errors |= _write_one(val, type_identity<T>{});
            errors |= do_sink(';');
            if(errors) {
                break;
            }
            ++done;
        }

        return errors;
    }

    auto enum_as_string() -> bool final {
        return true;
    }

    auto begin() -> result final {
        return do_sink('<');
    }

    auto begin_struct(const span_size_t count) -> result final {
        result errors = do_sink('{');
        errors |= _write_one(count, type_identity<span_size_t>{});
        errors |= do_sink('|');
        return errors;
    }

    auto begin_member(const string_view name) -> result final {
        result errors = do_sink(name);
        errors |= do_sink(':');
        return errors;
    }

    auto finish_member(const string_view) -> result final {
        return {};
    }

    auto finish_struct() -> result final {
        return do_sink("};");
    }

    auto begin_list(const span_size_t count) -> result final {
        result errors = do_sink('[');
        errors |= _write_one(count, type_identity<span_size_t>{});
        errors |= do_sink('|');
        return errors;
    }

    auto begin_element(const span_size_t) -> result final {
        return {};
    }

    auto finish_element(const span_size_t) -> result final {
        return {};
    }

    auto finish_list() -> result final {
        return do_sink("];");
    }

    auto finish() -> result final {
        return do_sink(">\0");
    }

private:
    auto _write_one(const bool value, const type_identity<bool>) -> result {
        if(value) {
            return do_sink("true");
        }
        return do_sink("false");
    }

    auto _write_one(const char value, const type_identity<char>) -> result {
        result errors = do_sink('\'');
        errors |= do_sink(value);
        errors |= do_sink('\'');
        return errors;
    }

    template <typename T, std::size_t L>
    auto _sprintf_one(const T value, const char (&fmt)[L]) -> result {
        std::array<char, 64> temp{};
        // TODO: to_chars from_chars when available
        // NOLINTNEXTLINE(hicpp-vararg)
        std::snprintf(
          temp.data(), temp.size(), static_cast<const char*>(fmt), value);
        return do_sink(string_view(temp.data()));
    }

    auto _write_one(const byte value, const type_identity<byte>) -> result {
        return _sprintf_one(value, "%02hhx");
    }

    auto _write_one(const signed char value, const type_identity<signed char>)
      -> result {
        return _sprintf_one(value, "%hhd");
    }

    auto _write_one(const short value, const type_identity<short>) -> result {
        return _sprintf_one(value, "%hd");
    }

    auto _write_one(
      const unsigned short value,
      const type_identity<unsigned short>) -> result {
        return _sprintf_one(value, "%hu");
    }

    auto _write_one(const int value, const type_identity<int>) -> result {
        return _sprintf_one(value, "%d");
    }

    auto _write_one(const unsigned value, const type_identity<unsigned>)
      -> result {
        return _sprintf_one(value, "%u");
    }

    auto _write_one(const long value, const type_identity<long>) -> result {
        return _sprintf_one(value, "%ld");
    }

    auto _write_one(
      const unsigned long value,
      const type_identity<unsigned long>) -> result {
        return _sprintf_one(value, "%lu");
    }

    auto _write_one(const long long value, const type_identity<long long>)
      -> result {
        return _sprintf_one(value, "%lld");
    }

    auto _write_one(
      const unsigned long long value,
      const type_identity<unsigned long long>) -> result {
        return _sprintf_one(value, "%llu");
    }

    auto _write_one(const float value, const type_identity<float>) -> result {
        return _sprintf_one(value, "%f");
    }

    auto _write_one(const double value, const type_identity<double>) -> result {
        return _sprintf_one(value, "%lf");
    }

    auto _write_one(const identifier id, const type_identity<identifier>)
      -> result {
        return do_sink(id.name().view());
    }

    auto _write_one(const decl_name name, const type_identity<decl_name>)
      -> result {
        return do_sink(name);
    }

    auto _write_one(const string_view str, const type_identity<string_view>)
      -> result {
        result errors = do_sink('"');
        errors |= _write_one(str.size(), type_identity<span_size_t>{});
        errors |= do_sink('|');
        errors |= do_sink(str);
        errors |= do_sink('"');
        return errors;
    }
};
//------------------------------------------------------------------------------
/// @brief Cross-platform implementation of deserializer backend using ASCII-only strings.
/// @ingroup serialization
/// @see string_serializer_backend
class string_deserializer_backend
  : public serializer_backend_id<
      common_deserializer_backend<string_deserializer_backend>,
      EAGINE_ID_V(String)> {
    using base = serializer_backend_id<
      common_deserializer_backend<string_deserializer_backend>,
      EAGINE_ID_V(String)>;

public:
    using base::base;
    using base::consume_until;
    using base::require;
    using error_code = deserialization_error_code;
    using result = deserialization_errors;

    template <typename T>
    auto do_read(span<T> values, span_size_t& done) -> result {
        done = 0;
        result errors{};
        for(T& val : values) { // NOLINT(hicpp-vararg)
            errors |= _read_one(val, ';');
            if(errors) {
                break;
            }
            ++done;
        }

        return errors;
    }

    auto enum_as_string() -> bool final {
        return true;
    }

    void skip_whitespaces() {
        consume_until([](byte b) { return !std::isspace(b); });
    }

    auto begin() -> result final {
        skip_whitespaces();
        return require('<');
    }

    auto begin_struct(span_size_t& count) -> result final {
        result errors = require('{');
        if(!errors) {
            errors |= _read_one(count, '|');
        }
        return errors;
    }

    auto begin_member(const string_view name) -> result final {
        result errors = require(name);
        if(!errors) {
            errors |= require(':');
        }
        return errors;
    }

    auto finish_member(const string_view) -> result final {
        return {};
    }

    auto finish_struct() -> result final {
        return require("};");
    }

    auto begin_list(span_size_t& count) -> result final {
        result errors = require('[');
        if(!errors) {
            errors |= _read_one(count, '|');
        }
        return errors;
    }

    auto begin_element(const span_size_t) -> result final {
        return {};
    }

    auto finish_element(const span_size_t) -> result final {
        return {};
    }

    auto finish_list() -> result final {
        return require("];");
    }

    auto finish() -> result final {
        return require(">\0");
    }

private:
    auto _read_one(bool& value, const char delimiter) -> result {
        result temp{};
        if(this->consume("true", temp)) {
            value = true;
        } else if(this->consume("false", temp)) {
            value = false;
        } else {
            return temp;
        }
        return require(delimiter);
    }

    auto _read_one(char& value, const char delimiter) -> result {
        result errors = require('\'');
        if(!errors) {
            if(auto opt_char = this->top_char()) {
                value = extract(opt_char);
                pop(1);
            } else {
                errors |= error_code::not_enough_data;
            }
            if(!errors) {
                const char t[3] = {'\'', delimiter, '\0'};
                errors |= require(string_view(t));
            }
        }
        return errors;
    }

    template <typename T, std::size_t L>
    auto _sscanf_one(T& value, const char delimiter, const char (&fmt)[L])
      -> result {
        result errors{};
        if(auto src = this->string_before(delimiter, 128)) {
            auto fmtstr = static_cast<const char*>(fmt);
            // TODO: to_chars from_chars when available
            // NOLINTNEXTLINE(hicpp-vararg)
            if(std::sscanf(src.data(), fmtstr, &value) == 1) {
                pop(src.size() + 1);
            } else {
                errors |= error_code::invalid_format;
            }
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(byte& value, const char delimiter) -> result {
        const char fmt[6] = {'%', 'h', 'h', 'x', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(signed char& value, const char delimiter) -> result {
        const char fmt[6] = {'%', 'h', 'h', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(short& value, const char delimiter) -> result {
        const char fmt[5] = {'%', 'h', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned short& value, char delimiter) -> result {
        const char fmt[5] = {'%', 'h', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(int& value, const char delimiter) -> result {
        const char fmt[4] = {'%', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned& value, const char delimiter) -> result {
        const char fmt[4] = {'%', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(long& value, const char delimiter) -> result {
        const char fmt[5] = {'%', 'l', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned long& value, const char delimiter) -> result {
        const char fmt[5] = {'%', 'l', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(long long& value, const char delimiter) -> result {
        const char fmt[6] = {'%', 'l', 'l', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned long long& value, const char delimiter) -> result {
        const char fmt[6] = {'%', 'l', 'l', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(float& value, const char delimiter) -> result {
        const char fmt[4] = {'%', 'f', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(double& value, const char delimiter) -> result {
        const char fmt[5] = {'%', 'l', 'f', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(identifier& value, const char delimiter) -> result {
        result errors{};
        if(auto src = this->string_before(delimiter, 32)) {
            value = identifier(src);
            pop(src.size() + 1);
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(decl_name_storage& value, const char delimiter) -> result {
        result errors{};
        const auto max = decl_name_storage::max_length + 1;
        if(auto src = this->string_before(delimiter, max)) {
            value.assign(src);
            pop(src.size() + 1);
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(std::string& value, const char delimiter) -> result {
        result errors = require('"');
        if(!errors) {
            span_size_t len{0};
            errors |= _read_one(len, '|');
            if(!errors) {
                auto str = this->top_string(len);
                if(str.size() < len) {
                    errors |= error_code::not_enough_data;
                } else {
                    assign_to(value, str);
                    pop(str.size());
                    errors |= require('"');
                    errors |= require(delimiter);
                }
            }
        }
        return errors;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

#ifdef __clang__
EAGINE_DIAG_POP()
#endif

#endif // EAGINE_SERIALIZE_STRING_BACKEND_HPP
