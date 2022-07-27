/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:structural;

import <utility>;

namespace eagine {

/// @brief Base class for other classes that store their internal state in a structure
export template <typename T>
struct structural_core {
    /// @brief The type of the state structure.
    using structure_type = T;

    constexpr structural_core() noexcept = default;
    constexpr structural_core(structure_type value) noexcept
      : _value{std::move(value)} {}

    static constexpr auto make_structure() noexcept -> structure_type {
        return {};
    }

    constexpr auto get_structure() const noexcept -> const structure_type& {
        return _value;
    }

    constexpr void set_structure(structure_type value) noexcept {
        _value = std::move(value);
    }

private:
    T _value{};
};

} // namespace eagine

