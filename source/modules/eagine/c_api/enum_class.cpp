/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.c_api:enum_class;

import eagine.core.concepts;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.container;
import eagine.core.identifier;
import <compare>;
import <cstdint>;
import <tuple>;
import <type_traits>;

namespace eagine::c_api {
//------------------------------------------------------------------------------
/// @brief Class holding the value of a (typically C-API) symbolic constant.
/// @tparam T the constant or enumerator value type.
/// @tparam ClassList a list of enum_class specializations to which the value
///         "belongs" and can be converted to.
/// @tparam Tag a tag type that can be used to customize some operations.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see any_enum_value
/// @see opt_enum_value
/// @see no_enum_value
export template <typename T, typename ClassList, typename Tag = nothing_t>
struct enum_value;

/// @brief Class holding the value of a symbolic constant or enumerator.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see any_enum_value
/// @see opt_enum_value
/// @see no_enum_value
export template <typename T, typename... Classes, typename Tag>
struct enum_value<T, mp_list<Classes...>, Tag> {
    using type = enum_value;

    /// @brief The constant or enumerator value type.
    using value_type = T;

    /// @brief The tag type specified as template argument.
    using tag_type = Tag;

    /// @brief The actual enumerator or constant value.
    const T value{};

    /// @brief Initialization from the specified value.
    constexpr enum_value(const value_type val) noexcept
      : value{val} {}

    /// @brief Explicit conversion to the value type.
    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    /// @brief Indicates whether the value is valid or not (always true here).
    explicit constexpr operator bool() const noexcept {
        return true;
    }
};

export template <typename... Classes, typename Tag>
struct enum_value<bool, mp_list<Classes...>, Tag> {
    using type = enum_value;

    using value_type = bool;
    using tag_type = Tag;

    const bool value{false};

    constexpr enum_value(const bool val) noexcept
      : value{val} {}

    explicit constexpr operator bool() const noexcept {
        return value;
    }
};
//------------------------------------------------------------------------------
/// @brief Class holding optional value of a (typically C-API) symbolic constant.
/// @tparam T the constant or enumerator value type.
/// @tparam ClassList a list of enum_class specializations to which the value
///         "belongs" and can be converted to.
/// @tparam Tag a tag type that can be used to customize some operations.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see enum_value
/// @see any_enum_value
/// @see no_enum_value
export template <typename T, typename ClassList, typename Tag = nothing_t>
struct opt_enum_value;

/// @brief Class holding optional value of a symbolic constant or enumerator.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see enum_value
/// @see any_enum_value
/// @see no_enum_value
export template <typename T, typename... Classes, typename Tag>
struct opt_enum_value<T, mp_list<Classes...>, Tag> {
    using type = opt_enum_value;

    /// @brief The constant or enumerator value type.
    using value_type = T;

    /// @brief The tag type specified as template argument.
    using tag_type = Tag;

    /// @brief The actual enumerator or constant value.
    const T value{};

    /// @brief Flag indicating if the value is valid.
    const bool is_valid{false};

    /// @brief Initialization from the specified value and validity indicator.
    constexpr opt_enum_value(const T val, const bool valid) noexcept
      : value{val}
      , is_valid{valid} {}

    /// @brief Initialization from the specified value and validity indicator.
    constexpr opt_enum_value(const std::tuple<value_type, bool> init) noexcept
      : value{std::get<0>(init)}
      , is_valid{std::get<1>(init)} {}

    /// @brief Explicit conversion to the value type.
    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    /// @brief Indicates whether the value is valid or not.
    /// @see is_valid
    explicit constexpr operator bool() const noexcept {
        return is_valid;
    }
};

export template <typename... Classes, typename Tag>
struct opt_enum_value<bool, mp_list<Classes...>, Tag> {
    using type = opt_enum_value;

    using value_type = bool;
    using tag_type = Tag;

    const bool value{};
    const bool is_valid{false};

    constexpr opt_enum_value(const bool val, const bool valid) noexcept
      : value(val)
      , is_valid{valid} {}

    constexpr opt_enum_value(const std::tuple<bool, bool> init) noexcept
      : value(std::get<0>(init))
      , is_valid{std::get<1>(init)} {}

