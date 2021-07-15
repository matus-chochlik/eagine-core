///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ENTRY_LOG
#define EAGINE_XML_LOGS_ENTRY_LOG

#include "EntryStorage.hpp"
#include <eagine/main_ctx_object.hpp>
#include <eagine/maybe_unused.hpp>
#include <QObject>

class Backend;
//------------------------------------------------------------------------------
class EntryLog
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    EntryLog(Backend& backend);

    void assignStorage(std::shared_ptr<LogEntryStorage>);
signals:
public slots:

private:
    Backend& _backend;
    std::shared_ptr<LogEntryStorage> _entries;
};
//------------------------------------------------------------------------------
#endif
