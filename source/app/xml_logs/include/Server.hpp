///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_SERVER
#define EAGINE_XML_LOGS_SERVER

#if EAGINE_CORE_MODULE
import eagine.core;
#else
#include <eagine/main_ctx_object.hpp>
#endif
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

//------------------------------------------------------------------------------
class Backend;
class Server
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    Server(Backend&);

signals:
public slots:
    void onNewConnection();

private:
    Backend& _backend;
    QTcpServer _server;
};
//------------------------------------------------------------------------------
#endif
