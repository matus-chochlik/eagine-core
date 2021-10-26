///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Server.hpp"
#include "Backend.hpp"
#include "Connection.hpp"

//------------------------------------------------------------------------------
Server::Server(Backend& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(Server), parent}
  , _backend{parent} {
    if(_server.listen(QHostAddress::Any, 34917)) {
        connect(
          &_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    }
}
//------------------------------------------------------------------------------
void Server::onNewConnection() {
    if(auto socket{_server.nextPendingConnection()}) {
        auto newConn{std::make_shared<Connection>(*socket, _backend)};
        newConn->adoptSelf(newConn);
    }
}
//------------------------------------------------------------------------------
