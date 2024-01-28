/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:protected_member;

import std;

namespace eagine {

/// @brief Class holding a single tagged data member.
/// @ingroup type_utils
export template <typename T, typename Selector = std::type_identity<T>>
class protected_member {
public:
    /// @brief Returns a reference to the stored member.
    [[nodiscard]] auto get_the_member(Selector = Selector()) noexcept -> T& {
        return _member;
    }

    /// @brief Returns a const reference to the stored member.
    [[nodiscard]] auto get_the_member(Selector = Selector()) const noexcept
      -> const T& {
        return _member;
    }

protected:
    template <typename... P>
    protected_member(P&&... p)
      : _member(std::forward<P>(p)...) {}

private:
    T _member;
};

/// @brief Returns a reference to the member stored in protected_member.
/// @ingroup type_utils
/// @relates protected_member
export template <typename Selector, typename T>
[[nodiscard]] auto get_member(
  protected_member<T, Selector>& pm,
  Selector selector = Selector()) noexcept -> T& {
    return pm.get_the_member(selector);
}

/// @brief Returns a const reference to the member stored in protected_member.
/// @ingroup type_utils
/// @relates protected_member
export template <typename Selector, typename T>
[[nodiscard]] auto get_member(
  const protected_member<T, Selector>& pm,
  Selector selector = Selector()) noexcept -> const T& {
    return pm.get_the_member(selector);
}

} // namespace eagine
