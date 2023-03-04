///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_STORAGE_HPP
#define EAGINE_XML_LOGS_ENTRY_STORAGE_HPP

import eagine.core;
import std;

class Backend;
//------------------------------------------------------------------------------
using stream_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
struct LogStreamInfo {
    std::vector<eagine::string_view> args;
    eagine::string_view logIdentity;
    eagine::string_view instanceId;
    eagine::string_view osName;
    eagine::string_view osCodeName;
    eagine::string_view gitBranch;
    eagine::string_view gitHashId;
    eagine::string_view gitVersion;
    eagine::string_view gitDescribe;
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
    auto cacheString(eagine::string_view s) -> eagine::string_view;

    void beginStream(stream_id_t stream_id, const LogStreamInfo&) noexcept;
    void endStream(stream_id_t stream_id) noexcept;
    auto streamInfoRef(const stream_id_t streamId) noexcept -> LogStreamInfo&;

    auto streamCount() const noexcept -> int;
    auto getStreamInfo(int index) noexcept -> LogStreamInfo*;

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

    auto entryCount() const noexcept -> int;
    auto getEntry(int index) noexcept -> LogEntryData*;

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
