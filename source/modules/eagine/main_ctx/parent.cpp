/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx:parent;
import :interface;
import <variant>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Helper class used to initialize main context objects.
/// @ingroup main_context
/// @see main_ctx_object
export class main_ctx_object_parent_info {
public:
    /// @brief Construction from main_ctx instance.
    main_ctx_object_parent_info(main_ctx_getters& ctx) noexcept
      : _parent{&ctx} {}

    /// @brief Construction from main context object.
    main_ctx_object_parent_info(const main_ctx_object& obj) noexcept
      : _parent{&obj} {}

    /// @brief Indicates if a pointer to the context object is stored
    /// @see context
    /// @see has_object
    auto has_context() const noexcept -> bool {
        return std::holds_alternative<main_ctx_getters*>(_parent);
    }

    /// @brief Returns a pointer to the context object.
    /// @pre has_context
    auto context() const noexcept -> main_ctx_getters& {
        assert(has_context());
        return *std::get<main_ctx_getters*>(_parent);
    }

    /// @brief Indicates if a pointer to the parent object.
    /// @see object
    /// @see has_context
    auto has_object() const noexcept -> bool {
        return std::holds_alternative<const main_ctx_object*>(_parent);
    }

    /// @brief Returns a pointer to the parent object.
    /// @pre has_object
    auto object() const noexcept -> const main_ctx_object& {
        assert(has_object());
        return *std::get<const main_ctx_object*>(_parent);
    }

protected:
    main_ctx_object_parent_info() noexcept = default;

private:
    std::variant<main_ctx_getters*, const main_ctx_object*> _parent;
};

/// @brief Alias for main_ctx_object_parent_info parameter type.
/// @ingroup main_context
export using main_ctx_parent = const main_ctx_object_parent_info&;
//------------------------------------------------------------------------------
} // namespace eagine
