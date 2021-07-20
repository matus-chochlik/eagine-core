///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_STORAGE_HPP
#define EAGINE_XML_LOGS_ENTRY_STORAGE_HPP

#include <eagine/identifier.hpp>
#include <eagine/is_within_limits.hpp>
#include <eagine/logging/fwd.hpp>
#include <eagine/logging/severity.hpp>
#include <eagine/string_span.hpp>
#include <map>
#include <set>
#include <string>

class Backend;
//------------------------------------------------------------------------------
struct LogSourceInfo {
    eagine::string_view display_name;
    eagine::string_view description;
};
//------------------------------------------------------------------------------
struct LogEntryData {
    std::size_t stream_id;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::logger_instance_id instance;
    eagine::log_event_severity severity;
    eagine::string_view format;

    std::map<
      eagine::identifier,
      std::tuple<eagine::identifier, eagine::string_view>>
      args_str;
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

    void setDescription(
      std::size_t stream_id,
      eagine::identifier source,
      eagine::logger_instance_id instance,
      eagine::string_view display_name,
      eagine::string_view description) {
        auto& info = _sources[{stream_id, source, instance}];
        info.display_name = cacheString(display_name);
        info.description = cacheString(description);
    }

    void addEntry(LogEntryData& entry) {
        _entries.emplace_back(std::move(entry));
    }

    auto entryCount() const noexcept -> int {
        return eagine::limit_cast<int>(_entries.size());
    }

    auto getEntry(int index) noexcept -> LogEntryData* {
        if(auto entIdx{eagine::convert_if_fits<std::size_t>(index)}) {
            return &_entries[extract(entIdx)];
        }
        return nullptr;
    }

private:
    std::vector<LogEntryData> _entries;
    std::map<
      std::tuple<std::size_t, eagine::identifier, eagine::logger_instance_id>,
      LogSourceInfo>
      _sources;
    std::set<std::string, eagine::str_view_less> _str_cache;
};
//------------------------------------------------------------------------------
#endif
