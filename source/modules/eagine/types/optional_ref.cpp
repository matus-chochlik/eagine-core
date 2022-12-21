/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cassert>

export module eagine.core.types:optional_ref;

import eagine.core.concepts;
import :nothing;
import :tribool;
import <cstddef>;
import <utility>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Optional reference to an instance of type @p T.
/// @ingroup valid_if
/// @see valid_if
export template <typename T>
class optional_reference_wrapper {
public:
    /// @brief Construction from a reference to value of type @p T.
    optional_reference_wrapper(T& ref) noexcept
      : _ptr{&ref} {}

    /// @brief Move constructor.
    optional_reference_wrapper(optional_reference_wrapper&&) noexcept = default;

    /// @brief Copy constructor.
    optional_reference_wrapper(const optional_reference_wrapper&) = default;

    /// @brief Move assignment operator.
    auto operator=(optional_reference_wrapper&&) noexcept
      -> optional_reference_wrapper& = default;

    /// @brief Copy assignment operator.
    auto operator=(const optional_reference_wrapper&)
      -> optional_reference_wrapper& = default;

    ~optional_reference_wrapper() noexcept = default;

    /// @brief Construction from nothing.
    /// @post not is_valid()
    constexpr optional_reference_wrapper(nothing_t) noexcept {}

    /// @brief Construction from nullptr.
    /// @post not is_valid()
    constexpr optional_reference_wrapper(std::nullptr_t) noexcept {}

    /// @brief Indicates if this stores a valid reference.
    [[nodiscard]] auto is_valid() const noexcept -> bool {
        return _ptr != nullptr;
    }

    /// @brief Indicates if this stores a valid reference.
    /// @see is_valid
    [[nodiscard]] explicit operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Returns the stored reference.
    /// @pre is_valid()
    [[nodiscard]] auto get() const noexcept -> T& {
        assert(is_valid());
        return *_ptr;
    }

    /// @brief Returns the stored value.
    /// @pre is_valid()
    [[nodiscard]] auto value() const noexcept -> const T& {
        assert(is_valid());
        return *_ptr;
    }

    /// @brief Returns the stored value if valid or @p fallback otherwise.
    /// @see is_valid
    template <typename U>
    [[nodiscard]] auto value_or(U&& fallback) const noexcept -> T
        requires(std::is_convertible_v<U, T>)
    {
        if(is_valid()) {
            return *_ptr;
        }
        return T(std::forward<U>(fallback));
    }

    /// @brief Returns the stored reference.
    /// @see get
    [[nodiscard]] explicit operator T&() const noexcept {
        return get();
    }

    /// @brief Tri-state equality comparison of the referred instance with a value.
    [[nodiscard]] auto operator==(const T& r) const noexcept -> tribool {
        if(is_valid()) {
            return value() == r;
        }
        return indeterminate;
    }

    /// @brief Tri-state nonequality comparison of the referred instance with a value.
    [[nodiscard]] auto operator!=(const T& r) const noexcept -> tribool {
        if(is_valid()) {
            return value() != r;
        }
        return indeterminate;
    }

private:
    T* _ptr{nullptr};
};
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits<optional_reference_wrapper<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Overload of extract for optional_reference_wrapper.
/// @ingroup valid_if
export template <typename T>
[[nodiscard]] auto extract(optional_reference_wrapper<T> ref) noexcept -> T& {
    return ref.get();
}
//------------------------------------------------------------------------------
} // namespace eagine
