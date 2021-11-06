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
using stream_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
struct LogStreamInfo {
    std::vector<eagine::string_view> args;
    eagine::string_view logIdentity;
    eagine::string_view instanceId;
    eagine::string_view gitBranch;
    eagine::string_view gitHashId;
    eagine::string_view gitVersion;
    eagine::string_view gitDescription;
    eagine::string_view architecture;
    eagine::string_view compilerName;
    int compilerVersionMajor{-1};
    int compilerVersionMinor{-1};
    int compilerVersionPatch{-1};
};
//------------------------------------------------------------------------------
struct LogSourceInfo {
    eagine::string_view displayName;
    eagine::string_view description;
};
//------------------------------------------------------------------------------
struct LogStreamList {
    std::uintmax_t entryUid;
    std::vector<stream_id_t> streamIds;
};
//------------------------------------------------------------------------------
struct LogEntryData {
    std::uintmax_t entryUid;
    stream_id_t streamId;
    std::uint64_t instance;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::string_view format;
    float reltimeSec{-1.F};
    eagine::log_event_severity severity;
    bool isFirst : 1 {false};
    bool isLast : 1 {false};
    bool hasProgress : 1 {false};

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
    short streamCount{0};
    short streamIndex{0};
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

    void beginStream(stream_id_t stream_id, const LogStreamInfo&) noexcept;
    void endStream(stream_id_t stream_id) noexcept;

    auto getStreamInfo(const stream_id_t streamId) noexcept -> LogStreamInfo&;

    auto streamCount() const noexcept -> int {
        if(EAGINE_LIKELY(!_streams.empty())) {
            return eagine::limit_cast<int>(_streams.size());
        }
        return 0;
    }

    auto getStreamInfo(int index) noexcept -> const LogStreamInfo* {
        const auto streamIdx{eagine::convert_if_fits<std::size_t>(index)};
        if(streamIdx) {
            return &_streams[extract(streamIdx)];
        }
        return 0;
    }

    void setDescription(
      stream_id_t stream_id,
      eagine::identifier source_id,
      eagine::logger_instance_id instance_id,
      eagine::string_view display_name,
      eagine::string_view description) noexcept;

    void addEntry(LogEntryData&& entry) noexcept {
        entry.entryUid = ++_uidSequence;
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

private:
    static constexpr auto _chunkSize() noexcept -> std::size_t {
        return 1024;
    }

    void _emplaceNextEntry(LogEntryData&& entry) noexcept;
    auto _getNextStreamList() noexcept -> LogStreamList&;

    std::uintmax_t _uidSequence{0U};
    std::vector<std::vector<LogEntryData>> _entries;
    std::vector<LogStreamList> _streamLists;
    std::map<stream_id_t, LogStreamInfo> _streams;
    std::map<
      std::tuple<stream_id_t, eagine::identifier, eagine::logger_instance_id>,
      LogSourceInfo>
      _sources;
    std::set<std::string, eagine::str_view_less> _str_cache;
};
//------------------------------------------------------------------------------
#endif
