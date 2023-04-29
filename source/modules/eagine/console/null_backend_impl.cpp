/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.console;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
class null_console_backend : public console_backend {
public:
    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return {};
    }

    auto type_id() noexcept -> identifier final {
        return "Null";
    }

    auto entry_backend(const identifier, const console_entry_kind) noexcept
      -> std::tuple<console_backend*, console_entry_id_t> final {
        return {this, 0};
    }

    auto begin_message(
      const identifier source,
      const console_entry_id_t,
      const console_entry_id_t,
      const console_entry_kind,
      const string_view) noexcept -> bool final {
        return true;
    }

    void add_nothing(const identifier, const identifier) noexcept final {}

    void add_identifier(
      const identifier,
      const identifier,
      const identifier) noexcept final {}

    void add_bool(const identifier, const identifier, const bool) noexcept
      final {}

    void add_integer(
      const identifier,
      const identifier,
      const std::intmax_t) noexcept final {}

    void add_unsigned(
      const identifier,
      const identifier,
      const std::uintmax_t) noexcept final {}

    void add_float(const identifier, const identifier, const double) noexcept
      final {}

    void add_duration(
      const identifier,
      const identifier,
      const std::chrono::duration<float>) noexcept final {}

    void add_string(
      const identifier,
      const identifier,
      const string_view) noexcept final {}

    void add_blob(
      const identifier,
      const identifier,
      const memory::const_block) noexcept final {}

    void add_separator() noexcept final {}

    void finish_message() noexcept final {}

    void to_be_continued(
      const console_entry_id_t,
      const console_entry_id_t) noexcept final {}

    void concluded(const console_entry_id_t id) noexcept final {}
};
//------------------------------------------------------------------------------
auto make_null_console_backend() -> std::unique_ptr<console_backend> {
    return std::make_unique<null_console_backend>();
}
//------------------------------------------------------------------------------
} // namespace eagine
