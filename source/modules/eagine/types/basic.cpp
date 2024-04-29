/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:basic;

import std;
import eagine.core.build_config;

namespace eagine {
//------------------------------------------------------------------------------
// basic aliases
//------------------------------------------------------------------------------
/// @brief Byte type alias.
/// @ingroup type_utils
export using byte = unsigned char;

/// @brief Alignment type used by std.
/// @ingroup type_utils
export using std_align_t = std::size_t;

/// @brief Size type used by std.
/// @ingroup type_utils
export using std_size_t = std::size_t;

/// @brief Signed span size type used by eagine.
/// @ingroup type_utils
export using span_size_t = std::ptrdiff_t;
//------------------------------------------------------------------------------
/// @brief The underlying integer type for eagine::identifier.
/// @ingroup identifiers
/// @see identifier
export using identifier_t = std::uint64_t;

#if __SIZEOF_INT128__
/// @brief The underlying integer type for eagine::long_identifier.
/// @ingroup identifiers
/// @see long_identifier
export using long_identifier_t = __uint128_t;
#else
export using long_identifier_t = identifier_t;
#endif
//------------------------------------------------------------------------------
/// @brief Unique process identifier type (does not necessarily match to OS PID).
export using process_instance_id_t = std::uint32_t;

/// @brief Unique host identifier type.
export using host_id_t = std::uint32_t;
//------------------------------------------------------------------------------
/// @brief Alias for signed int constant type.
/// @ingroup type_utils
export template <int I>
using int_constant = std::integral_constant<int, I>;

/// @brief Alias for unsigned int constant type.
/// @ingroup type_utils
export template <unsigned U>
using unsigned_constant = std::integral_constant<unsigned, U>;

/// @brief Alias for size_t constant type.
/// @ingroup type_utils
export template <std::size_t S>
using size_constant = std::integral_constant<std::size_t, S>;

/// @brief Alias for identifier_t constant type.
/// @ingroup type_utils
/// @see identifier_t
export template <identifier_t Id>
using id_constant = std::integral_constant<identifier_t, Id>;
//------------------------------------------------------------------------------
// anything
//------------------------------------------------------------------------------
/// @brief Type that can by constructed from single argument of any other type.
/// @ingroup type_utils
export struct anything {
    /// @brief Default constructor.
    constexpr anything() noexcept = default;

    /// @brief Move constructor.
    constexpr anything(anything&&) noexcept = default;

    /// @brief Copy constructor.
    constexpr anything(const anything&) noexcept = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(anything&&) noexcept -> anything& = default;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const anything&) noexcept -> anything& = default;

    ~anything() noexcept = default;

    /// @brief Construction from any other type.
    template <typename Unused>
    constexpr anything(const Unused&) noexcept {}
};
//------------------------------------------------------------------------------
// nothing
//------------------------------------------------------------------------------
/// @brief Class representing "none" / "nothing" values.
/// @ingroup type_utils
/// @see nothing
/// @see not_in_low_profile
export struct nothing_t {

    /// @brief Type alias to self.
    using type = nothing_t;

    constexpr explicit operator bool() const noexcept {
        return false;
    }

    constexpr auto has_value() const noexcept -> bool {
        return false;
    }

    constexpr auto value() const noexcept -> nothing_t {
        return {};
    }

