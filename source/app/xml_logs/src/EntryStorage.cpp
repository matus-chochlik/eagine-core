///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryStorage.hpp"

//------------------------------------------------------------------------------
void LogEntryStorage::_emplaceNextEntry(LogEntryData&& entry) noexcept {
    auto& chunk = [&]() -> std::vector<LogEntryData>& {
        if(EAGINE_LIKELY(!_entries.empty())) {
            if(EAGINE_UNLIKELY(_entries.back().size() >= _chunkSize())) {
                EAGINE_ASSERT(_entries.back().size() == _chunkSize());
                _entries.emplace_back();
                _entries.back().reserve(_chunkSize());
            }
        } else {
            _entries.emplace_back();
            _entries.back().reserve(_chunkSize());
        }
        return _entries.back();
    }();
    chunk.emplace_back(std::move(entry));
}
//------------------------------------------------------------------------------
auto LogEntryStorage::_getNextStreamList() noexcept -> LogStreamList& {
    if(EAGINE_LIKELY(!_streamLists.empty())) {
        _streamLists.push_back(_streamLists.back());
    } else {
        _streamLists.emplace_back();
    }
    auto& info = _streamLists.back();
    return info;
}
//------------------------------------------------------------------------------
void LogEntryStorage::setDescription(
  std::uintptr_t stream_id,
  eagine::identifier source_id,
  eagine::logger_instance_id instance_id,
  eagine::string_view display_name,
  eagine::string_view description) noexcept {
    auto& sourceInfo = _sources[{stream_id, source_id, instance_id}];
    sourceInfo.display_name = display_name;
    sourceInfo.description = description;
}
//------------------------------------------------------------------------------
void LogEntryStorage::beginStream(
  std::uintptr_t stream_id,
  const LogStreamInfo& info) noexcept {
    _streams[stream_id] = info;
    //
    auto& streamList = _getNextStreamList();
    streamList.entry_uid = ++_uid_sequence;
    streamList.stream_ids.push_back(stream_id);
    //
    LogEntryData entry{};
    entry.entry_uid = _uid_sequence;
    entry.stream_id = stream_id;
    entry.source = EAGINE_ID(XmlLogView);
    entry.severity = eagine::log_event_severity::info;
    entry.reltime_sec = 0.F;
    entry.is_first = true;
    entry.format = "Log started";
    _emplaceNextEntry(std::move(entry));
}
//------------------------------------------------------------------------------
void LogEntryStorage::endStream(std::uintptr_t stream_id) noexcept {
    LogEntryData entry{};
    entry.entry_uid = ++_uid_sequence;
    entry.stream_id = stream_id;
    entry.source = EAGINE_ID(XmlLogView);
    entry.severity = eagine::log_event_severity::info;
    entry.is_last = true;
    entry.format = "Log finished";
    _emplaceNextEntry(std::move(entry));
    //
    auto& info = _getNextStreamList();
    info.entry_uid = ++_uid_sequence;
    const auto pos =
      std::find(info.stream_ids.begin(), info.stream_ids.end(), stream_id);
    if(EAGINE_LIKELY(pos != info.stream_ids.end())) {
        info.stream_ids.erase(pos);
    }
}
//------------------------------------------------------------------------------
auto LogEntryStorage::getEntryConnectors(const LogEntryData& entry) noexcept
  -> LogEntryConnectors {
    LogEntryConnectors result;
    const auto pos = std::lower_bound(
      _streamLists.rbegin(),
      _streamLists.rend(),
      entry.entry_uid,
      [](const auto& current, const auto entry_uid) {
          return entry_uid < current.entry_uid;
      });
    if(pos != _streamLists.rend()) {
        const auto& si = pos->stream_ids;
        result.stream_count = eagine::limit_cast<short>(si.size());
        result.stream_index = eagine::limit_cast<short>(std::distance(
          si.begin(),
          std::find_if(si.begin(), si.end(), [&entry](const auto stream_id) {
              return stream_id == entry.stream_id;
          })));
    }
    return result;
}
//------------------------------------------------------------------------------
auto LogEntryStorage::getStreamInfo(const std::uintptr_t streamId) noexcept
  -> const LogStreamInfo& {
    return _streams[streamId];
}
//------------------------------------------------------------------------------