    explicit constexpr operator bool() const noexcept {
        return is_valid and value;
    }
};
//------------------------------------------------------------------------------
/// @brief Class representing undefined value of a (typically C-API) symbolic constant.
/// @tparam T the constant or enumerator value type.
/// @tparam Tag a tag type that can be used to customize some operations.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see enum_value
/// @see any_enum_value
/// @see opt_enum_value
export template <typename T, typename Tag = nothing_t>
struct no_enum_value {
    using type = no_enum_value;

    /// @brief The constant or enumerator value type.
    using value_type = T;

    /// @brief The tag type specified as template argument.
    using tag_type = Tag;

    const T value{};

    /// @brief Explicit conversion to the value type.
    explicit constexpr operator T() const noexcept {
        return value;
    }

    /// @brief Indicates whether the value is valid or not (always false here).
    explicit constexpr operator bool() const noexcept {
        return false;
    }
};

export template <typename Tag>
struct no_enum_value<bool, Tag> {
    using type = no_enum_value;

    using value_type = bool;
    using tag_type = Tag;

    explicit constexpr operator bool() const noexcept {
        return false;
    }
};
//------------------------------------------------------------------------------
export template <identifier_value LibId>
struct any_enum_value;

export template <
  typename Self,
  typename T,
  identifier_value LibId,
  identifier_value Id>
struct enum_class;

/// @brief Implementation of is_enum_class trait.
/// @ingroup c_api_wrap
export template <typename T>
struct is_enum_class : std::false_type {};

/// @brief Trait indicating if type T is an enum_class.
/// @ingroup c_api_wrap
/// @see is_enum_class_value_t
/// @see enum_class
export template <typename T>
constexpr const bool is_enum_class_v = is_enum_class<T>::value;

export template <
  typename Self,
  typename T,
  identifier_value LibId,
  identifier_value Id>
struct is_enum_class<enum_class<Self, T, LibId, Id>> : std::true_type {
    static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
};
//------------------------------------------------------------------------------
/// @brief Implementation of is_enum_class_value trait.
/// @ingroup c_api_wrap
export template <typename Class, typename Value>
struct is_enum_class_value : std::false_type {};

/// @brief Trait indicating if type T is an enum_value, opt_enum_value or no_enum_value.
/// @ingroup c_api_wrap
/// @see is_enum_class_t
/// @see enum_value
/// @see opt_enum_value
/// @see no_enum_value
export template <typename C, typename V>
constexpr const auto is_enum_class_value_v =
  is_enum_class_value<type_t<C>, type_t<V>>::value;

export template <
  typename Self,
  typename T,
  typename Tag,
  identifier_value LibId,
  identifier_value Id>
struct is_enum_class_value<enum_class<Self, T, LibId, Id>, no_enum_value<T, Tag>>
  : std::true_type {
    static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
};

export template <
  typename Self,
  typename T,
  typename Classes,
  typename Tag,
  identifier_value LibId,
  identifier_value Id>
struct is_enum_class_value<
  enum_class<Self, T, LibId, Id>,
  enum_value<T, Classes, Tag>> : mp_contains<Classes, Self> {
    static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
};

export template <
  typename Self,
  typename T,
  typename Classes,
  typename Tag,
  identifier_value LibId,
  identifier_value Id>
struct is_enum_class_value<
  enum_class<Self, T, LibId, Id>,
  opt_enum_value<T, Classes, Tag>> : mp_contains<Classes, Self> {
    static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
};
//------------------------------------------------------------------------------
export template <
  typename ParameterEnumClass,
  typename Parameter,
  typename ValueType,
  typename Value>
constexpr bool is_enum_parameter_value_v =
  c_api::is_enum_class_value_v<ParameterEnumClass, Parameter> and
      std::is_same_v<typename Parameter::tag_type, nothing_t>
    ? std::is_convertible_v<Value, ValueType>
    : c_api::is_enum_class_value_v<typename Parameter::tag_type, Value>;
//------------------------------------------------------------------------------
export template <typename ParameterEnumClass, typename ValueType>
struct enum_parameter_value {
    template <typename Parameter, typename Value>
        requires(is_enum_parameter_value_v<
                  ParameterEnumClass,
                  Parameter,
                  ValueType,
                  Value>)
    constexpr enum_parameter_value(Parameter param, Value val) noexcept
      : parameter{param.value}
      , value{eagine::limit_cast<ValueType>(val.value)} {}

    constexpr enum_parameter_value(
      ParameterEnumClass param,
      ValueType val) noexcept
      : parameter{param._value}
      , value{val} {}

