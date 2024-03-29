///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

import eagine.core;
#include "EntryListModel.hpp"
#include "Backend.hpp"
#include "EntryFormat.hpp"
#include "EntryLog.hpp"
#include "EntryViewModel.hpp"
#include "Utility.hpp"
//------------------------------------------------------------------------------
EntryListModel::EntryListModel(EntryViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{"EntListMdl", parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
auto EntryListModel::backend() const noexcept -> Backend& {
    return _parent.backend();
}
//------------------------------------------------------------------------------
auto EntryListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    result.insert(Qt::DisplayRole, "display");
    result.insert(entryMessage, "message");
    result.insert(entryFormat, "format");
    result.insert(entryLogIdentity, "logIdentity");
    result.insert(entryStreamId, "streamId");
    result.insert(entryInstanceId, "instanceId");
    result.insert(entrySourceId, "sourceId");
    result.insert(entryTag, "tag");
    result.insert(entrySeverity, "severity");
    result.insert(entrySeverityColor, "severityColor");
    result.insert(entryReltimeSec, "reltimeSec");
    result.insert(entryStreamCount, "streamCount");
    result.insert(entryStreamIndex, "streamIndex");
    result.insert(entryStreamPosition, "streamPosition");
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
auto EntryListModel::getEntryMessage(const LogEntryData& entry) const
  -> QString {
    return toQString(EntryFormat().format(entry));
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryFormat(const LogEntryData& entry) const
  -> QString {
    return toQString(entry.format);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryLogIdentity(const LogEntryData& entry) const
  -> QString {
    return toQString(
      _parent.entryLog().streamInfoRef(entry.streamId).logIdentity);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryStreamId(const LogEntryData& entry) const
  -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.streamId);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryInstanceId(const LogEntryData& entry) const
  -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.instance);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntrySourceId(const LogEntryData& entry) const
  -> QString {
    return toQString(entry.source.name().view());
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryTag(const LogEntryData& entry) const -> QString {
    return toQString(entry.tag.name().view());
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntrySeverity(const LogEntryData& entry) const
  -> QString {
    return toQString(eagine::enumerator_name(entry.severity));
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntrySeverityColor(const LogEntryData& entry) const
  -> QColor {
    return backend().theme().getSeverityColor(entry.severity);
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryReltimeSec(const LogEntryData& entry) const
  -> QVariant {
    if(entry.reltimeSec >= 0.F) {
        return {entry.reltimeSec};
    }
    return {};
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryStreamCount(const LogEntryData& entry) const
  -> short {
    return _parent.entryLog().getEntryConnectors(entry).streamCount;
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryStreamIndex(const LogEntryData& entry) const
  -> short {
    return _parent.entryLog().getEntryConnectors(entry).streamIndex;
}
//------------------------------------------------------------------------------
auto EntryListModel::getEntryStreamPosition(const LogEntryData& entry) const
  -> short {
    return short(entry.isFirst ? -1 : entry.isLast ? 1 : 0);
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
            case entryLogIdentity:
                return {getEntryLogIdentity(entry)};
            case entryStreamId:
                return {getEntryStreamId(entry)};
            case entryInstanceId:
                return {getEntryInstanceId(entry)};
            case entrySourceId:
                return {getEntrySourceId(entry)};
            case entryTag:
                return {getEntryTag(entry)};
            case entrySeverity:
                return {getEntrySeverity(entry)};
            case entrySeverityColor:
                return {getEntrySeverityColor(entry)};
            case entryReltimeSec:
                return {getEntryReltimeSec(entry)};
            case entryStreamCount:
                return {getEntryStreamCount(entry)};
            case entryStreamIndex:
                return {getEntryStreamIndex(entry)};
            case entryStreamPosition:
                return {getEntryStreamPosition(entry)};
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
