///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_FORMAT_HPP
#define EAGINE_XML_LOGS_ENTRY_FORMAT_HPP

import eagine.core;
import <optional>;
import <chrono>;

//------------------------------------------------------------------------------
struct LogEntryData;
class EntryFormat {
public:
    using result_type = std::optional<eagine::string_view>;

    auto format(const LogEntryData&) noexcept -> eagine::string_view;

    auto operator()(const eagine::identifier, const eagine::nothing_t) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const eagine::identifier) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const eagine::message_id&) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const bool) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const std::intmax_t) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const std::uintmax_t) noexcept
      -> result_type;
    auto operator()(const eagine::identifier, const float) noexcept
      -> result_type;
    auto operator()(
      const eagine::identifier,
      const std::tuple<float, float, float>&) noexcept -> result_type;
    auto operator()(
      const eagine::identifier,
      const std::chrono::duration<float>) noexcept -> result_type;
    auto operator()(const eagine::identifier, const eagine::string_view) noexcept
      -> result_type;

private:
    std::string _msg;
    std::string _arg;
};
//------------------------------------------------------------------------------

#endif // EAGINE_XML_LOGS_ENTRY_FORMAT_HPP

