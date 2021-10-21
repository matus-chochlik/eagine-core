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
auto Connection::_toIdentifier(const QStringRef& s) noexcept
  -> eagine::identifier {
    const auto utf8{s.toUtf8()};
    return eagine::identifier{eagine::view(utf8)};
}
//------------------------------------------------------------------------------
auto Connection::_cacheString(const QStringRef& s) noexcept
  -> eagine::string_view {
    const auto utf8{s.toUtf8()};
    return _backend.entryLog().cacheString(eagine::view(utf8));
}
//------------------------------------------------------------------------------
auto Connection::_isAtLogTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("log");
}
//------------------------------------------------------------------------------
auto Connection::_isAtMessageTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("m");
}
//------------------------------------------------------------------------------
auto Connection::_isAtFormatTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("f");
}
//------------------------------------------------------------------------------
auto Connection::_isAtArgumentTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("a");
}
//------------------------------------------------------------------------------
void Connection::_handleStartElement() noexcept {
    if(_isAtArgumentTag()) {
        _readingAttribute = true;
    } else if(_isAtFormatTag()) {
        _readingFormat = true;
    } else if(_isAtMessageTag()) {
        // tag
        QStringRef s = _xmlReader.attributes().value("tag");
        if(s.isEmpty()) {
            _currentEntry.tag = {};
        } else {
            _currentEntry.source = _toIdentifier(s);
        }
        _readingMessage = true;
        // id
        _currentEntry.stream_id = _streamId;
        // source
        s = _xmlReader.attributes().value("src");
        if(s.isEmpty()) {
            _currentEntry.source = EAGINE_ID(Unknown);
        } else {
            _currentEntry.source = _toIdentifier(s);
        }
    } else if(_isAtLogTag()) {
        _readingLog = true;
    }
}
//------------------------------------------------------------------------------
void Connection::_handleEndElement() noexcept {
    if(_isAtArgumentTag()) {
        _readingAttribute = false;
    } else if(_isAtFormatTag()) {
        _readingFormat = false;
    } else if(_isAtMessageTag()) {
        if(_readingMessage) {
            _readingMessage = false;
            _backend.entryLog().addEntry(_currentEntry);
        }
    } else if(_isAtLogTag()) {
        _readingLog = false;
    }
}
//------------------------------------------------------------------------------
void Connection::_handleElementText() noexcept {
    if(_readingMessage) {
        if(_readingFormat) {
            _currentEntry.format = _cacheString(_xmlReader.text());
        } else if(_readingAttribute) {
        }
    }
}
//------------------------------------------------------------------------------
void Connection::onReadyRead() {
    bool keepReading = _readingLog;
    do {
        switch(_xmlReader.readNext()) {
            case QXmlStreamReader::StartElement: {
                _handleStartElement();
                break;
            }
            case QXmlStreamReader::EndElement: {
                _handleEndElement();
                break;
            }
            case QXmlStreamReader::Characters: {
                _handleElementText();
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
