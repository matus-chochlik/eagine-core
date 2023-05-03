/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.console:backend;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Console entry kind enumeration.
/// @ingroup console
export enum class console_entry_kind : std::uint8_t {
    /// @brief Statistic entries.
    stat,
    /// @brief Informational entries.
    info,
    /// @brief Warning entries, indicating potential problems.
    warning,
    /// @brief Error entries, indicating serious problems.
    error
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<console_entry_kind>,
  const Selector) noexcept {
    return enumerator_map_type<console_entry_kind, 4>{
      {{"stat", console_entry_kind::stat},
       {"info", console_entry_kind::info},
       {"warning", console_entry_kind::warning},
       {"error", console_entry_kind::error}}};
}
//------------------------------------------------------------------------------
/// @brief Console entry instance id type.
/// @ingroup console
export using console_entry_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
/// @brief Interface for console backend implementations.
/// @ingroup console
export struct console_backend : interface<console_backend> {
    virtual auto configure(basic_config&) -> bool {
        return false;
    }

    /// @brief The memory allocator used by the console backend.
    virtual auto allocator() noexcept -> memory::shared_byte_allocator = 0;

    /// @brief The backend type identifier.
    virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the actual backend to be used by console_entry.
    /// @param source the identifier of the source object.
    /// @param kind the kind of the entry.
    virtual auto entry_backend(
      const identifier source,
      const console_entry_kind kind) noexcept
      -> std::tuple<optional_reference<console_backend>, console_entry_id_t> = 0;

    /// @brief Begins a new console message.
    /// @param source the identifier of the entry source object.
    /// @param kind the kind of the entry message.
    /// @param format the format string of the message. May contain argument placeholders.
    /// @see finish_message
    /// @see add_separator
    virtual auto begin_message(
      const identifier source,
      const console_entry_id_t parent_id,
      const console_entry_id_t entry_id,
      const console_entry_kind kind,
      const string_view format) noexcept -> bool = 0;

    /// @brief Add argument with no value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    virtual void add_nothing(
      const identifier arg,
      const identifier tag) noexcept = 0;

    /// @brief Add argument with identifier value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept = 0;

    /// @brief Add argument with boolean value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept = 0;

    /// @brief Add argument with signed integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept = 0;

    /// @brief Add argument with unsigned integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept = 0;

    /// @brief Add argument with floating-point value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_float(
      const identifier arg,
      const identifier tag,
      const double value) noexcept = 0;

    /// @brief Add argument with time duration value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept = 0;

    /// @brief Add argument with string value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept = 0;

    /// @brief Add argument with BLOB `value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept = 0;

    /// @brief Prints a message separator.
    /// @see begin_message
    /// @see finish_message
    virtual void add_separator() noexcept = 0;

    /// @brief Finishes the current console message.
    /// @see begin_message
    /// @see add_separator
    virtual void finish_message() noexcept = 0;

    /// @brief Indicates that the entry will be followed by some sub-entries.
    /// @see concluded
    virtual void to_be_continued(
      const console_entry_id_t parent_id,
      const console_entry_id_t id) noexcept = 0;

    /// @brief Indicates that no other sub entries will be appended to previous entry.
    /// @see to_be_continued
    virtual void concluded(const console_entry_id_t) noexcept = 0;
};
//------------------------------------------------------------------------------
// backends
//------------------------------------------------------------------------------
/// @brief Structure holding console customization options.
/// @ingroup console
export struct console_options {
    /// @brief The explicitly-specified backend to be used by the console.
    std::shared_ptr<console_backend> forced_backend{};
};
//------------------------------------------------------------------------------
export auto make_iostream_console_backend(std::istream&, std::ostream&)
  -> std::unique_ptr<console_backend>;
//------------------------------------------------------------------------------
export auto make_null_console_backend() -> std::unique_ptr<console_backend>;
//------------------------------------------------------------------------------
} // namespace eagine

