/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:file_contents;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
export auto read_stream_data(std::istream&, memory::buffer& dest) noexcept
  -> bool;
export auto read_file_data(const string_view path, memory::buffer& dest) noexcept
  -> bool;
//------------------------------------------------------------------------------
/// @brief Interface for file content getter implementations.
/// @see file_contents
export struct file_contents_intf : interface<file_contents_intf> {
    virtual auto block() noexcept -> memory::const_block = 0;
};
//------------------------------------------------------------------------------
/// @brief Class providing access to the contents of a file.
/// @see structured_file_content
export class file_contents {
public:
    /// @brief Default constructor.
    file_contents() noexcept = default;

    /// @brief Constructor that opens and loads contents of file at the given path.
    file_contents(const string_view path);

    /// @brief Checks if the contents were loaded.
    /// @see block
    auto is_loaded() const noexcept -> bool {
        return bool(_impl);
    }

    /// @brief Checks if the contents were loaded.
    /// @see is_loaded
    explicit operator bool() const noexcept {
        return is_loaded();
    }

    /// @brief Returns the block viewing the loaded file contents.
    /// @see is_loaded
    auto block() const noexcept -> memory::const_block {
        return _impl.member(&file_contents_intf::block)
          .value_or(memory::const_block{});
    }

    /// @brief Implicit conversion to the block viewing the loaded file contents.
    /// @see block
    /// @see is_loaded
    operator memory::const_block() const noexcept {
        return block();
    }

private:
    shared_holder<file_contents_intf> _impl{};
};
//------------------------------------------------------------------------------
/// @brief Class loading a baked structured data from a file.
/// @see file_contents
export template <typename T>
class structured_file_content
  : protected_member<file_contents>
  , public memory::structured_block<const T> {
public:
    /// @brief Constructor that opens and loads contents of file at the given path.
    structured_file_content(const string_view path)
      : protected_member<file_contents>(path)
      , memory::structured_block<const T>(get_the_member()) {}

    /// @brief Construction from a file contents instance.
    /// @see file_contents
    structured_file_content(file_contents&& fc)
      : protected_member<file_contents>(std::move(fc))
      , memory::structured_block<const T>(get_the_member()) {}
};
//------------------------------------------------------------------------------
} // namespace eagine

