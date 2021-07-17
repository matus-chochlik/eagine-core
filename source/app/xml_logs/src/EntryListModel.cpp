///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryListModel.hpp"
#include "EntriesViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
EntryListModel::EntryListModel(EntriesViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(EntListMdl), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
auto EntryListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    result.insert(Qt::DisplayRole, "display");
    result.insert(entryMessage, "message");
    result.insert(entryFormat, "format");
    result.insert(entryStreamId, "streamId");
    result.insert(entryInstanceId, "instanceId");
    result.insert(entrySource, "source");
    result.insert(entryTag, "tag");
    result.insert(entrySeverity, "severity");
    result.insert(entryArgCount, "argCount");
    return result;
}
//------------------------------------------------------------------------------
auto EntryListModel::columnCount(const QModelIndex&) const -> int {
    return 9;
}
//------------------------------------------------------------------------------
auto EntryListModel::parent(const QModelIndex&) const -> QModelIndex {
    return {};
}
//------------------------------------------------------------------------------
auto EntryListModel::index(int row, int column, const QModelIndex&) const
  -> QModelIndex {
    return QAbstractItemModel::createIndex(
      row, column, _parent.entryLog().getEntryData(row));
}
//------------------------------------------------------------------------------
auto EntryListModel::rowCount(const QModelIndex&) const -> int {
    return _parent.entryLog().getEntryCount();
}
//------------------------------------------------------------------------------
auto EntryListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    (void)index;
    (void)role;
    return {"-"};
}
//------------------------------------------------------------------------------
