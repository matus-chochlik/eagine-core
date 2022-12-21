/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:executable_module;

import eagine.core.types;
import eagine.core.memory;
import <memory>;
import <optional>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Enumeration of executable module load options.
/// @ingroup dyn_lib
/// @see module_load_options
export enum class module_load_option : unsigned {
    /// @brief Lazy load.
    load_lazy = 1U << 0U,
    /// @brief Immediate load.
    load_immediate = 1U << 1U
};

/// @brief Bitfield of executable module load options.
/// @ingroup dyn_lib
/// @see module_load_options
export using module_load_options = bitfield<module_load_option>;

export constexpr auto operator|(
  module_load_option l,
  module_load_option r) noexcept -> module_load_options {
    return {l, r};
}
//------------------------------------------------------------------------------
export struct executable_module : interface<executable_module> {
    /// @brief Checks if this module is open.
    /// @see find_object
    /// @see find_function
    virtual auto is_open() const noexcept -> bool = 0;

    /// @brief Tests if this executable module exports the specified symbol.
    /// @see is_open
    /// @see find_object
    /// @see find_function
    virtual auto exports(const string_view name) noexcept -> bool = 0;

    /// @brief Returns a pointer to the exported object with the specifed name.
    /// @see is_open
    /// @see find_function
    /// @see error_message
    virtual auto find_object(const string_view name)
      -> std::optional<void*> = 0;

    /// @brief Returns a pointer to the exported function with the specifed name.
    /// @see is_open
    /// @see find_object
    /// @see error_message
    virtual auto find_function(const string_view name)
      -> std::optional<void (*)()> = 0;

    /// @brief Returns the user-readable error message from the last failed operation.
    virtual auto error_message() const noexcept -> string_view = 0;
};
//------------------------------------------------------------------------------
/// @brief Reference counting handle to dynamically linkable executable module.
/// @ingroup dyn_lib
export class shared_executable_module {
public:
    /// @brief Default constructor.
    /// @post not is_open()
    shared_executable_module() noexcept = default;

    /// @brief Loads the executable module from the current executable.
    /// @see is_open
    shared_executable_module(nothing_t, module_load_options opts) noexcept;

    /// @brief Loads the executable module from the current executable.
    /// @see is_open
    shared_executable_module(nothing_t) noexcept
      : shared_executable_module{nothing, module_load_options{}} {}

    /// @brief Loads the executable module from the specified file.
    /// @see is_open
    shared_executable_module(
      const string_view filename,
      module_load_options opts) noexcept;

    /// @brief Loads the executable module from the specified file.
    /// @see is_open
    shared_executable_module(const string_view filename) noexcept
      : shared_executable_module{filename, module_load_options{}} {}

    /// @brief Checks if the shared module is open.
    /// @see find_object
    /// @see find_function
    /// @see exports
    auto is_open() const noexcept -> bool {
        if(_module) {
            return _module->is_open();
        }
        return false;
    }

    /// @brief Indicates if the shared module is open.
    /// @see is_open
    explicit operator bool() const noexcept {
        return is_open();
    }

    /// @brief Tests if this executable module exports the specified symbol.
    /// @see is_open
    /// @see find_object
    /// @see find_function
    auto exports(const string_view name) const noexcept -> bool {
        if(is_open()) {
            return _module->exports(name);
        }
        return false;
    }

    /// @brief Returns a pointer to the exported object with the specifed name.
    /// @see is_open
    /// @see find_function
    /// @see error_message
    auto find_object(const string_view name) -> std::optional<void*> {
        if(_module) {
            return _module->find_object(name);
        }
        return {};
    }

    /// @brief Returns a pointer to the exported function with the specifed name.
    /// @see is_open
    /// @see find_object
    /// @see error_message
    template <typename RV, typename... P>
    auto find_function(const string_view name, std::type_identity<RV(P...)>)
      -> std::optional<RV (*)(P...)> {
        if(_module) {
            if(auto found{_module->find_function(name)}) {
                return {reinterpret_cast<RV (*)(P...)>(*found)};
            }
        }
        return {};
    }

    /// @brief Returns the user-readable error message from the last failed operation.
    auto error_message() const noexcept -> string_view {
        if(_module) {
            return _module->error_message();
        }
        return {};
    }

private:
    std::shared_ptr<executable_module> _module;
};
//------------------------------------------------------------------------------
} // namespace eagine
