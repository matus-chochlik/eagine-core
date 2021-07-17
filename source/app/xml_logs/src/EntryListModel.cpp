///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryListModel.hpp"
#include "LogViewModel.hpp"
//------------------------------------------------------------------------------
EntryListModel::EntryListModel(LogViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(EntListMdl), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
auto EntryListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    return result;
}
//------------------------------------------------------------------------------
auto EntryListModel::index(int row, int column, const QModelIndex&) const
  -> QModelIndex {
    return QAbstractItemModel::createIndex(row, column);
}
//------------------------------------------------------------------------------
auto EntryListModel::parent(const QModelIndex&) const -> QModelIndex {
    return {};
}
//------------------------------------------------------------------------------
auto EntryListModel::columnCount(const QModelIndex&) const -> int {
    return 0;
}
//------------------------------------------------------------------------------
auto EntryListModel::rowCount(const QModelIndex&) const -> int {
    return 0;
}
//------------------------------------------------------------------------------
auto EntryListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    (void)index;
    (void)role;
    return {};
}
//------------------------------------------------------------------------------
