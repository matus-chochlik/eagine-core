///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryListModel.hpp"
#include "EntriesViewModel.hpp"
#include "EntryLog.hpp"
#include <eagine/extract.hpp>
#include <eagine/is_within_limits.hpp>
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
    result.insert(entrySourceId, "sourceId");
    result.insert(entryTag, "tag");
    result.insert(entrySeverity, "severity");
    result.insert(entryArgCount, "argCount");
    return result;
}
//------------------------------------------------------------------------------
auto EntryListModel::columnCount(const QModelIndex&) const -> int {
    return 1;
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
auto EntryListModel::rowCount(const QModelIndex& i) const -> int {
    return i.isValid() ? 0 : _parent.entryLog().getEntryCount();
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryCount() const -> int {
    return _parent.entryLog().getEntryCount();
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryMessage(LogEntryData&) const -> QString {
    return {};
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryFormat(LogEntryData& entry) const -> QString {
    return {c_str(entry.format)};
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryStreamId(LogEntryData& entry) const -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.stream_id);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryInstanceId(LogEntryData& entry) const
  -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.instance);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntrySourceId(LogEntryData& entry) const -> QString {
    return {c_str(entry.source.name().view())};
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryTag(LogEntryData& entry) const -> QString {
    return {c_str(entry.tag.name().view())};
}
//------------------------------------------------------------------------------
auto EntryListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    if(auto optEntry{static_cast<LogEntryData*>(index.internalPointer())}) {
        auto& entry{eagine::extract(optEntry)};
        switch(role) {
            case entryMessage:
                return {getEntryMessage(entry)};
            case entryFormat:
                return {getEntryFormat(entry)};
            case entryStreamId:
                return {getEntryStreamId(entry)};
            case entryInstanceId:
                return {getEntryInstanceId(entry)};
            case entrySourceId:
                return {getEntrySourceId(entry)};
            case entryTag:
                return {getEntryTag(entry)};
            default:
                break;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
void EntryListModel::handleEntriesAdded(int previous, int current) {
    (void)previous;
    (void)current;
    emit modelReset({});
    emit entryCountChanged();
}
//------------------------------------------------------------------------------
