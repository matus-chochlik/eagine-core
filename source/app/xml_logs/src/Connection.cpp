///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Connection.hpp"
#include "Backend.hpp"
#include <eagine/assert.hpp>
//------------------------------------------------------------------------------
Connection::Connection(QTcpSocket& socket, Backend& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(Connection), parent}
  , _backend{parent}
  , _socket{socket}
  , _streamId{reinterpret_cast<std::uintptr_t>(&_socket)} {

    connect(&_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &Connection::onError);

    _xmlReader.setDevice(&_socket);
}
//------------------------------------------------------------------------------
Connection::~Connection() noexcept {
    _socket.close();
}
//------------------------------------------------------------------------------
auto Connection::_toIdentifier(const QStringRef& s) -> eagine::identifier {
    const auto utf8{s.toUtf8()};
    return eagine::identifier{eagine::view(utf8)};
}
//------------------------------------------------------------------------------
auto Connection::_cacheString(const QStringRef& s) -> eagine::string_view {
    const auto utf8{s.toUtf8()};
    return _backend.entryLog().cacheString(eagine::view(utf8));
}
//------------------------------------------------------------------------------
void Connection::onReadyRead() {
    bool keepReading = _readingLog;
    do {
        switch(_xmlReader.readNext()) {
            case QXmlStreamReader::StartElement: {
                if(_xmlReader.name() == QLatin1String("a")) {
                    _readingAttribute = true;
                } else if(_xmlReader.name() == QLatin1String("f")) {
                    _readingFormat = true;
                } else if(_xmlReader.name() == QLatin1String("m")) {
                    _readingMessage = true;
                    // id
                    _currentEntry.stream_id = _streamId;
                    // source
                    QStringRef s = _xmlReader.attributes().value("src");
                    if(s.isEmpty()) {
                        _currentEntry.source = EAGINE_ID(Unknown);
                    } else {
                        _currentEntry.source = _toIdentifier(s);
                    }
                    // tag
                    s = _xmlReader.attributes().value("tag");
                    if(s.isEmpty()) {
                        _currentEntry.tag = {};
                    } else {
                        _currentEntry.source = _toIdentifier(s);
                    }
                } else if(_xmlReader.name() == QLatin1String("log")) {
                    _readingLog = true;
                }
                break;
            }
            case QXmlStreamReader::EndElement: {
                if(_xmlReader.name() == QLatin1String("a")) {
                    _readingAttribute = false;
                } else if(_xmlReader.name() == QLatin1String("f")) {
                    _readingFormat = false;
                } else if(_xmlReader.name() == QLatin1String("m")) {
                    if(_readingMessage) {
                        _readingMessage = false;
                        _backend.entryLog().addEntry(_currentEntry);
                    }
                } else if(_xmlReader.name() == QLatin1String("log")) {
                    _readingLog = false;
                }
                break;
            }
            case QXmlStreamReader::Characters: {
                if(_readingMessage) {
                    if(_readingFormat) {
                        _currentEntry.format = _cacheString(_xmlReader.text());
                    } else if(_readingAttribute) {
                    }
                }
                break;
            }
            case QXmlStreamReader::StartDocument: {
                keepReading = true;
                break;
            }
            case QXmlStreamReader::NoToken:
            case QXmlStreamReader::Invalid:
            case QXmlStreamReader::EndDocument: {
                keepReading = false;
                break;
            }
            default: {
                break;
            }
        }
    } while(keepReading);
    _backend.entryLog().commitEntries();
}
//------------------------------------------------------------------------------
void Connection::onError(QAbstractSocket::SocketError) {
    _self.reset();
}
//------------------------------------------------------------------------------
