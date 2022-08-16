///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_LOG
#define EAGINE_XML_LOGS_ENTRY_LOG

import eagine.core;
#include "ActivityStorage.hpp"
#include "ChartsViewModel.hpp"
#include "EntryStorage.hpp"
#include "EntryViewModel.hpp"
#include "ProgressViewModel.hpp"
#include "StreamViewModel.hpp"
#include <QObject>

class Backend;
//------------------------------------------------------------------------------
class EntryLog
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(EntryViewModel* entries READ getEntryViewModel CONSTANT)
    Q_PROPERTY(ProgressViewModel* progress READ getProgressViewModel CONSTANT)
    Q_PROPERTY(ChartsViewModel* charts READ getChartsViewModel CONSTANT)
    Q_PROPERTY(StreamViewModel* streams READ getStreamViewModel CONSTANT)
public:
    EntryLog(Backend& backend);

    auto backend() const noexcept -> Backend& {
        return _backend;
    }

    auto getStreamCount() const noexcept -> int;
    auto getStreamInfo(int index) noexcept -> LogStreamInfo*;
    auto streamInfoRef(const stream_id_t streamId) noexcept -> LogStreamInfo&;

    auto getEntryViewModel() noexcept -> EntryViewModel*;
    auto getProgressViewModel() noexcept -> ProgressViewModel*;
    auto getChartsViewModel() noexcept -> ChartsViewModel*;
    auto getStreamViewModel() noexcept -> StreamViewModel*;

    auto getEntryCount() const noexcept -> int;
    auto getEntryData(int index) noexcept -> LogEntryData*;
    auto getEntryConnectors(const LogEntryData&) noexcept -> LogEntryConnectors;

    auto getActivityCount() const noexcept -> int;
    auto getActivityData(int index) noexcept -> ActivityData*;

    void assignStorage(std::shared_ptr<LogEntryStorage>);
    void assignStorage(std::shared_ptr<ActivityStorage>);
    auto cacheString(eagine::string_view) -> eagine::string_view;

    void beginStream(stream_id_t streamId, const LogStreamInfo&);
    void endStream(stream_id_t streamId);
    void addEntry(LogEntryData& entry);
    void commitEntries();

    auto cleanupDoneActivities() noexcept -> bool;
signals:
    void streamsAdded();
    void entriesAdded(int, int);
public slots:

private:
    Backend& _backend;
    EntryViewModel _entryViewModel;
    ProgressViewModel _progressViewModel;
    ChartsViewModel _chartsViewModel;
    StreamViewModel _streamViewModel;
    std::shared_ptr<LogEntryStorage> _entries;
    std::shared_ptr<ActivityStorage> _activities;
    int _prevEntryCount{0};
};
//------------------------------------------------------------------------------
#endif
