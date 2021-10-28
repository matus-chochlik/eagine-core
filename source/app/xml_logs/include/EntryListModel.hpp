///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRY_LIST_MODEL
#define EAGINE_XML_LOG_ENTRY_LIST_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QAbstractItemModel>
#include <QColor>

class Backend;
class EntryViewModel;
struct LogEntryData;
//------------------------------------------------------------------------------
class EntryListModel
  : public QAbstractItemModel
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(int entryCount READ getEntryCount NOTIFY entryCountChanged)
public:
    EntryListModel(EntryViewModel& parent);

    auto backend() const noexcept -> Backend&;

    auto roleNames() const -> QHash<int, QByteArray> final;
    auto index(int row, int column, const QModelIndex& parent = {}) const
      -> QModelIndex final;
    auto parent(const QModelIndex& child) const -> QModelIndex final;
    auto columnCount(const QModelIndex& parent) const -> int final;
    auto rowCount(const QModelIndex& parent) const -> int final;
    auto data(const QModelIndex& index, int role) const -> QVariant final;
    auto getEntryCount() const -> int;

    void handleEntriesAdded(int previous, int current);
signals:
    void entryCountChanged();
public slots:

private:
    enum EntryAttributes : int {
        entryMessage = Qt::UserRole + 1,
        entryFormat,
        entryStreamId,
        entryInstanceId,
        entrySourceId,
        entryTag,
        entrySeverity,
        entrySeverityColor,
        entryReltimeSec,
        entryStreamCount,
        entryStreamIndex,
        entryStreamPosition,
        entryArgCount
    };

    auto getEntryMessage(const LogEntryData&) const -> QString;
    auto getEntryFormat(const LogEntryData&) const -> QString;
    auto getEntryStreamId(const LogEntryData&) const -> qlonglong;
    auto getEntryInstanceId(const LogEntryData&) const -> qlonglong;
    auto getEntrySourceId(const LogEntryData&) const -> QString;
    auto getEntryTag(const LogEntryData&) const -> QString;
    auto getEntrySeverity(const LogEntryData&) const -> QString;
    auto getEntrySeverityColor(const LogEntryData&) const -> QColor;
    auto getEntryReltimeSec(const LogEntryData&) const -> QVariant;
    auto getEntryStreamCount(const LogEntryData&) const -> short;
    auto getEntryStreamIndex(const LogEntryData&) const -> short;
    auto getEntryStreamPosition(const LogEntryData&) const -> short;

    EntryViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