    typename ParameterEnumClass::value_type parameter{};
    ValueType value{};
};
//------------------------------------------------------------------------------
/// @brief Enum class for constants or enumerators (typically from a C-API).
/// @tparam Self the actual derived class based on this template.
/// @tparam T the type of the constant or enumerator value.
/// @tparam LibId an identifier of the "library" or API that this enum belongs to.
/// @tparam Id a type identifier of this enumeration (unique within an API).
/// @ingroup c_api_wrap
/// @see any_enum_class
/// @see enum_value
/// @see any_enum_value
/// @see opt_enum_value
/// @see no_enum_value
/// @see is_enum_class_v
/// @see enum_class_span
/// @see enum_class_view
/// @see enum_class_array
///
/// Instantiations of this template can be used to represent a class of
/// enumerated values from a C-API that logically belong together. All values
/// from that logical class can be stored in enum_class and they are typically
/// declared as enum_value or opt_enum_value. The conversions between enum_class
/// and enum_value do static type checking to ensure that constants from
/// unrelated enum classes cannot be assigned.
export template <
  typename Self,
  typename T,
  identifier_value LibId,
  identifier_value Id>
struct enum_class {
    using type = enum_class;

    /// @brief The constant or enumerator value type.
    using value_type = T;
    using tag_type = T;

    static constexpr const identifier_value lib_id = LibId;
    static constexpr const identifier_value id = Id;

    value_type _value{};

    /// @brief Default constructor.
    enum_class() = default;

    /// @brief Construction from a related enum_value.
    template <typename Classes, typename Tag>
    constexpr enum_class(const enum_value<T, Classes, Tag> ev) noexcept
        requires(mp_contains_v<Classes, Self>)
      : _value{ev.value} {}

    /// @brief Construction from a related opt_enum_value.
    template <typename Classes, typename Tag>
    constexpr enum_class(const opt_enum_value<T, Classes, Tag> ev) noexcept
        requires(mp_contains_v<Classes, Self>)
      : _value{ev.value} {
        assert(ev.is_valid);
    }

    /// @brief Construction from a no_enum_value.
    constexpr enum_class(const no_enum_value<T>) noexcept {}

    /// @brief Construction from an any_enum_value.
    constexpr enum_class(const any_enum_value<LibId>& aev) noexcept
      : _value{static_cast<T>(aev._value)} {
        assert(aev._type_id == Id);
    }

    /// @brief Explicit initialization from argument of value type.
    explicit constexpr enum_class(const value_type value) noexcept
      : _value{value} {}

    constexpr operator Self() const noexcept {
        return Self{_value};
    }

    /// @brief Explicit conversion to value type.
    explicit constexpr operator value_type() const noexcept {
        return _value;
    }

    template <typename V>
    explicit constexpr operator V() const noexcept
        requires(
          not std::is_same_v<bool, V> and std::is_convertible_v<value_type, V>)
    {
        return limit_cast<T>(_value);
    }

    /// @brief Comparison.
    constexpr auto operator<=>(const enum_class&) const noexcept = default;

    struct transform {
        constexpr auto operator()(const T value) noexcept {
            return enum_class<Self, T, LibId, Id>{value};
        }
    };
};

export template <
  typename Self,
  typename T,
  identifier_value LibId,
  identifier_value Id>
constexpr auto to_underlying(enum_class<Self, T, LibId, Id> val) noexcept -> T {
    return val._value;
}

export template <
  typename Dst,
  typename Self,
  typename Src,
  identifier_value LibId,
  identifier_value Id>
constexpr auto limit_cast(enum_class<Self, Src, LibId, Id> val) noexcept -> Dst
    requires(std::is_convertible_v<Src, Dst>)
{
    return limit_cast<Dst>(val._value);
}
//------------------------------------------------------------------------------
/// @brief Type erasure for instantiations of enum_class from a specified library.
/// @tparam LibId unique identifier of a "library" or API the enums belong to.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see any_enum_value
export template <identifier_value LibId>
struct any_enum_class {

    identifier_value _type_id{~identifier_t(0)};

    /// @brief Default constructor.
    constexpr any_enum_class() noexcept = default;

    /// @brief Construction from enum_class from the same "library" or API.
    template <typename Self, typename T, identifier_value Id>
    constexpr any_enum_class(const enum_class<Self, T, LibId, Id>&) noexcept
      : _type_id{Id} {
        static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
    }

