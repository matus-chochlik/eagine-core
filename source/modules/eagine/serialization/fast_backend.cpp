/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:fast_backend;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;
import :result;
import :interface;
import :implementation;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export class fast_serializer_backend
  : public common_serializer_backend<fast_serializer_backend> {
    using base = common_serializer_backend<fast_serializer_backend>;

public:
    using base::base;
    using base::remaining_size;
    using base::sink;
    using error_code = serialization_error_code;
    using result = serialization_errors;

    static constexpr const identifier_value id_value{"FastLocal"};

    auto type_id() noexcept -> identifier final {
        return "FastLocal";
    }

    template <typename T>
    auto do_write(span<const T> values, span_size_t& done) noexcept -> result {
        result errors{};
        const span_size_t remaining = remaining_size();
        span_size_t can_do = remaining / span_size(sizeof(T));
        if(can_do < values.size()) {
            values = head(values, can_do);
            errors = serialization_error_code::incomplete_write;
        } else {
            can_do = values.size();
        }
        errors |= do_sink(as_bytes(values));
        done = can_do;
        return errors;
    }

    template <typename Str>
    auto do_write_strings(
      const span<const Str> values,
      span_size_t& done) noexcept -> result {
        done = 0;
        serialization_errors errors{};
        for(const auto& str : values) {
            span_size_t written{0};
            errors |= do_write(view_one(str.size()), written);
            if(errors) {
                return errors;
            }
            do_sink(str);
            done += written;
        }
        return errors;
    }

    auto do_write(const span<const decl_name> values, span_size_t& done) {
        return do_write_strings(values, done);
    }

    auto do_write(const span<const string_view> values, span_size_t& done) {
        return do_write_strings(values, done);
    }

    auto begin_struct(const span_size_t size) noexcept -> result final {
        span_size_t written{0};
        return do_write(view_one(size), written);
    }

    auto begin_list(const span_size_t size) noexcept -> result final {
        span_size_t written{0};
        return do_write(view_one(size), written);
    }
};
//------------------------------------------------------------------------------
export class fast_deserializer_backend
  : public common_deserializer_backend<fast_deserializer_backend> {
    using base = common_deserializer_backend<fast_deserializer_backend>;

public:
    using base::base;
    using base::pop;
    using base::top;
    using error_code = deserialization_error_code;
    using result = deserialization_errors;

    static constexpr const identifier_value id_value{"FastLocal"};

    auto type_id() noexcept -> identifier final {
        return "FastLocal";
    }

    template <typename T>
    auto do_read(span<T> values, span_size_t& done) noexcept -> result {
        auto dst = as_bytes(values);
        auto src = top(dst.size());
        const auto ts = span_size(sizeof(T));
        if(src.size() < ts) {
            return {error_code::not_enough_data};
        }
        result errors{};
        if(src.size() < dst.size()) {
            done = src.size() / ts;
            src = head(src, done * ts);
            errors = error_code::incomplete_read;
        } else {
            done = values.size();
        }
        memory::copy(src, dst);
        pop(src.size());
        return errors;
    }

    auto do_read(span<decl_name_storage> values, span_size_t& done) noexcept
      -> result {
        result errors{};
        done = 0;
        for(auto& name : values) {
            span_size_t unused{0};
            span_size_t size{0};
            errors |= do_read(cover_one(size), unused);
            if(errors) {
                break;
            }
            auto src = as_chars(top(size));
            if(src.size() < size) {
                return {error_code::not_enough_data};
            }
            name.assign(src);
            pop(src.size());
            ++done;
        }
        return errors;
    }

    auto do_read(span<std::string> values, span_size_t& done) noexcept
      -> result {
        result errors{};
        done = 0;
        for(auto& str : values) {
            span_size_t unused{0};
            span_size_t size{0};
            errors |= do_read(cover_one(size), unused);
            if(errors) {
                break;
            }
            auto src = as_chars(top(size));
            if(src.size() < size) {
                return {error_code::not_enough_data};
            }
            assign_to(src, str);
            pop(src.size());
            ++done;
        }
        return errors;
    }

    auto begin_struct(span_size_t& size) noexcept -> result final {
        span_size_t done{0};
        return do_read(cover_one(size), done);
    }

    auto begin_list(span_size_t& size) noexcept -> result final {
        span_size_t done{0};
        return do_read(cover_one(size), done);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
