///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRY_FILTER_MODEL
#define EAGINE_XML_LOG_ENTRY_FILTER_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QObject>

class EntriesViewModel;
//------------------------------------------------------------------------------
class EntryFilterModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    EntryFilterModel(EntriesViewModel& parent);

signals:
public slots:

private:
    EntriesViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