    /// @brief Construction from any_enum_value from the same "library" or API.
    constexpr any_enum_class(const any_enum_value<LibId>& aev) noexcept
      : _type_id{aev._type_id} {}

    /// @brief Indicates if this is a valid enumeration class.
    explicit constexpr operator bool() const noexcept {
        return _type_id != ~identifier_t(0);
    }

    /// @brief Comparison.
    constexpr auto operator<=>(const any_enum_class&) const noexcept = default;
};

/// @brief Type erasure for instantiations of enum_value from a specified library.
/// @tparam LibId unique identifier of a "library" or API the enums belong to.
/// @ingroup c_api_wrap
/// @see enum_value
/// @see any_enum_class
export template <identifier_value LibId>
struct any_enum_value {
    long _value{0};
    identifier_value _type_id{~identifier_t(0)};

    /// @brief Default constructor.
    constexpr any_enum_value() noexcept = default;

    /// @brief Construction from enum_class from the same "library" or API.
    template <typename Self, typename T, identifier_value Id>
    constexpr any_enum_value(const enum_class<Self, T, LibId, Id> v) noexcept
      : _value{long(v._value)}
      , _type_id{Id} {
        static_assert(std::is_base_of_v<enum_class<Self, T, LibId, Id>, Self>);
    }

    /// @brief Indicates if this value has a valid enumeration class.
    explicit constexpr operator bool() const noexcept {
        return _type_id != ~identifier_t(0);
    }

    /// @brief Equality comparison.
    constexpr auto operator<=>(const any_enum_value&) const noexcept = default;
};

/// @brief Tests if two instances of any_enum_class belong to the same enum class.
/// @ingroup c_api_wrap
/// @see any_enum_class
export template <identifier_value LibId>
constexpr auto same_enum_class(
  const any_enum_class<LibId> a,
  const any_enum_class<LibId> b) noexcept {
    return a._type_id == b._type_id;
}
//------------------------------------------------------------------------------
/// @brief Template for containers of enum_class.
/// @ingroup c_api_wrap
/// @see enum_class
/// @see enum_class_span
/// @see enum_class_view
/// @see enum_class_array
export template <typename EnumClass, typename Container>
class enum_class_container
  : public basic_wrapping_container<
      Container,
      EnumClass,
      typename EnumClass::value_type> {
    using base = basic_wrapping_container<
      Container,
      EnumClass,
      typename EnumClass::value_type>;

public:
    using base::base;

    constexpr auto raw_enums() noexcept {
        return this->raw_items();
    }

    constexpr auto raw_enums() const noexcept {
        return this->raw_items();
    }
};
//------------------------------------------------------------------------------
/// @brief Alias for non-const span of multiple enum_class values.
/// @ingroup c_api_wrap
/// @see enum_class_view
/// @see enum_class_array
export template <typename EnumClass>
using enum_class_span =
  enum_class_container<EnumClass, span<typename EnumClass::value_type>>;

/// @brief Alias for const span of multiple enum_class values.
/// @ingroup c_api_wrap
/// @see enum_class_span
/// @see enum_class_array
export template <typename EnumClass>
using enum_class_view =
  enum_class_container<EnumClass, span<const typename EnumClass::value_type>>;

/// @brief Alias for array of multiple enum_class values.
/// @ingroup c_api_wrap
/// @see enum_class_span
/// @see enum_class_view
export template <typename EnumClass, std::size_t N>
using enum_class_array =
  enum_class_container<EnumClass, std::array<typename EnumClass::value_type, N>>;
//------------------------------------------------------------------------------
export template <typename Dst, typename Src, typename... Classes, typename Tag>
constexpr auto limit_cast(
  c_api::opt_enum_value<Src, Classes..., Tag> val) noexcept -> Dst
    requires(std::is_convertible_v<Src, Dst>)
{
    assert(bool(val));
    return limit_cast<Dst>(val.value);
}

export template <typename Dst, typename Src, typename Tag>
constexpr auto limit_cast(c_api::no_enum_value<Src, Tag> val) noexcept -> Dst
    requires(std::is_convertible_v<Src, Dst>)
{
    assert(bool(val));
    return limit_cast<Dst>(val.value);
}
//------------------------------------------------------------------------------
} // namespace eagine::c_api

