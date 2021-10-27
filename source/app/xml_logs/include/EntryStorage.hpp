///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_STORAGE_HPP
#define EAGINE_XML_LOGS_ENTRY_STORAGE_HPP

#include <eagine/flat_map.hpp>
#include <eagine/identifier.hpp>
#include <eagine/is_within_limits.hpp>
#include <eagine/logging/fwd.hpp>
#include <eagine/logging/severity.hpp>
#include <eagine/message_id.hpp>
#include <eagine/string_span.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <variant>

class Backend;
//------------------------------------------------------------------------------
struct LogSourceInfo {
    eagine::string_view display_name;
    eagine::string_view description;
};
//------------------------------------------------------------------------------
struct LogStreamList {
    std::uintmax_t entry_uid;
    std::vector<std::uintptr_t> stream_ids;
};
//------------------------------------------------------------------------------
struct LogEntryData {
    std::uintmax_t entry_uid;
    std::uintptr_t stream_id;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::logger_instance_id instance;
    eagine::log_event_severity severity;
    eagine::string_view format;

    using argument_value_type = std::variant<
      eagine::nothing_t,
      eagine::identifier,
      eagine::message_id,
      bool,
      std::intmax_t,
      std::uintmax_t,
      float,
      std::tuple<float, float, float>,
      std::chrono::duration<float>,
      eagine::string_view>;

    eagine::flat_map<
      eagine::identifier,
      std::tuple<eagine::identifier, argument_value_type>>
      args;
};
//------------------------------------------------------------------------------
struct LogEntryConnectors {
    short stream_count{0};
    short stream_index{0};
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
      std::uintptr_t stream_id,
      eagine::identifier source,
      eagine::logger_instance_id instance,
      eagine::string_view display_name,
      eagine::string_view description) {
        auto& info = _sources[{stream_id, source, instance}];
        info.display_name = cacheString(display_name);
        info.description = cacheString(description);
    }

    void beginStream(std::uintptr_t id) {
        auto& info = _getNextStreamList();
        info.stream_ids.push_back(id);
    }

    void endStream(std::uintptr_t id) {
        auto& info = _getNextStreamList();
        const auto pos =
          std::find(info.stream_ids.begin(), info.stream_ids.end(), id);
        if(EAGINE_LIKELY(pos != info.stream_ids.end())) {
            info.stream_ids.erase(pos);
        }
    }

    void addEntry(LogEntryData& entry) {
        entry.entry_uid = ++_uid_sequence;
        _entries.emplace_back(std::move(entry));
    }

    auto entryCount() const noexcept -> int {
        return eagine::limit_cast<int>(_entries.size());
    }

    auto getEntry(int index) noexcept -> LogEntryData* {
        if(auto entIdx{eagine::convert_if_fits<std::uintptr_t>(index)}) {
            return &_entries[extract(entIdx)];
        }
        return nullptr;
    }

    auto getEntryConnectors(const LogEntryData& entry) noexcept
      -> LogEntryConnectors {
        LogEntryConnectors result;
        const auto pos = std::lower_bound(
          _streams.begin(),
          _streams.end(),
          entry.entry_uid,
          [](const auto& current, const auto entry_uid) {
              return entry_uid < current.entry_uid;
          });
        if(pos != _streams.end()) {
            const auto& si = pos->stream_ids;
            result.stream_count = eagine::limit_cast<short>(si.size());
            result.stream_index = eagine::limit_cast<short>(std::distance(
              si.begin(),
              std::find_if(
                si.begin(), si.end(), [&entry](const auto stream_id) {
                    return stream_id == entry.stream_id;
                })));
        }
        return result;
    }

private:
    auto _getNextStreamList() -> LogStreamList& {
        if(EAGINE_LIKELY(!_streams.empty())) {
            _streams.push_back(_streams.back());
        } else {
            _streams.emplace_back();
        }
        auto& info = _streams.back();
        info.entry_uid = _uid_sequence;
        return info;
    }

    std::uintmax_t _uid_sequence{0U};
    std::vector<LogEntryData> _entries;
    std::vector<LogStreamList> _streams;
    std::map<
      std::tuple<std::uintptr_t, eagine::identifier, eagine::logger_instance_id>,
      LogSourceInfo>
      _sources;
    std::set<std::string, eagine::str_view_less> _str_cache;
};
//------------------------------------------------------------------------------
#endif
