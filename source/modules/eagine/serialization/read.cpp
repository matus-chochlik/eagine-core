/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:read;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.math;
import eagine.core.valid_if;
import eagine.core.units;
import :result;
import :interface;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
class fragment_deserialize_wrapper;

export template <typename T>
class fragment_deserialize_wrapper<span<T>> {
public:
    constexpr fragment_deserialize_wrapper() noexcept = default;

    constexpr fragment_deserialize_wrapper(span<T> dst) noexcept {
        set_target(dst);
    }

    void set_target(span<T> dst) noexcept {
        _dst = dst;
        _done_bits.resize(integer(dst.size()), false);
        _done_size = 0;
    }

    auto slice(const span_size_t offset, const span_size_t size) const noexcept {
        return memory::slice(_dst, offset, size);
    }

    void mark_done(const span_size_t offs, const span_size_t size) noexcept {
        auto i = math::minimum(_done_bits.begin() + offs, _done_bits.end());
        auto e = math::minimum(i + size, _done_bits.end());
        while(i != e) {
            if(not *i) {
                *i = true;
                ++_done_size;
            }
            ++i;
        }
    }

    auto is_done() const noexcept -> bool {
        return _done_size >= _dst.size();
    }

private:
    span<T> _dst{};
    std::vector<bool> _done_bits{};
    span_size_t _done_size{0};
};
//------------------------------------------------------------------------------
export template <typename T>
struct deserializer;

export template <typename T>
struct deserializer<T&> : deserializer<T> {};
//------------------------------------------------------------------------------
export template <typename T>
struct plain_deserializer {

    static auto read(T& value, auto& backend) noexcept
      -> deserialization_errors {
        span_size_t done{0};
        return backend.read(cover_one(value), done);
    }

    static auto read(span<T> values, auto& backend) noexcept
      -> deserialization_errors {
        span_size_t done{0};
        return backend.read(values, done);
    }
};
//------------------------------------------------------------------------------
export template <>
struct deserializer<bool> : plain_deserializer<bool> {};
export template <>
struct deserializer<char> : plain_deserializer<char> {};
export template <>
struct deserializer<std::int8_t> : plain_deserializer<std::int8_t> {};
export template <>
struct deserializer<short> : plain_deserializer<short> {};
export template <>
struct deserializer<int> : plain_deserializer<int> {};
export template <>
struct deserializer<long> : plain_deserializer<long> {};
export template <>
struct deserializer<long long> : plain_deserializer<long long> {};
export template <>
struct deserializer<std::uint8_t> : plain_deserializer<std::uint8_t> {};
export template <>
struct deserializer<unsigned short> : plain_deserializer<unsigned short> {};
export template <>
struct deserializer<unsigned> : plain_deserializer<unsigned> {};
export template <>
struct deserializer<unsigned long> : plain_deserializer<unsigned long> {};
export template <>
struct deserializer<unsigned long long>
  : plain_deserializer<unsigned long long> {};
export template <>
struct deserializer<float> : plain_deserializer<float> {};
export template <>
struct deserializer<double> : plain_deserializer<double> {};
export template <>
struct deserializer<identifier> : plain_deserializer<identifier> {};
export template <>
struct deserializer<decl_name_storage>
  : plain_deserializer<decl_name_storage> {};
export template <>
struct deserializer<std::string> : plain_deserializer<std::string> {};
//------------------------------------------------------------------------------
export template <identifier_t Tag>
struct deserializer<tagged_id<Tag>> {
    static auto read(tagged_id<Tag>& value, auto& backend) noexcept
      -> deserialization_errors {
        span_size_t done{0};
        identifier_t temp{0};
        auto result{backend.read(cover_one(temp), done)};
        value = {temp};
        return result;
    }
};
//------------------------------------------------------------------------------
export template <typename T>
struct structural_deserializer {
    auto read(T& value, auto& backend) {
        auto temp{T::make_structure()};
        const auto errors{_deserializer.read(temp, backend)};
        if(not errors) [[likely]] {
            value.set_structure(std::move(temp));
        }
        return errors;
    }

private:
    deserializer<typename T::structure_type> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct common_deserializer {

    auto read(span<T> values, auto& backend) const noexcept {
        using ec = deserialization_error_code;
        deserialization_errors result{};
        for(auto& elem : values) {
            result |=
              static_cast<const deserializer<T>*>(this)->read(elem, backend);
            if(result.has(ec::not_enough_data) or result.has(ec::backend_error)) {
                break;
            }
        }
        return result;
    }
};
//------------------------------------------------------------------------------
export template <typename Bit>
struct deserializer<bitfield<Bit>> : common_deserializer<bitfield<Bit>> {

