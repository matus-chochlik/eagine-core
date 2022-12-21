/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:string_backend;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.valid_if;
import :result;
import :interface;
import :implementation;
import <cstdio>;
import <cctype>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Cross-platform implementation of serializer backend using ASCII-only strings.
/// @ingroup serialization
/// @see string_deserializer_backend
export class string_serializer_backend
  : public common_serializer_backend<string_serializer_backend> {
    using base = common_serializer_backend<string_serializer_backend>;

public:
    using base::base;
    using base::remaining_size;
    using base::sink;
    using error_code = serialization_error_code;
    using result = serialization_errors;

    template <typename T>
    auto do_write(const span<const T> values, span_size_t& done) noexcept
      -> result {
        done = 0;
        result errors{};
        for(const auto& val : values) {
            errors |= _write_one(val, std::type_identity<T>{});
            errors |= do_sink(';');
            if(errors) {
                break;
            }
            ++done;
        }

        return errors;
    }

    static constexpr const identifier_value id_value{"String"};

    auto type_id() noexcept -> identifier final {
        return "String";
    }

    auto enum_as_string() noexcept -> bool final {
        return true;
    }

    auto begin() noexcept -> result final {
        return do_sink('<');
    }

    auto begin_struct(const span_size_t count) noexcept -> result final {
        result errors = do_sink('{');
        errors |= _write_one(count, std::type_identity<span_size_t>{});
        errors |= do_sink('|');
        return errors;
    }

    auto begin_member(const string_view name) noexcept -> result final {
        result errors = do_sink(name);
        errors |= do_sink(':');
        return errors;
    }

    auto finish_member(const string_view) noexcept -> result final {
        return {};
    }

    auto finish_struct() noexcept -> result final {
        return do_sink("};");
    }

    auto begin_list(const span_size_t count) noexcept -> result final {
        result errors = do_sink('[');
        errors |= _write_one(count, std::type_identity<span_size_t>{});
        errors |= do_sink('|');
        return errors;
    }

    auto begin_element(const span_size_t) noexcept -> result final {
        return {};
    }

    auto finish_element(const span_size_t) noexcept -> result final {
        return {};
    }

    auto finish_list() noexcept -> result final {
        return do_sink("];");
    }

    auto finish() noexcept -> result final {
        return do_sink(">\0");
    }

private:
    auto _write_one(const bool value, const std::type_identity<bool>) noexcept
      -> result {
        if(value) {
            return do_sink("true");
        }
        return do_sink("false");
    }

    auto _write_one(const char value, const std::type_identity<char>) noexcept
      -> result {
        result errors = do_sink('\'');
        errors |= do_sink(value);
        errors |= do_sink('\'');
        return errors;
    }

    template <typename T, std::size_t L>
    auto _sprintf_one(const T value, const char (&fmt)[L]) noexcept -> result {
        std::array<char, 64> temp{};
        // TODO: to_chars from_chars when available
        // NOLINTNEXTLINE(hicpp-vararg)
        [[maybe_unused]] const auto unused{std::snprintf(
          temp.data(), temp.size(), static_cast<const char*>(fmt), value)};
        return do_sink(string_view(temp.data()));
    }

    auto _write_one(const byte value, const std::type_identity<byte>) noexcept
      -> result {
        return _sprintf_one(value, "%02hhx");
    }

    auto _write_one(
      const signed char value,
      const std::type_identity<signed char>) noexcept -> result {
        return _sprintf_one(value, "%hhd");
    }

    auto _write_one(const short value, const std::type_identity<short>) noexcept
      -> result {
        return _sprintf_one(value, "%hd");
    }

    auto _write_one(
      const unsigned short value,
      const std::type_identity<unsigned short>) noexcept -> result {
        return _sprintf_one(value, "%hu");
    }

    auto _write_one(const int value, const std::type_identity<int>) noexcept
      -> result {
        return _sprintf_one(value, "%d");
    }

    auto _write_one(
      const unsigned value,
      const std::type_identity<unsigned>) noexcept -> result {
        return _sprintf_one(value, "%u");
    }

    auto _write_one(const long value, const std::type_identity<long>) noexcept
      -> result {
        return _sprintf_one(value, "%ld");
    }

    auto _write_one(
      const unsigned long value,
      const std::type_identity<unsigned long>) noexcept -> result {
        return _sprintf_one(value, "%lu");
    }

    auto _write_one(
      const long long value,
      const std::type_identity<long long>) noexcept -> result {
        return _sprintf_one(value, "%lld");
    }

    auto _write_one(
      const unsigned long long value,
      const std::type_identity<unsigned long long>) noexcept -> result {
        return _sprintf_one(value, "%llu");
    }

    auto _write_one(const float value, const std::type_identity<float>) noexcept
      -> result {
        return _sprintf_one(value, "%f");
    }

    auto _write_one(
      const double value,
      const std::type_identity<double>) noexcept -> result {
        return _sprintf_one(value, "%lf");
    }

    auto _write_one(
      const identifier id,
      const std::type_identity<identifier>) noexcept -> result {
        return do_sink(id.name().view());
    }

    auto _write_one(
      const decl_name name,
      const std::type_identity<decl_name>) noexcept -> result {
        return do_sink(name);
    }

    auto _write_one(
      const string_view str,
      const std::type_identity<string_view>) noexcept -> result {
        result errors = do_sink('"');
        errors |= _write_one(str.size(), std::type_identity<span_size_t>{});
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
export class string_deserializer_backend
  : public common_deserializer_backend<string_deserializer_backend> {
    using base = common_deserializer_backend<string_deserializer_backend>;

public:
    using base::base;
    using base::consume_until;
    using base::require;
    using error_code = deserialization_error_code;
    using result = deserialization_errors;

    template <typename T>
    auto do_read(span<T> values, span_size_t& done) noexcept -> result {
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

    static constexpr const identifier_value id_value{"String"};

    auto type_id() noexcept -> identifier final {
        return "String";
    }

    auto enum_as_string() noexcept -> bool final {
        return true;
    }

    void skip_whitespaces() noexcept {
        consume_until([](byte b) { return not std::isspace(b); });
    }

    auto begin() noexcept -> result final {
        skip_whitespaces();
        return require('<');
    }

    auto begin_struct(span_size_t& count) noexcept -> result final {
        result errors = require('{');
        if(not errors) {
            errors |= _read_one(count, '|');
        }
        return errors;
    }

    auto begin_member(const string_view name) noexcept -> result final {
        result errors = require(name);
        if(not errors) {
            errors |= require(':');
        }
        return errors;
    }

    auto finish_member(const string_view) noexcept -> result final {
        return {};
    }

    auto finish_struct() noexcept -> result final {
        return require("};");
    }

    auto begin_list(span_size_t& count) noexcept -> result final {
        result errors = require('[');
        if(not errors) {
            errors |= _read_one(count, '|');
        }
        return errors;
    }

    auto begin_element(const span_size_t) noexcept -> result final {
        return {};
    }

    auto finish_element(const span_size_t) noexcept -> result final {
        return {};
    }

    auto finish_list() noexcept -> result final {
        return require("];");
    }

    auto finish() noexcept -> result final {
        return require(">\0");
    }

private:
    auto _read_one(bool& value, const char delimiter) noexcept -> result {
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

    auto _read_one(char& value, const char delimiter) noexcept -> result {
        result errors = require('\'');
        if(not errors) {
            if(const auto opt_char{this->top_char()}) {
                value = extract(opt_char);
                pop(1);
            } else {
                errors |= error_code::not_enough_data;
            }
            if(not errors) {
                const char t[3] = {'\'', delimiter, '\0'};
                errors |= require(string_view(t));
            }
        }
        return errors;
    }

    template <typename T, std::size_t L>
    auto _sscanf_one(
      T& value,
      const char delimiter,
      const char (&fmt)[L]) noexcept -> result {
        result errors{};
        if(const auto src{this->string_before(delimiter, 128)}) {
            auto fmtstr = static_cast<const char*>(fmt);
            // TODO: to_chars from_chars when available
            // NOLINTNEXTLINE(hicpp-vararg)
            if(std::sscanf(src.data(), fmtstr, &value) == 1) {
                pop(safe_add(src.size(), 1));
            } else {
                errors |= error_code::invalid_format;
            }
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(byte& value, const char delimiter) noexcept -> result {
        const char fmt[6] = {'%', 'h', 'h', 'x', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(signed char& value, const char delimiter) noexcept
      -> result {
        const char fmt[6] = {'%', 'h', 'h', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(short& value, const char delimiter) noexcept -> result {
        const char fmt[5] = {'%', 'h', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned short& value, char delimiter) noexcept -> result {
        const char fmt[5] = {'%', 'h', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(int& value, const char delimiter) noexcept -> result {
        const char fmt[4] = {'%', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned& value, const char delimiter) noexcept -> result {
        const char fmt[4] = {'%', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(long& value, const char delimiter) noexcept -> result {
        const char fmt[5] = {'%', 'l', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned long& value, const char delimiter) noexcept
      -> result {
        const char fmt[5] = {'%', 'l', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(long long& value, const char delimiter) noexcept -> result {
        const char fmt[6] = {'%', 'l', 'l', 'd', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(unsigned long long& value, const char delimiter) noexcept
      -> result {
        const char fmt[6] = {'%', 'l', 'l', 'u', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(float& value, const char delimiter) noexcept -> result {
        const char fmt[4] = {'%', 'f', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(double& value, const char delimiter) noexcept -> result {
        const char fmt[5] = {'%', 'l', 'f', delimiter, '\0'};
        return _sscanf_one(value, delimiter, fmt);
    }

    auto _read_one(identifier& value, const char delimiter) noexcept -> result {
        result errors{};
        if(const auto src{this->string_before(delimiter, 32)}) {
            value = identifier(src);
            pop(safe_add(src.size(), 1));
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(decl_name_storage& value, const char delimiter) noexcept
      -> result {
        result errors{};
        const auto max = safe_add(decl_name_storage::max_length, 1);
        if(const auto src{this->string_before(delimiter, max)}) {
            value.assign(src);
            pop(safe_add(src.size(), 1));
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    auto _read_one(std::string& value, const char delimiter) noexcept
      -> result {
        result errors = require('"');
        if(not errors) {
            span_size_t len{0};
            errors |= _read_one(len, '|');
            if(not errors) {
                auto str = this->top_string(len);
                if(str.size() < len) {
                    errors |= error_code::not_enough_data;
                } else {
                    assign_to(str, value);
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

