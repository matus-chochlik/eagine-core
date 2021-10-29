///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_STORAGE_HPP
#define EAGINE_XML_LOGS_ENTRY_STORAGE_HPP

#include <eagine/branch_predict.hpp>
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
struct LogStreamInfo {
    eagine::string_view log_identity;
};
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
    std::uint64_t instance;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::string_view format;
    float reltime_sec{-1.F};
    eagine::log_event_severity severity;
    bool is_first : 1 {false};
    bool is_last : 1 {false};
    bool has_progress : 1 {false};

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
      eagine::identifier source_id,
      eagine::logger_instance_id instance_id,
      eagine::string_view display_name,
      eagine::string_view description) noexcept;

    void beginStream(std::uintptr_t stream_id, const LogStreamInfo&) noexcept;
    void endStream(std::uintptr_t stream_id) noexcept;

    void addEntry(LogEntryData&& entry) noexcept {
        entry.entry_uid = ++_uid_sequence;
        _emplaceNextEntry(std::move(entry));
    }

    auto entryCount() const noexcept -> int {
        if(EAGINE_LIKELY(!_entries.empty())) {
            return eagine::limit_cast<int>(
              (_entries.size() - 1U) * _chunkSize() + _entries.back().size());
        }
        return 0;
    }

    auto getEntry(int index) noexcept -> LogEntryData* {
        if(EAGINE_LIKELY(!_entries.empty())) {
            const auto entIdx{eagine::convert_if_fits<std::size_t>(index)};
            if(EAGINE_LIKELY(entIdx)) {
                const auto chunkIdx{extract(entIdx) / _chunkSize()};
                if(EAGINE_LIKELY(chunkIdx < _entries.size())) {
                    return &_entries[chunkIdx][extract(entIdx) % _chunkSize()];
                }
            }
        }
        return nullptr;
    }

    auto getEntryConnectors(const LogEntryData& entry) noexcept
      -> LogEntryConnectors;

    auto getStreamInfo(const LogEntryData& entry) noexcept
      -> const LogStreamInfo&;

private:
    static constexpr auto _chunkSize() noexcept -> std::size_t {
        return 1024;
    }

    void _emplaceNextEntry(LogEntryData&& entry) noexcept;
    auto _getNextStreamList() noexcept -> LogStreamList&;

    std::uintmax_t _uid_sequence{0U};
    std::vector<std::vector<LogEntryData>> _entries;
    std::vector<LogStreamList> _streamLists;
    std::map<std::uintptr_t, LogStreamInfo> _streams;
    std::map<
      std::tuple<std::uintptr_t, eagine::identifier, eagine::logger_instance_id>,
      LogSourceInfo>
      _sources;
    std::set<std::string, eagine::str_view_less> _str_cache;
};
//------------------------------------------------------------------------------
#endif
