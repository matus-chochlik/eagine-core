///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRY_LIST_MODEL
#define EAGINE_XML_LOG_ENTRY_LIST_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QAbstractItemModel>

class EntriesViewModel;
struct LogEntryData;
//------------------------------------------------------------------------------
class EntryListModel
  : public QAbstractItemModel
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(int entryCount READ getEntryCount NOTIFY entryCountChanged)
public:
    EntryListModel(EntriesViewModel& parent);

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
        entryArgCount
    };

    auto getEntryMessage(LogEntryData&) const -> QString;
    auto getEntryFormat(LogEntryData&) const -> QString;
    auto getEntryStreamId(LogEntryData&) const -> qlonglong;
    auto getEntryInstanceId(LogEntryData&) const -> qlonglong;
    auto getEntrySourceId(LogEntryData&) const -> QString;
    auto getEntryTag(LogEntryData&) const -> QString;

    EntriesViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
