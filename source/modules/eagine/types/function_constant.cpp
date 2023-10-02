/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:function_constant;

import std;
import :basic;

namespace eagine {

/// @brief Declaration of compile-time member function pointer wrappers.
/// @ingroup functional
export template <typename T, T Ptr>
struct member_function_constant;
//------------------------------------------------------------------------------
/// @brief Implementation of compile-time member function pointer wrappers.
/// @ingroup functional
export template <typename RV, typename C, typename... P, RV (C::*Ptr)(P...)>
struct member_function_constant<RV (C::*)(P...), Ptr> {
    using type = member_function_constant;

    /// @brief Alias for the member function pointer type.
    using pointer = RV (C::*)(P...);

    /// @brief Alias for corresponding free function pointer type.
    using free_pointer = RV (*)(C*, P...);

    /// @brief Alias for the function result type.
    using result_type = RV;

    /// @brief Function parameter count.
    using arity = size_constant<sizeof...(P)>;

    /// @brief Alias for the class to which the member function belongs.
    using scope = C;

    using callee = C;

    /// @brief Alias for type indicating if the member function is const.
    using is_const = std::false_type;

    /// @brief Alias for type indicating if the member function is noexcept.
    using is_noexcept = std::false_type;

    /// @brief Returns the member function pointer.
    /// @see free_func
    /// @see invoke
    [[nodiscard]] static constexpr auto get() noexcept -> pointer {
        return Ptr;
    }

    /// @brief A function that calls the member function on an instance of @p C.
    /// @see make_free
    /// @see invoke
    static auto free_func(C* c, P... a) -> RV {
        return (c->*Ptr)(std::forward<P>(a)...);
    }

    /// @brief Returns pointer to a free function corresponding to the member function.
    /// @see get
    /// @see free_func
    /// @see invoke
    [[nodiscard]] static auto make_free() noexcept -> free_pointer {
        return &free_func;
    }

