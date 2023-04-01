/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:data_buffer;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;

namespace eagine {
//------------------------------------------------------------------------------
export template <std::size_t S, bool C = true>
struct serialize_size_constant : size_constant<S> {

    static constexpr bool is_constant = C;

    template <typename X>
    static constexpr auto get(const X&) noexcept {
        return std_size(S);
    }

    template <typename X>
    static constexpr auto get(string_view s, const X&) noexcept {
        return std_size(safe_add(12, s.size(), 2, span_size(S)));
    }
};
//------------------------------------------------------------------------------
export template <identifier_t SerializerId, typename T, typename Selector>
struct get_serialize_buffer_size;

export template <identifier_t SerializerId, typename T, typename Selector>
constexpr const auto serialize_buffer_size_v =
  get_serialize_buffer_size<SerializerId, T, Selector>::value;

export template <identifier_t SerializerId, typename T, typename Selector>
constexpr const auto serialize_buffer_is_constant_v =
  get_serialize_buffer_size<SerializerId, T, Selector>::value;
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename T, typename Selector>
struct get_serialize_buffer_size<Sid, T&, Selector>
  : get_serialize_buffer_size<Sid, T, Selector> {};

export template <identifier_t Sid, typename T, typename Selector>
struct get_serialize_buffer_size<Sid, const T, Selector>
  : get_serialize_buffer_size<Sid, T, Selector> {};

export template <identifier_t Sid, typename T, typename Selector>
struct get_serialize_buffer_size<Sid, const T&, Selector>
  : get_serialize_buffer_size<Sid, T, Selector> {};
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, bool, Selector>
  : serialize_size_constant<8> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, byte, Selector>
  : serialize_size_constant<4> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, char, Selector>
  : serialize_size_constant<4> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, short int, Selector>
  : serialize_size_constant<6> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, unsigned short int, Selector>
  : serialize_size_constant<6> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, int, Selector>
  : serialize_size_constant<12> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, unsigned int, Selector>
  : serialize_size_constant<12> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, long int, Selector>
  : serialize_size_constant<24> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, unsigned long int, Selector>
  : serialize_size_constant<24> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, long long int, Selector>
  : serialize_size_constant<24> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, unsigned long long int, Selector>
  : serialize_size_constant<24> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, float, Selector>
  : serialize_size_constant<24> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, double, Selector>
  : serialize_size_constant<48> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, identifier, Selector>
  : serialize_size_constant<12> {};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, string_view, Selector>
  : serialize_size_constant<0, false> {

    static constexpr auto get(string_view s, string_view v) noexcept {
        return std_size(safe_add(12, s.size(), 2, v.size()));
    }
};

export template <identifier_t Sid, typename Selector>
struct get_serialize_buffer_size<Sid, std::string, Selector>
  : serialize_size_constant<0, false> {

    static constexpr auto get(string_view s, const std::string& v) noexcept {
        return std_size(safe_add(12, s.size(), 2, span_size(v.size())));
    }
};
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename T, std::size_t N, typename Selector>
struct get_serialize_buffer_size<Sid, std::array<T, N>, Selector>
  : serialize_size_constant<
      (4 + (2 + serialize_buffer_size_v<Sid, T, Selector>)*N),
      (get_serialize_buffer_size<Sid, T, Selector>::is_constant)> {

    static constexpr auto get(
      string_view s,
      const std::array<T, N>& v) noexcept {
        auto result = std_size(12 + s.size());
        for(const auto& e : v) {
            result +=
              2 + get_serialize_buffer_size<Sid, T, Selector>::get({}, e);
        }
        return result;
    }
};
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename... T, typename Selector>
struct get_serialize_buffer_size<Sid, std::tuple<T...>, Selector>
  : serialize_size_constant<
      (4 + ... + (2 + serialize_buffer_size_v<Sid, T, Selector>)),
      (true and ... and (serialize_buffer_is_constant_v<Sid, T, Selector>))> {

    template <typename Tup>
    static constexpr auto get(string_view s, const Tup& v) noexcept {
        return std_size(safe_add(
          12,
          s.size(),
          2,
          _do_get(v, std::make_index_sequence<sizeof...(T)>())));
    }

private:
    template <typename Tup, std::size_t... I>
    static constexpr auto _do_get(
      const Tup& t,
      std::index_sequence<I...>) noexcept {
        return safe_add(safe_add(
          2,
          get_serialize_buffer_size<Sid, T, Selector>::get(
            {}, std::get<I>(t)))...);
    }
};
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename T, typename Selector>
struct get_struct_serialize_buffer_size : serialize_size_constant<0, false> {

    static constexpr auto get(string_view s, const T& v) noexcept {
        return std_size(safe_add(12, s.size(), 2, _get(map_data_members(v))));
    }

private:
    template <typename Mapped>
    static constexpr auto _get(const Mapped& mapped) noexcept {
        return _do_get(
          mapped, std::make_index_sequence<std::tuple_size_v<Mapped>>());
    }

    template <typename... M, std::size_t... I>
    static constexpr auto _do_get(
      const std::tuple<std::pair<const string_view, const M&>...>& mapped,
      std::index_sequence<I...>) noexcept {
        return span_size(safe_add(safe_add(
          2,
          get_serialize_buffer_size<Sid, M, Selector>::get(
            std::get<I>(mapped).first, std::get<I>(mapped).second))...));
    }
};
//------------------------------------------------------------------------------
export template <identifier_t Sid, typename T, typename Selector>
struct get_serialize_buffer_size
  : std::conditional_t<
      default_mapped_struct<T>,
      get_struct_serialize_buffer_size<Sid, T, Selector>,
      std::
        conditional_t<default_mapped_enum<T>, serialize_size_constant<96>, void>> {
};
//------------------------------------------------------------------------------
export template <identifier_t SerializerId, typename T, typename Selector>
constexpr auto get_serialize_array_for(const T&, Selector) noexcept
  -> std::array<byte, serialize_buffer_size_v<SerializerId, T, Selector>> {
    return {};
}
//------------------------------------------------------------------------------
export template <identifier_t SerializerId, typename T, typename Selector>
auto serialize_buffer_size_for(const T& inst, Selector) noexcept
  -> span_size_t {
    return get_serialize_buffer_size<SerializerId, T, Selector>::get({}, inst);
}
//------------------------------------------------------------------------------
export template <identifier_t SerializerId, typename T, typename Selector>
auto get_serialize_vector_for(const T& inst, Selector sel) noexcept
  -> std::vector<byte> {
    std::vector<byte> result;
    result.resize(serialize_buffer_size_for<SerializerId>(inst, sel));
    return result;
}
//------------------------------------------------------------------------------
/// @brief Returns a buffer large enough for the serialization of the specified instance.
/// @ingroup serialization
export template <
  identifier_t SerializerId,
  typename T,
  typename Selector = default_selector_t>
constexpr auto serialize_buffer_for(const T& inst, Selector sel = {}) noexcept {
    if constexpr(get_serialize_buffer_size<SerializerId, T, Selector>::
                   is_constant) {
        return get_serialize_array_for<SerializerId>(inst, sel);
    } else {
        return get_serialize_vector_for<SerializerId>(inst, sel);
    }
}
//------------------------------------------------------------------------------
} // namespace eagine

