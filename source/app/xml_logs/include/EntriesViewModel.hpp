///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRIES_VIEW_MODEL
#define EAGINE_XML_LOG_ENTRIES_VIEW_MODEL

#include "EntryFilterModel.hpp"
#include "EntryListModel.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>

class Backend;
class EntryLog;
//------------------------------------------------------------------------------
class EntriesViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(EntryFilterModel* entryFilter READ getEntryFilterModel CONSTANT)
    Q_PROPERTY(EntryListModel* entryList READ getEntryListModel CONSTANT)
public:
    EntriesViewModel(EntryLog& parent);

    auto backend() const noexcept -> Backend&;
    auto entryLog() const noexcept -> EntryLog& {
        return _parent;
    }

    auto getEntryFilterModel() noexcept -> EntryFilterModel*;
    auto getEntryListModel() noexcept -> EntryListModel*;
signals:
public slots:
    void onEntriesAdded(int previous, int current);

private:
    EntryLog& _parent;
    EntryFilterModel _entryFilterModel;
    EntryListModel _entryListModel;
};
//------------------------------------------------------------------------------
#endif