    /// @brief Invokes the referenced member function on the given object with args.
    template <typename... Args>
    static constexpr auto invoke(C& c, Args&&... args) -> RV {
        return (c.*Ptr)(std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
export template <typename RV, typename C, typename... P, RV (C::*Ptr)(P...) const>
struct member_function_constant<RV (C::*)(P...) const, Ptr> {
    using type = member_function_constant;

    /// @brief Alias for the member function pointer type.
    using pointer = RV (C::*)(P...) const;

    /// @brief Alias for corresponding free function pointer type.
    using free_pointer = RV (*)(const C*, P...);

    /// @brief Alias for the function result type.
    using result_type = RV;

    /// @brief Function parameter count.
    using arity = size_constant<sizeof...(P)>;

    /// @brief Alias for the class to which the member function belongs.
    using scope = C;

    using callee = const C;

    /// @brief Alias for type indicating if the member function is const.
    using is_const = std::true_type;

    /// @brief Alias for type indicating if the member function is noexcept.
    using is_noexcept = std::false_type;

    /// @brief Returns the member function pointer.
    /// @see free_func
    [[nodiscard]] static constexpr auto get() noexcept -> pointer {
        return Ptr;
    }

    /// @brief A function that calls the member function on an instance of @p C.
    /// @see make_free
    [[nodiscard]] static auto free_func(const C* c, P... a) -> RV {
        return (c->*Ptr)(std::forward<P>(a)...);
    }

    /// @brief Returns pointer to a free function corresponding to the member function.
    /// @see get
    /// @see free_func
    [[nodiscard]] static auto make_free() noexcept -> free_pointer {
        return &free_func;
    }

    /// @brief Invokes the referenced member function on the given object with args.
    template <typename... Args>
    static constexpr auto invoke(const C& c, Args&&... args) -> RV {
        return (c.*Ptr)(std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
export template <typename RV, typename C, typename... P, RV (C::*Ptr)(P...) noexcept>
struct member_function_constant<RV (C::*)(P...) noexcept, Ptr> {
    using type = member_function_constant;

    /// @brief Alias for the member function pointer type.
    using pointer = RV (C::*)(P...) noexcept;

    /// @brief Alias for corresponding free function pointer type.
    using free_pointer = RV (*)(C*, P...) noexcept;

    /// @brief Alias for the function result type.
    using result_type = RV;

    /// @brief Function parameter count.
    using arity = size_constant<sizeof...(P)>;

    /// @brief Alias for the class to which the member function belongs.
    using scope = C;

    using callee = C;

    /// @brief Alias for type indicating if the member function is const.
    using is_const = std::false_type;

    /// @brief Alias for type indicating if the member function is noexcept.
    using is_noexcept = std::true_type;

    /// @brief Returns the member function pointer.
    /// @see free_func
    [[nodiscard]] static constexpr auto get() noexcept -> pointer {
        return Ptr;
    }

    /// @brief A function that calls the member function on an instance of @p C.
    /// @see make_free
    [[nodiscard]] static auto free_func(C* c, P... a) noexcept -> RV {
        return (c->*Ptr)(std::forward<P>(a)...);
    }

    /// @brief Returns pointer to a free function corresponding to the member function.
    /// @see get
    /// @see free_func
    [[nodiscard]] static auto make_free() noexcept -> free_pointer {
        return &free_func;
    }

    /// @brief Invokes the referenced member function on the given object with args.
    template <typename... Args>
    static constexpr auto invoke(C& c, Args&&... args) noexcept -> RV {
        return (c.*Ptr)(std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
export template <
  typename RV,
  typename C,
  typename... P,
  RV (C::*Ptr)(P...) const noexcept>
struct member_function_constant<RV (C::*)(P...) const noexcept, Ptr> {
    using type = member_function_constant;

    /// @brief Alias for the member function pointer type.
    using pointer = RV (C::*)(P...) const noexcept;

    /// @brief Alias for corresponding free function pointer type.
    using free_pointer = RV (*)(const C*, P...) noexcept;

    /// @brief Alias for the function result type.
    using result_type = RV;

    /// @brief Function parameter count.
    using arity = size_constant<sizeof...(P)>;

    /// @brief Alias for the class to which the member function belongs.
    using scope = C;

    using callee = const C;

    /// @brief Alias for type indicating if the member function is const.
    using is_const = std::true_type;

    /// @brief Alias for type indicating if the member function is noexcept.
    using is_noexcept = std::true_type;

    /// @brief Returns the member function pointer.
    /// @see free_func
    [[nodiscard]] static constexpr auto get() noexcept -> pointer {
        return Ptr;
    }

    /// @brief A function that calls the member function on an instance of @p C.
    /// @see make_free
    [[nodiscard]] static auto free_func(const C* c, P... a) noexcept -> RV {
        return (c->*Ptr)(std::forward<P>(a)...);
    }

    /// @brief Returns pointer to a free function corresponding to the member function.
    /// @see get
    /// @see free_func
    [[nodiscard]] static auto make_free() noexcept -> free_pointer {
        return &free_func;
    }

    /// @brief Invokes the referenced member function on the given object with args.
    template <typename... Args>
    static constexpr auto invoke(const C& c, Args&&... args) noexcept -> RV {
        return (c.*Ptr)(std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
/// @brief Helper for instantiating the member_function_constant template.
/// @ingroup functional
/// @see is_member_function_constant_v
export template <auto C>
using member_function_constant_t = member_function_constant<decltype(C), C>;

export template <typename X>
struct is_member_function_constant : std::false_type {};

export template <auto C>
struct is_member_function_constant<member_function_constant<decltype(C), C>>
  : std::true_type {};

/// @brief Trait indicating if a X is an member_function_constant
/// @ingroup functional
/// @see is_member_function_constant_v
/// @see member_function_constant
export template <typename X>
constexpr const bool is_member_function_constant_v =
  is_member_function_constant<X>::value;
//------------------------------------------------------------------------------
} // namespace eagine

