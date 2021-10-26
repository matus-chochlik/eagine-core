///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_LOG
#define EAGINE_XML_LOGS_ENTRY_LOG

#include "ActivityStorage.hpp"
#include "ChartsViewModel.hpp"
#include "EntriesViewModel.hpp"
#include "EntryStorage.hpp"
#include "ProgressViewModel.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>

class Backend;
//------------------------------------------------------------------------------
class EntryLog
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(EntriesViewModel* entries READ getEntriesViewModel CONSTANT)
    Q_PROPERTY(ChartsViewModel* charts READ getChartsViewModel CONSTANT)
    Q_PROPERTY(ProgressViewModel* progress READ getProgressViewModel CONSTANT)
public:
    EntryLog(Backend& backend);

    auto backend() const noexcept -> Backend& {
        return _backend;
    }

    auto getEntriesViewModel() noexcept -> EntriesViewModel*;
    auto getChartsViewModel() noexcept -> ChartsViewModel*;
    auto getProgressViewModel() noexcept -> ProgressViewModel*;

    auto getEntryCount() const noexcept -> int;
    auto getEntryData(int index) noexcept -> LogEntryData*;

    auto getActivityCount() const noexcept -> int;
    auto getActivityData(int index) noexcept -> ActivityData*;

    void assignStorage(std::shared_ptr<LogEntryStorage>);
    void assignStorage(std::shared_ptr<ActivityStorage>);
    auto cacheString(eagine::string_view) -> eagine::string_view;

    void beginStream(std::uintptr_t streamId);
    void endStream(std::uintptr_t streamId);
    void addEntry(LogEntryData& entry);
    void commitEntries();
signals:
    void entriesAdded(int, int);
public slots:

private:
    Backend& _backend;
    EntriesViewModel _entriesViewModel;
    ChartsViewModel _chartsViewModel;
    ProgressViewModel _progressViewModel;
    std::shared_ptr<LogEntryStorage> _entries;
    std::shared_ptr<ActivityStorage> _activities;
    int _prevEntryCount{0};
};
//------------------------------------------------------------------------------
#endif