    constexpr auto value_or(nothing_t) const noexcept -> nothing_t {
        return {};
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, nothing_t>>>
    auto and_then(F&&) -> R {
        return R{};
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, nothing_t>>>
    [[nodiscard]] auto transform(F&& function) {
        return std::optional<R>{};
    }
};

/// @brief Trait indicating if T is nothing_t type.
/// @ingroup type_utils
/// @see nothing_t
export template <typename T>
constexpr const bool is_nothing_v =
  std::is_same_v<std::remove_cvref_t<T>, nothing_t>;

export using noopt = std::optional<nothing_t>;

/// @brief Constant of nothing_t type.
/// @ingroup type_utils
/// @see nothing_t
export constexpr nothing_t nothing{};

/// @brief Alias that results in T in non low-profile builds and in nothing_t otherwise.
/// @ingroup type_utils
/// @see nothing_t
export template <typename T>
using not_in_low_profile = std::conditional_t<low_profile_build, nothing_t, T>;
//------------------------------------------------------------------------------
// selector
//------------------------------------------------------------------------------
/// @brief Template used to construct tag-types used mostly in tag-dispatching.
/// @ingroup metaprogramming
/// @see default_selector
/// @see construct_from
export template <identifier_t V>
struct selector {
    static constexpr const identifier_t value = V;
    using type = selector;
};

/// @brief Type alias for the default overload selector type.
/// @ingroup metaprogramming
/// @see default_selector
export using default_selector_t = selector<0U>;

/// @brief The default overload selector constant.
/// @ingroup metaprogramming
/// @see default_selector_t
export constexpr const default_selector_t default_selector{};

/// @brief Type alias for the construct-from tag-dispatch selector.
/// @ingroup metaprogramming
/// @see construct_from
///
/// This type is typically used as an additional parameter in templated
/// constructors to avoid conflicts with copy/move constructors.
export using construct_from_t = selector<0xFU>;

/// @brief The construct-from tag-dispatch constant.
/// @ingroup metaprogramming
/// @see construct_from_t
export constexpr const construct_from_t construct_from{};

/// @brief Type alias for immediate function selector.
/// @ingroup metaprogramming
/// @see immediate_function
///
/// This type is typically used as an additional parameter in consteval
/// function overloads.
export using immediate_function_t = selector<0x1FU>;

/// @brief The use immediate function tag-dispatch constant
/// @ingroup metaprogramming
/// @see immediate_function_t
export constexpr const immediate_function_t immediate_function{};

/// @brief Type alias for split-by function selector.
/// @ingroup metaprogramming
/// @see split_by
///
/// This type is typically used as an additional parameter some string function
/// function overloads.
export using split_by_t = selector<0x58U>;

/// @brief The split-by function tag-dispatch constant
/// @ingroup metaprogramming
/// @see split_by_t
export constexpr const split_by_t split_by{};

/// @brief Type alias for from-pack function selector.
/// @ingroup metaprogramming
/// @see from_pack
///
/// This type is typically used as an additional parameter some string function
/// overloads.
export using from_pack_t = selector<0xF9U>;

/// @brief The from-pack function tag-dispatch constant
/// @ingroup metaprogramming
/// @see from_pack_t
export constexpr const from_pack_t from_pack{};
//------------------------------------------------------------------------------
/// @brief Template for types that serve as identifiers for specific purpose.
export template <identifier_t Tag>
class tagged_id {
public:
    /// @brief Constructs an invalid identifier.
    /// @post not has_value()
    constexpr tagged_id() noexcept = default;

    /// @brief Constructs an identifier with the specified value.
    constexpr tagged_id(identifier_t id) noexcept
      : _id{id} {}

    /// @brief Indicates if this id has a valid value.
    constexpr auto has_value() const noexcept -> bool {
        return _id != 0;
    }

    /// @brief Indicates if this id has a valid value.
    /// @see has_value
    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    /// @brief Indicates if the specified id has a valid value.
    /// @see has_value
    friend constexpr auto is_valid_id(const tagged_id id) noexcept -> bool {
        return id.has_value();
    }

    /// @brief Returns the underlying value.
    constexpr auto value() const noexcept -> identifier_t {
        return _id;
    }

    /// @brief Comparison.
    constexpr auto operator<=>(const tagged_id&) const noexcept = default;

private:
    identifier_t _id{0};
};
//------------------------------------------------------------------------------
export template <identifier_t Tag>
constexpr auto operator<<(std::ostream& out, const tagged_id<Tag> id)
  -> std::ostream& {
    return out << id.value();
}
//------------------------------------------------------------------------------
export template <typename T>
struct string_traits;
//------------------------------------------------------------------------------
} // namespace eagine
