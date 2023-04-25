/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:write;

import std;
import eagine.core.types;
import eagine.core.memory;
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
class fragment_serialize_wrapper;

export template <typename T>
class fragment_serialize_wrapper<span<const T>> {
public:
    constexpr fragment_serialize_wrapper() noexcept = default;

    fragment_serialize_wrapper(const span<const T> src) noexcept
      : _src{src} {}

    auto remaining() const noexcept {
        return skip(_src, _offset);
    }

    auto offset() const noexcept {
        return _offset;
    }

    void advance(const span_size_t inc) noexcept {
        _offset += inc;
    }

    auto is_done() const noexcept -> bool {
        return not remaining();
    }

private:
    span<const T> _src{};
    span_size_t _offset{0};
};
//------------------------------------------------------------------------------
export template <typename T>
struct serializer;

export template <typename T>
struct serializer<const T> : serializer<T> {};

export template <typename T>
struct serializer<T&> : serializer<T> {};

export template <typename T>
struct serializer<const T&> : serializer<T> {};
//------------------------------------------------------------------------------
export template <typename T>
struct plain_serializer {
    auto write(const T value, auto& backend) const noexcept {
        span_size_t written{0};
        const auto errors{backend.write(view_one(value), written)};
        if(written < 1) [[unlikely]] {
            assert(errors.has(serialization_error_code::too_much_data));
        }
        return errors;
    }

    auto write(const span<const T> values, auto& backend) const noexcept {
        span_size_t written{0};
        const auto errors{backend.write(values, written)};
        if(written < 1) [[unlikely]] {
            assert(errors.has(serialization_error_code::too_much_data));
        } else if(written < values.size()) [[unlikely]] {
            assert(
              errors.has(serialization_error_code::incomplete_write) or
              errors.has(serialization_error_code::too_much_data));
        }
        return errors;
    }
};

//------------------------------------------------------------------------------
export template <>
struct serializer<bool> : plain_serializer<bool> {};
export template <>
struct serializer<char> : plain_serializer<char> {};
export template <>
struct serializer<std::int8_t> : plain_serializer<std::int8_t> {};
export template <>
struct serializer<short> : plain_serializer<short> {};
export template <>
struct serializer<int> : plain_serializer<int> {};
export template <>
struct serializer<long> : plain_serializer<long> {};
export template <>
struct serializer<long long> : plain_serializer<long long> {};
export template <>
struct serializer<std::uint8_t> : plain_serializer<std::uint8_t> {};
export template <>
struct serializer<unsigned short> : plain_serializer<unsigned short> {};
export template <>
struct serializer<unsigned> : plain_serializer<unsigned> {};
export template <>
struct serializer<unsigned long> : plain_serializer<unsigned long> {};
export template <>
struct serializer<unsigned long long> : plain_serializer<unsigned long long> {};
export template <>
struct serializer<float> : plain_serializer<float> {};
export template <>
struct serializer<double> : plain_serializer<double> {};
export template <>
struct serializer<identifier> : plain_serializer<identifier> {};
export template <>
struct serializer<decl_name> : plain_serializer<decl_name> {};
export template <>
struct serializer<string_view> : plain_serializer<string_view> {};
//------------------------------------------------------------------------------
export template <typename T>
struct structural_serializer {
    auto write(const T& value, auto& backend) const noexcept {
        return _serializer.write(value.get_structure(), backend);
    }

private:
    serializer<typename T::structure_type> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct common_serializer {

    auto write(const span<const T> values, auto& backend) const noexcept {
        const auto tmd = serialization_error_code::too_much_data;
        const auto icw = serialization_error_code::incomplete_write;
        auto& sink = extract(backend.sink());
        serialization_errors errors{};
        span_size_t i = 0;
        for(const auto& elem : values) {
            const auto th = sink.begin_work();
            errors |= backend.begin_element(i);
            if(not errors) [[likely]] {
                errors |=
                  static_cast<const serializer<T>*>(this)->write(elem, backend);
                errors |= backend.finish_element(i++);
                if(not errors) [[likely]] {
                    errors |= sink.commit(th);
                } else if(errors.has_at_most(tmd)) {
                    errors.clear(tmd);
                    errors |= icw;
                    sink.rollback(th);
                    break;
                } else {
                    break;
                }
            } else if(errors.has_only(tmd)) {
                errors.clear(tmd);
                errors |= icw;
                sink.rollback(th);
                break;
            } else {
                break;
            }
        }
        return errors;
    }
};
//------------------------------------------------------------------------------
export template <typename... T>
struct serializer<std::tuple<T...>> : common_serializer<std::tuple<T...>> {

