///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_FORMAT_HPP
#define EAGINE_XML_LOGS_ENTRY_FORMAT_HPP

#include <eagine/message_id.hpp>
#include <eagine/string_span.hpp>
#include <eagine/valid_if/decl.hpp>
#include <chrono>

//------------------------------------------------------------------------------
struct LogEntryData;
class EntryFormat {
public:
    using result_type = eagine::optionally_valid<eagine::string_view>;

    auto format(const LogEntryData&) noexcept -> eagine::string_view;

    auto operator()(const eagine::nothing_t) noexcept -> result_type;
    auto operator()(const eagine::identifier) noexcept -> result_type;
    auto operator()(const eagine::message_id&) noexcept -> result_type;
    auto operator()(const bool) noexcept -> result_type;
    auto operator()(const std::intmax_t) noexcept -> result_type;
    auto operator()(const std::uintmax_t) noexcept -> result_type;
    auto operator()(const float) noexcept -> result_type;
    auto operator()(const std::tuple<float, float, float>&) noexcept
      -> result_type;
    auto operator()(const std::chrono::duration<float>) noexcept -> result_type;
    auto operator()(const eagine::string_view) noexcept -> result_type;

private:
    std::string _msg;
    std::string _arg;
};
//------------------------------------------------------------------------------

#endif // EAGINE_XML_LOGS_ENTRY_FORMAT_HPP

