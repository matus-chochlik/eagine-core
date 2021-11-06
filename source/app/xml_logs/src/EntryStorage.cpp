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
  stream_id_t streamId,
  eagine::identifier sourceId,
  eagine::logger_instance_id instanceId,
  eagine::string_view displayName,
  eagine::string_view description) noexcept {
    auto& sourceInfo = _sources[{streamId, sourceId, instanceId}];
    sourceInfo.displayName = displayName;
    sourceInfo.description = description;
}
//------------------------------------------------------------------------------
void LogEntryStorage::beginStream(
  stream_id_t streamId,
  const LogStreamInfo& info) noexcept {
    _streams[streamId] = info;
    //
    auto& streamList = _getNextStreamList();
    streamList.entryUid = ++_uidSequence;
    streamList.streamIds.push_back(streamId);
    //
    LogEntryData entry{};
    entry.entryUid = _uidSequence;
    entry.streamId = streamId;
    entry.source = EAGINE_ID(XmlLogView);
    entry.severity = eagine::log_event_severity::info;
    entry.reltimeSec = 0.F;
    entry.isFirst = true;
    entry.format = "Log started";
    _emplaceNextEntry(std::move(entry));
}
//------------------------------------------------------------------------------
void LogEntryStorage::endStream(stream_id_t streamId) noexcept {
    LogEntryData entry{};
    entry.entryUid = ++_uidSequence;
    entry.streamId = streamId;
    entry.source = EAGINE_ID(XmlLogView);
    entry.severity = eagine::log_event_severity::info;
    entry.isLast = true;
    entry.format = "Log finished";
    _emplaceNextEntry(std::move(entry));
    //
    auto& info = _getNextStreamList();
    info.entryUid = ++_uidSequence;
    const auto pos =
      std::find(info.streamIds.begin(), info.streamIds.end(), streamId);
    if(EAGINE_LIKELY(pos != info.streamIds.end())) {
        info.streamIds.erase(pos);
    }
}
//------------------------------------------------------------------------------
auto LogEntryStorage::getStreamInfo(const stream_id_t streamId) noexcept
  -> LogStreamInfo& {
    return _streams[streamId];
}
//------------------------------------------------------------------------------
auto LogEntryStorage::getEntryConnectors(const LogEntryData& entry) noexcept
  -> LogEntryConnectors {
    LogEntryConnectors result;
    const auto pos = std::lower_bound(
      _streamLists.rbegin(),
      _streamLists.rend(),
      entry.entryUid,
      [](const auto& current, const auto entryUid) {
          return entryUid < current.entryUid;
      });
    if(pos != _streamLists.rend()) {
        const auto& si = pos->streamIds;
        result.streamCount = eagine::limit_cast<short>(si.size());
        result.streamIndex = eagine::limit_cast<short>(std::distance(
          si.begin(),
          std::find_if(si.begin(), si.end(), [&entry](const auto streamId) {
              return streamId == entry.streamId;
          })));
    }
    return result;
}
//------------------------------------------------------------------------------