    using common_serializer<std::tuple<T...>>::write;

    auto write(const std::tuple<T...>& values, auto& backend) const noexcept {
        auto errors{backend.begin_list(span_size(sizeof...(T)))};
        if(not errors) [[likely]] {
            _write_elements(
              errors, values, backend, std::make_index_sequence<sizeof...(T)>());
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    template <typename Tuple, typename Backend, std::size_t... I>
    void _write_elements(
      serialization_errors& errors,
      Tuple& values,
      Backend& backend,
      std::index_sequence<I...>) const noexcept {
        (...,
         _write_element(
           errors, I, std::get<I>(values), backend, std::get<I>(_serializers)));
    }

    template <typename Elem, typename Backend, typename Serializer>
    void _write_element(
      serialization_errors& errors,
      const std::size_t index,
      Elem& elem,
      Backend& backend,
      Serializer& serial) const noexcept {
        if(not errors) [[likely]] {
            errors |= backend.begin_element(span_size(index));
            if(not errors) [[likely]] {
                errors |= serial.write(elem, backend);
                errors |= backend.finish_element(span_size(index));
            }
        }
    }

    std::tuple<serializer<T>...> _serializers{};
};
//------------------------------------------------------------------------------
export template <typename... T>
struct serializer<std::tuple<std::pair<const string_view, T>...>>
  : common_serializer<std::tuple<std::pair<const string_view, T>...>> {

    auto write(
      const std::tuple<std::pair<const string_view, T>...>& members,
      auto& backend) const noexcept {
        auto errors{backend.begin_struct(span_size(sizeof...(T)))};
        if(not errors) [[likely]] {
            _write_members(
              errors,
              members,
              backend,
              std::make_index_sequence<sizeof...(T)>());
            errors |= backend.finish_struct();
        }
        return errors;
    }

private:
    template <typename Tuple, typename Backend, std::size_t... I>
    void _write_members(
      serialization_errors& errors,
      Tuple& members,
      Backend& backend,
      std::index_sequence<I...>) const noexcept {
        (...,
         _write_member(
           errors,
           std::get<0>(std::get<I>(members)),
           std::get<1>(std::get<I>(members)),
           backend,
           std::get<I>(_serializers)));
    }

    template <typename Memb, typename Backend, typename Serializer>
    void _write_member(
      serialization_errors& errors,
      string_view name,
      Memb& value,
      Backend& backend,
      Serializer& serial) const noexcept {
        if(not errors) [[likely]] {
            errors |= backend.begin_member(name);
            if(not errors) [[likely]] {
                errors |= serial.write(value, backend);
                errors |= backend.finish_member(name);
            }
        }
    }

    std::tuple<serializer<T>...> _serializers{};
};
//------------------------------------------------------------------------------
export template <typename Bit>
struct serializer<bitfield<Bit>> : common_serializer<bitfield<Bit>> {

    auto write(bitfield<Bit> value, auto& backend) const noexcept {
        return _serializer.write(value.bits(), backend);
    }

private:
    serializer<typename bitfield<Bit>::value_type> _serializer{};
};
//------------------------------------------------------------------------------
export template <std::size_t N>
struct serializer<char[N]> : serializer<string_view> {};
//------------------------------------------------------------------------------
export template <typename Char, typename Traits, typename Alloc>
struct serializer<std::basic_string<Char, Traits, Alloc>>
  : common_serializer<std::basic_string<Char, Traits, Alloc>> {

    using common_serializer<std::basic_string<Char, Traits, Alloc>>::write;

    auto write(
      const std::basic_string<Char, Traits, Alloc>& value,
      auto& backend) const noexcept {
        return _serializer.write(value, backend);
    }

private:
    serializer<string_view> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct serializer<span<const T>> : common_serializer<span<const T>> {

    using common_serializer<span<const T>>::write;

    auto write(const span<const T> values, auto& backend) const noexcept {
        auto errors{backend.begin_list(values.size())};
        if(not errors) [[likely]] {
            errors |= _elem_serializer.write(values, backend);
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    serializer<T> _elem_serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct serializer<span<T>> : serializer<span<const T>> {};
//------------------------------------------------------------------------------
export template <typename T>
struct serializer<fragment_serialize_wrapper<span<const T>>>
  : common_serializer<fragment_serialize_wrapper<span<const T>>> {

    using common_serializer<fragment_serialize_wrapper<span<const T>>>::write;

    auto write(fragment_serialize_wrapper<span<const T>>& frag, auto& backend)
      const noexcept {
        auto errors{_size_serializer.write(frag.offset(), backend)};
        if(not errors) [[likely]] {
            const auto todo = frag.remaining();
            errors |= _size_serializer.write(todo.size(), backend);
            if(not errors) [[likely]] {
                span_size_t written{0};
                errors |= backend.write(todo, written);
                if(errors.has_at_most(
                     serialization_error_code::incomplete_write)) [[likely]] {
                    frag.advance(written);
                }
            }
        }
        return errors;
    }

    serializer<span_size_t> _size_serializer{};
};
//------------------------------------------------------------------------------
export template <typename T, std::size_t N>
struct serializer<std::array<T, N>> : common_serializer<std::array<T, N>> {

    using common_serializer<std::array<T, N>>::write;

    auto write(const std::array<T, N>& values, auto& backend) const noexcept {
        auto errors{backend.begin_list(span_size(N))};
        if(not errors) [[likely]] {
            errors |= _elem_serializer.write(view(values), backend);
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    serializer<T> _elem_serializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename A>
struct serializer<std::vector<T, A>> : common_serializer<std::vector<T, A>> {

    using common_serializer<std::vector<T, A>>::write;

    auto write(const std::vector<T, A>& values, auto& backend) const noexcept {
        auto errors{backend.begin_list(values.size())};
        if(not errors) [[likely]] {
            errors |= _elem_serializer.write(view(values), backend);
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    serializer<T> _elem_serializer{};
};
//------------------------------------------------------------------------------
export template <typename Rep>
struct serializer<std::chrono::duration<Rep>>
  : common_serializer<std::chrono::duration<Rep>> {

    auto write(const std::chrono::duration<Rep> value, auto& backend)
      const noexcept {
        return _serializer.write(value.count(), backend);
    }

private:
    serializer<Rep> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename P>
struct serializer<valid_if<T, P>> : common_serializer<valid_if<T, P>> {

    auto write(const valid_if<T, P>& value, auto& backend) const noexcept {
        const bool has_value{value.has_value()};
        auto errors{backend.begin_list(span_size(has_value))};
        if(not errors) [[likely]] {
            if(has_value) {
                errors |= _serializer.write(value.value_anyway(), backend);
            }
            errors |= backend.finish_list();
        }
        return errors;
    }

private:
    serializer<T> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T, typename U>
struct serializer<tagged_quantity<T, U>>
  : common_serializer<tagged_quantity<T, U>> {

    auto write(const tagged_quantity<T, U>& qty, auto& backend) const noexcept {
        return _serializer.write(qty.value(), backend);
    }

private:
    serializer<T> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct enum_serializer {
    auto write(const T enumerator, auto& backend) const noexcept {
        if(backend.enum_as_string()) {
            return _name_serializer.write(enumerator_name(enumerator), backend);
        } else {
            return _value_serializer.write(
              enumerator_value(enumerator), backend);
        }
    }

private:
    serializer<std::underlying_type_t<T>> _value_serializer{};
    serializer<decl_name> _name_serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct struct_serializer {
public:
    auto write(const T& instance, auto& backend) const noexcept {
        const auto member_map = map_data_members(instance);
        return _serializer.write(member_map, backend);
    }

private:
    serializer<decltype(map_data_members(std::declval<T>()))> _serializer{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct serializer
  : std::conditional_t<
      bool(default_mapped_enum<T>),
      enum_serializer<T>,
      std::conditional_t<
        (default_mapped_struct<T>),
        struct_serializer<T>,
        nothing_t>> {};
//------------------------------------------------------------------------------
/// @brief Serializes a value with the specified serialization backend.
/// @ingroup serialization
/// @see deserialize
/// @see serializer_backend
export template <typename T, typename Backend>
[[nodiscard]] auto serialize(const T& value, Backend& backend) noexcept
  -> serialization_result<const T&>
    requires(std::is_base_of_v<serializer_backend, Backend>)
{
    auto errors{backend.begin()};
    if(not errors) [[likely]] {
        serializer<std::remove_cv_t<T>> writer;
        errors |= writer.write(value, backend);
        errors |= backend.finish();
    }
    errors |= extract(backend.sink()).finalize();
    return {value, errors};
}
//------------------------------------------------------------------------------
} // namespace eagine
