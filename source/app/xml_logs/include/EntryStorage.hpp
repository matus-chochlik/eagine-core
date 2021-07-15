///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_STORAGE_HPP
#define EAGINE_XML_LOGS_ENTRY_STORAGE_HPP

#include <eagine/identifier.hpp>
#include <eagine/logging/fwd.hpp>
#include <eagine/logging/severity.hpp>
#include <eagine/string_span.hpp>
#include <map>
#include <set>
#include <string>

class Backend;
//------------------------------------------------------------------------------
struct LogStreamInfo {
    std::string display_name;
    std::string description;
};
//------------------------------------------------------------------------------
struct LogEntryData {
    std::size_t stream_id;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::logger_instance_id instance;
    eagine::log_event_severity severity;
};
//------------------------------------------------------------------------------
class LogEntryStorage {
public:
    auto cacheString(eagine::string_view s) -> eagine::string_view {
        auto pos = _str_cache.find(s);
        if(pos == _str_cache.end()) {
            pos = _str_cache.emplace(to_string(s)).first;
        }
        return {*pos};
    }

    void addEntry(LogEntryData entry) {
        _entries.emplace_back(std::move(entry));
    }

private:
    std::set<std::string, eagine::str_view_less> _str_cache;
    std::vector<LogEntryData> _entries;
};
//------------------------------------------------------------------------------
#endif
