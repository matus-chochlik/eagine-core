/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:from_string;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :c_str;

namespace eagine {
//------------------------------------------------------------------------------
export template <>
struct string_traits<string_view> {
    static auto from(const string_view src) noexcept
      -> always_valid<string_view> {
        return src;
    }
};
//------------------------------------------------------------------------------
export template <>
struct string_traits<std::string_view> {
    static auto from(const string_view src) noexcept
      -> always_valid<std::string_view> {
        return src.std_view();
    }
};
//------------------------------------------------------------------------------
export template <>
struct string_traits<std::string> {
    static auto from(const string_view src) noexcept
      -> always_valid<std::string>;
};

export template <>
struct string_traits<char> {
    static auto from(const string_view src) noexcept -> optionally_valid<char>;
};

export template <>
struct string_traits<bool> {
    static auto from(const string_view src) noexcept -> optionally_valid<bool>;
};

export template <>
struct string_traits<tribool> {
    static auto from(const string_view src) noexcept
      -> optionally_valid<tribool>;
};

export template <>
struct string_traits<byte> {
    static auto from(const string_view src) noexcept -> optionally_valid<byte>;
};
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long long int&) noexcept -> bool;
//------------------------------------------------------------------------------
export template <std::integral T>
[[nodiscard]] auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> optionally_valid<T> {
    long long int parsed{};
    if(_parse_from_string(src, parsed)) {
        return convert_if_fits<T>(parsed);
    }
    return {};
}
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long double&) noexcept -> bool;
//------------------------------------------------------------------------------
export template <std::floating_point T>
[[nodiscard]] auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> optionally_valid<T> {
    long double parsed{};
    if(_parse_from_string(src, parsed)) {
        return convert_if_fits<T>(parsed);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <>
struct string_traits<short> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<short>;
};

export template <>
struct string_traits<int> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<int>;
};

export template <>
struct string_traits<long> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<long>;
};

export template <>
struct string_traits<long long> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<long long>;
};

export template <>
struct string_traits<unsigned short> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<unsigned short>;
};

export template <>
struct string_traits<unsigned int> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<unsigned int>;
};

export template <>
struct string_traits<unsigned long> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<unsigned long>;
};

export template <>
struct string_traits<unsigned long long> {
    static auto from(const string_view src, const int base = 10) noexcept
      -> optionally_valid<unsigned long long>;
};
//------------------------------------------------------------------------------
export template <>
struct string_traits<float> {
    static auto from(const string_view src) noexcept -> optionally_valid<float>;
};

export template <>
struct string_traits<double> {
    static auto from(const string_view src) noexcept
      -> optionally_valid<double>;
};

export template <>
struct string_traits<long double> {
    static auto from(const string_view src) noexcept
      -> optionally_valid<long double>;
};
//------------------------------------------------------------------------------
export template <typename Rep, typename Period>
struct string_traits<std::chrono::duration<Rep, Period>> {
private:
    template <typename P>
    static auto _convert(
      const string_view src,
      const std::type_identity<std::chrono::duration<Rep, P>>,
      const string_view symbol) noexcept
      -> optionally_valid<std::chrono::duration<Rep, P>> {
        if(memory::ends_with(src, symbol)) {
            if(const auto opt_val{from_string(
                 snip(src, symbol.size()), std::type_identity<Rep>())}) {
                return {std::chrono::duration<Rep, P>(*opt_val), true};
            }
        }
        return {};
    }

public:
    static auto from(const string_view str) noexcept
      -> optionally_valid<std::chrono::duration<Rep, Period>> {
        using dur_t = std::chrono::duration<Rep, Period>;

        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::ratio<1>>>(),
             "s")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::milli>>(),
             "ms")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::micro>>(),
             "μs")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::nano>>(),
             "ns")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::ratio<60>>>(),
             "min")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<std::chrono::duration<Rep, std::ratio<3600>>>(),
             "hr")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<
               std::chrono::duration<Rep, std::ratio<86400LL>>>(),
             "dy")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<
               std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
             "yr")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        if(const auto d{_convert(
             str,
             std::type_identity<
               std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
             "a")}) {
            return {std::chrono::duration_cast<dur_t>(*d), true};
        }
        return {};
    }
};
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename Log>
struct string_traits<basic_valid_if<T, Policy, Log>> {
    static auto from(const string_view str) noexcept -> optionally_valid<T> {
        if(auto temp{from_string(str, std::type_identity<T>())}) {
            return {std::move(*temp), Policy()(*temp)};
        }
        return {};
    }
};
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
export template <typename T>
[[nodiscard]] auto from_strings(
  const span<const string_view> src,
  const std::type_identity<T> tid = {}) noexcept {
    using eagine::from_string;
    if(src.has_value()) {
        return from_string(*src, tid);
    }
    return decltype(from_string(*src, tid)){};
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] auto assign_if_fits(const string_view src, T& dst) noexcept
  -> bool {
    if(auto conv{from_string(src, std::type_identity<T>{})}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const string_view src,
  std::optional<T>& dst) noexcept -> bool {
    if(auto conv{from_string(src, std::type_identity<T>{})}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  T& dst) noexcept -> bool {
    if(auto conv{from_strings(src, std::type_identity<T>{})}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  std::optional<T>& dst) noexcept -> bool {
    if(auto conv{from_strings(src, std::type_identity<T>{})}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename X, typename T>
[[nodiscard]] constexpr auto assign_if_fits(
  placeholder_expression<X> e,
  T& dst) noexcept {
    return placeholder_expression{[e, &dst](auto&&... args) mutable {
        return tribool{
          assign_if_fits(string_view{e(decltype(args)(args)...)}, dst)};
    }};
}
//------------------------------------------------------------------------------
export template <typename T, typename X>
[[nodiscard]] constexpr auto from_string(
  placeholder_expression<X> e,
  std::type_identity<T> tid = {}) noexcept {
    return placeholder_expression{[e, tid](auto&&... args) mutable {
        return from_string(string_view{e(decltype(args)(args)...)}, tid);
    }};
}
//------------------------------------------------------------------------------
/// @brief Checks if the string can be converted to a value equal to the one given.
/// @ingroup type_utils
/// @see from_string
export template <typename T>
[[nodiscard]] auto string_has_value(
  const string_view str,
  const T& value) noexcept -> bool {
    if(const auto converted{from_string(str, std::type_identity<T>{})}) {
        return are_equal(*converted, value);
    }
    return false;
}

export template <typename T>
[[nodiscard]] constexpr auto string_has_value(
  const string_view str,
  const string_view value) noexcept -> bool {
    return str == value;
}

export template <std::size_t L>
[[nodiscard]] constexpr auto string_has_value(
  const string_view str,
  const char (&value)[L]) noexcept -> bool {
    return str == string_view{value};
}

export template <typename X, typename T>
[[nodiscard]] constexpr auto string_has_value(
  placeholder_expression<X> e,
  const T& value) noexcept {
    return placeholder_expression{[e, value](auto&&... args) mutable {
        return tribool{
          string_has_value(string_view{e(decltype(args)(args)...)}, value)};
    }};
}
//------------------------------------------------------------------------------
} // namespace eagine