    auto read(bitfield<Bit>& value, auto& backend) const noexcept {
        typename bitfield<Bit>::value_type temp{0};
        auto errors{_deserializer.read(temp, backend)};
        if(not errors) [[likely]] {
            value = bitfield<Bit>{temp};
        }
        return errors;
    }

private:
    deserializer<typename bitfield<Bit>::value_type> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename Rep>
struct deserializer<std::chrono::duration<Rep>>
  : common_deserializer<std::chrono::duration<Rep>> {

    auto read(std::chrono::duration<Rep>& value, auto& backend) const noexcept {
        Rep temp{0};
        auto errors{_deserializer.read(temp, backend)};
        if(not errors) [[likely]] {
            value = std::chrono::duration<Rep>{temp};
        }
        return errors;
    }

private:
    deserializer<Rep> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename... T>
struct deserializer<std::tuple<T...>> : common_deserializer<std::tuple<T...>> {

    using common_deserializer<std::tuple<T...>>::read;

    auto read(std::tuple<T...>& values, auto& backend) noexcept {
        deserialization_errors errors{};
        span_size_t elem_count{0};
        errors |= backend.begin_list(elem_count);
        if(elem_count < span_size(sizeof...(T))) [[unlikely]] {
            errors |= deserialization_error_code::missing_element;
        } else if(elem_count > span_size(sizeof...(T))) [[unlikely]] {
            errors |= deserialization_error_code::excess_element;
        }
        if(errors.has_at_most(deserialization_error_code::excess_element))
          [[likely]] {
            _read_elements(
              errors, values, backend, std::make_index_sequence<sizeof...(T)>());
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    template <typename Tuple, typename Backend, std::size_t... I>
    void _read_elements(
      deserialization_errors& errors,
      Tuple& values,
      Backend& backend,
      const std::index_sequence<I...>) noexcept {
        (...,
         _read_element(
           errors,
           I,
           std::get<I>(values),
           backend,
           std::get<I>(_deserializers)));
    }

    template <typename Elem, typename Backend, typename Deserializer>
    static void _read_element(
      deserialization_errors& errors,
      const std::size_t index,
      Elem& elem,
      Backend& backend,
      Deserializer& deserial) noexcept {
        if(not errors) [[likely]] {
            errors |= backend.begin_element(span_size(index));
            if(not errors) [[likely]] {
                errors |= deserial.read(elem, backend);
                errors |= backend.finish_element(span_size(index));
            }
        }
    }

    std::tuple<deserializer<T>...> _deserializers{};
};
//------------------------------------------------------------------------------
export template <typename... T>
struct deserializer<std::tuple<std::pair<const string_view, T>...>>
  : common_deserializer<std::tuple<std::pair<const string_view, T>...>> {

    using common_deserializer<
      std::tuple<std::pair<const string_view, T>...>>::read;

    auto read(
      std::tuple<std::pair<const string_view, T>...>& values,
      auto& backend) noexcept {
        deserialization_errors errors{};
        span_size_t memb_count{0};
        errors |= backend.begin_struct(memb_count);
        if(memb_count < span_size(sizeof...(T))) [[unlikely]] {
            errors |= deserialization_error_code::missing_member;
        } else if(memb_count > span_size(sizeof...(T))) [[unlikely]] {
            errors |= deserialization_error_code::excess_member;
        }
        if(errors.has_at_most(deserialization_error_code::excess_member))
          [[likely]] {
            _read_members(
              errors, values, backend, std::make_index_sequence<sizeof...(T)>());
            errors |= backend.finish_struct();
        }
        return errors;
    }

private:
    template <typename Tuple, typename Backend, std::size_t... I>
    void _read_members(
      deserialization_errors& errors,
      Tuple& values,
      Backend& backend,
      const std::index_sequence<I...>) noexcept {
        (...,
         _read_member(
           errors,
           std::get<0>(std::get<I>(values)),
           std::get<1>(std::get<I>(values)),
           backend,
           std::get<I>(_deserializers)));
    }

    template <typename Memb, typename Backend, typename Serializer>
    static void _read_member(
      deserialization_errors& errors,
      const string_view name,
      Memb& value,
      Backend& backend,
      Serializer& serial) noexcept {
        if(not errors) [[likely]] {
            errors |= backend.begin_member(name);
            if(not errors) [[likely]] {
                errors |= serial.read(value, backend);
                errors |= backend.finish_member(name);
            }
        }
    }

    std::tuple<deserializer<T>...> _deserializers{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct deserializer<fragment_deserialize_wrapper<span<T>>>
  : common_deserializer<fragment_deserialize_wrapper<span<T>>> {

    using common_deserializer<fragment_deserialize_wrapper<span<T>>>::read;

    auto read(fragment_deserialize_wrapper<span<T>>& frag, auto& backend)
      const noexcept {
        deserialization_errors errors{};
        span_size_t offs{0};
        errors |= _size_deserializer.read(offs, backend);
        if(not errors) [[likely]] {
            span_size_t size{0};
            errors |= _size_deserializer.read(size, backend);
            if(not errors) [[likely]] {
                span_size_t done{0};
                errors |= backend.read(frag.slice(offs, size), done);
                frag.mark_done(offs, done);
            }
        }
        return errors;
    }

    deserializer<span_size_t> _size_deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T, std::size_t N>
struct deserializer<std::array<T, N>> : common_deserializer<std::array<T, N>> {

    using common_deserializer<std::array<T, N>>::read;

    auto read(std::array<T, N>& values, auto& backend) const noexcept {
        deserialization_errors errors{};
        span_size_t elem_count{0};
        errors |= backend.begin_list(elem_count);
        if(elem_count < span_size(N)) [[unlikely]] {
            errors |= deserialization_error_code::missing_element;
        } else if(elem_count > span_size(N)) [[unlikely]] {
            errors |= deserialization_error_code::excess_element;
        }
        if(errors.has_at_most(deserialization_error_code::excess_element))
          [[likely]] {
            errors |= _elem_deserializer.read(cover(values), backend);
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    deserializer<T> _elem_deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename A>
struct deserializer<std::vector<T, A>>
  : common_deserializer<std::vector<T, A>> {

    auto read(std::vector<T, A>& values, auto& backend) const noexcept {
        deserialization_errors errors{};
        span_size_t elem_count{0};
        errors |= backend.begin_list(elem_count);
        if(not errors) [[likely]] {
            values.resize(integer(elem_count));
            errors |= _elem_deserializer.read(cover(values), backend);
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    deserializer<T> _elem_deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename P>
struct deserializer<valid_if<T, P>> : common_deserializer<valid_if<T, P>> {

    auto read(valid_if<T, P>& value, auto& backend) const noexcept {
        deserialization_errors errors{};
        span_size_t elem_count{0};
        errors |= backend.begin_list(elem_count);
        if(elem_count < 0) [[unlikely]] {
            errors |= deserialization_error_code::missing_element;
        } else if(elem_count > 1) [[unlikely]] {
            errors |= deserialization_error_code::excess_element;
        }
        if(not errors) [[likely]] {
            T temp{};
            errors |= _deserializer.read(temp, backend);
            errors |= backend.finish_list();
            if(not errors) [[likely]] {
                _assign(value, std::move(temp));
            }
        }
        return errors;
    }

private:
    static void _assign(valid_if<T, P>& dest, T&& src) noexcept {
        if constexpr(std::is_same_v<P, valid_flag_policy>) {
            dest = {std::move(src), true};
        } else {
            dest = std::move(src);
        }
    }

    deserializer<T> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename U>
struct deserializer<tagged_quantity<T, U>>
  : common_deserializer<tagged_quantity<T, U>> {

    auto read(tagged_quantity<T, U>& value, auto& backend) const noexcept {
        deserialization_errors errors{};
        T temp{};
        errors |= _deserializer.read(temp, backend);
        if(not errors) [[likely]] {
            value = tagged_quantity<T, U>{std::move(temp)};
        }
        return errors;
    }

private:
    deserializer<T> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct enum_deserializer {

    auto read(T& enumerator, auto& backend) const noexcept {
        deserialization_errors errors{};
        if(backend.enum_as_string()) {
            decl_name_storage temp_name{};
            errors |= _name_deserializer.read(temp_name, backend);
            if(not errors) [[likely]] {
                if(const auto found{from_string<T>(temp_name.get())}) {
                    enumerator = *found;
                } else {
                    errors |= deserialization_error_code::unexpected_data;
                }
            }
        } else {
            std::underlying_type_t<T> temp_value{};
            errors |= _value_deserializer.read(temp_value, backend);
            if(not errors) [[likely]] {
                if(const auto found{
                     from_value(temp_value, std::type_identity<T>{})}) {
                    enumerator = *found;
                } else {
                    errors |= deserialization_error_code::unexpected_data;
                }
            }
        }
        return errors;
    }

private:
    deserializer<std::underlying_type_t<T>> _value_deserializer{};
    deserializer<decl_name_storage> _name_deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct struct_deserializer {
public:
    auto read(T& instance, auto& backend) noexcept {
        auto member_map = map_data_members(instance);
        return _deserializer.read(member_map, backend);
    }

private:
    deserializer<decltype(map_data_members(std::declval<T&>()))> _deserializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct deserializer
  : std::conditional_t<
      mapped_enum<T>,
      enum_deserializer<T>,
      std::conditional_t<mapped_struct<T>, struct_deserializer<T>, nothing_t>> {
};
//------------------------------------------------------------------------------
/// @brief Deserializes a value with the specified serialization backend.
/// @ingroup serialization
/// @see serialize
/// @see deserializer_backend
export template <typename T, typename Backend>
[[nodiscard]] auto deserialize(T& value, Backend& backend) noexcept
  -> deserialization_result<T&>
    requires(std::is_base_of_v<deserializer_backend, Backend>)
{
    deserialization_errors errors{};
    errors |= backend.begin();
    if(not errors) [[likely]] {
        deserializer<T> reader;
        errors |= reader.read(value, backend);
        errors |= backend.finish();
    }
    return {value, errors};
}
//------------------------------------------------------------------------------
} // namespace eagine

