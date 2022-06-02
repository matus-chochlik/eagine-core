/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_SERIALIZE_PORTABLE_BACKEND_HPP
#define EAGINE_SERIALIZE_PORTABLE_BACKEND_HPP

#include "fputils.hpp"
#include "read_backend.hpp"
#include "write_backend.hpp"
#include <concepts>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Cross-platform implementation serializer backend.
/// @ingroup serialization
/// @see portable_deserializer_backend
class portable_serializer_backend
  : public serializer_backend_id<
      common_serializer_backend<portable_serializer_backend>,
      EAGINE_ID_V(Portable)> {
    using base = serializer_backend_id<
      common_serializer_backend<portable_serializer_backend>,
      EAGINE_ID_V(Portable)>;

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
            if(errors) [[unlikely]] {
                break;
            }
            ++done;
        }

        return errors;
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
        return do_sink("}");
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
        return do_sink("]");
    }

    auto finish() noexcept -> result final {
        return do_sink(">\0");
    }

private:
    auto _write_one(const bool value, const std::type_identity<bool>) noexcept
      -> result {
        if(value) {
            return do_sink("T");
        }
        return do_sink("U");
    }

    template <std::unsigned_integral I>
    auto _write_one(I value, const std::type_identity<I>) noexcept -> result {
        result errors{};
        const auto encode = [&]() {
            // clang-format off
			const char c[16] = {
				'0','1','2','3','4','5','6','7',
				'8','9','A','B','C','D','E','F'};
            // clang-format on
            errors |= do_sink(c[(value & 0xFU)]);
            value >>= 4U;
        };
        encode();
        while(!errors && value) {
            encode();
        }
        return errors;
    }

    template <std::signed_integral I>
    auto _write_one(I value, const std::type_identity<I>) noexcept -> result {
        result errors = do_sink(value < 0 ? '-' : '+');
        using U = std::make_unsigned_t<I>;
        errors |= _write_one(
          limit_cast<U>(value < 0 ? -value : value), std::type_identity<U>{});
        return errors;
    }

    template <typename F>
    auto _write_one(const F value, const std::type_identity<F> tid) noexcept
      -> result
      requires(std::is_floating_point_v<F>) {
          const auto [f, e] = fputils::decompose(value, tid);
          result errors = _write_one(
            f, std::type_identity<fputils::decompose_fraction_t<F>>{});
          errors |= do_sink('`');
          errors |= _write_one(
            e, std::type_identity<fputils::decompose_exponent_t<F>>{});

          return errors;
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
/// @brief Cross-platform implementation of deserializer backend.
/// @ingroup serialization
/// @see portable_serializer_backend
class portable_deserializer_backend
  : public serializer_backend_id<
      common_deserializer_backend<portable_deserializer_backend>,
      EAGINE_ID_V(Portable)> {
    using base = serializer_backend_id<
      common_deserializer_backend<portable_deserializer_backend>,
      EAGINE_ID_V(Portable)>;

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
            if(errors) [[unlikely]] {
                break;
            }
            ++done;
        }

        return errors;
    }

    auto enum_as_string() noexcept -> bool final {
        return true;
    }

    void skip_whitespaces() noexcept {
        consume_until([](byte b) { return !std::isspace(b); });
    }

    auto begin() noexcept -> result final {
        skip_whitespaces();
        return require('<');
    }

    auto begin_struct(span_size_t& count) noexcept -> result final {
        result errors = require('{');
        if(!errors) [[likely]] {
            errors |= _read_one(count, '|');
        }
        return errors;
    }

    auto begin_member(const string_view name) noexcept -> result final {
        result errors = require(name);
        if(!errors) [[likely]] {
            errors |= require(':');
        }
        return errors;
    }

    auto finish_member(const string_view) noexcept -> result final {
        return {};
    }

    auto finish_struct() noexcept -> result final {
        return require("}");
    }

    auto begin_list(span_size_t& count) noexcept -> result final {
        result errors = require('[');
        if(!errors) [[likely]] {
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
        return require("]");
    }

    auto finish() noexcept -> result final {
        return require(">\0");
    }

private:
    auto _read_one(bool& value, const char delimiter) noexcept -> result {
        result temp{};
        if(this->consume('T', temp)) {
            value = true;
        } else if(this->consume('U', temp)) {
            value = false;
        } else {
            return temp;
        }
        return require(delimiter);
    }

    template <std::unsigned_integral I>
    auto _read_one(I& value, const char delimiter) noexcept -> result {
        value = I(0);
        result errors{};
        auto src{this->string_before(delimiter, 48)};
        if(src) [[likely]] {
            const auto skip_len = safe_add(src.size(), 1);
            unsigned shift = 0U;
            while(src) {
                const char c = extract(src);
                I frag{};
                if((c >= '0') && (c <= '9')) {
                    frag = I(c - '0');
                } else if((c >= 'A') && (c <= 'F')) {
                    frag = I(10 + c - 'A');
                } else {
                    errors |= deserialization_error_code::invalid_format;
                    return errors;
                }
                EAGINE_ASSERT(frag < 16U);
                value |= I(frag << shift);
                shift += 4U;
                src = skip(src, 1);
            }
            pop(skip_len);
        } else {
            errors |= error_code::not_enough_data;
        }
        return errors;
    }

    template <std::signed_integral I>
    auto _read_one(I& value, const char delimiter) noexcept -> result {
        using U = std::make_unsigned_t<I>;
        const char sign = extract_or(top_char(), '\0');
        result errors{};
        if((sign == '+') || (sign == '-')) [[likely]] {
            pop(1);
            U temp{};
            errors |= _read_one(temp, delimiter);
            const auto conv{convert_if_fits<I>(temp)};
            if(conv) [[likely]] {
                value = (sign == '-') ? -extract(conv) : extract(conv);
            } else {
                errors |= deserialization_error_code::invalid_format;
            }
        } else {
            errors |= deserialization_error_code::invalid_format;
        }
        return errors;
    }

    template <std::floating_point F>
    auto _read_one(F& value, const char delimiter) noexcept -> result {
        fputils::decompose_fraction_t<F> f{};

        result errors = _read_one(f, '`');
        if(!errors) [[likely]] {
            fputils::decompose_exponent_t<F> e{};
            errors |= _read_one(e, delimiter);
            if(!errors) [[likely]] {
                value = fputils::compose({f, e}, std::type_identity<F>{});
            }
        }
        return errors;
    }

    auto _read_one(identifier& value, const char delimiter) noexcept -> result {
        result errors{};
        const auto src{this->string_before(delimiter, 32)};
        if(src) [[likely]] {
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
        const auto max = decl_name_storage::max_length + 1;
        const auto src{this->string_before(delimiter, max)};
        if(src) [[likely]] {
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
        if(!errors) {
            span_size_t len{0};
            errors |= _read_one(len, '|');
            if(!errors) {
                auto str{this->top_string(len)};
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

#endif // EAGINE_SERIALIZE_PORTABLE_BACKEND_HPP
