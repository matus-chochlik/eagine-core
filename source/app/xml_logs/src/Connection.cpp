///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Connection.hpp"
#include "Backend.hpp"
#include <eagine/assert.hpp>
#include <eagine/from_string.hpp>
#include <eagine/reflect/enumerators.hpp>
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
    return eagine::identifier{eagine::view(s.toUtf8())};
}
//------------------------------------------------------------------------------
auto Connection::_toSeverity(const QStringRef& s) noexcept
  -> eagine::log_event_severity {
    if(const auto severity{eagine::from_string<eagine::log_event_severity>(
         eagine::view(s.toUtf8()))}) {
        return extract(severity);
    }
    return eagine::log_event_severity::info;
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
auto Connection::_handleBeginMessage() noexcept -> bool {
    // id
    _currentEntry.stream_id = _streamId;
    // tag
    QStringRef s = _xmlReader.attributes().value("tag");
    if(s.isEmpty()) {
        _currentEntry.tag = {};
    } else {
        _currentEntry.source = _toIdentifier(s);
    }
    // source
    s = _xmlReader.attributes().value("src");
    if(s.isEmpty()) {
        _currentEntry.source = EAGINE_ID(Unknown);
    } else {
        _currentEntry.source = _toIdentifier(s);
    }
    // severity
    s = _xmlReader.attributes().value("lvl");
    _currentEntry.severity = _toSeverity(s);

    return true;
}
//------------------------------------------------------------------------------
auto Connection::_handleBeginArgument() noexcept -> bool {
    QStringRef s = _xmlReader.attributes().value("n");
    if(s.isEmpty()) {
        return false;
    }
    _currentArgName = _toIdentifier(s);
    s = _xmlReader.attributes().value("t");
    if(!s.isEmpty()) {
        _currentArgTag = _toIdentifier(s);
    }
    return true;
}
//------------------------------------------------------------------------------
void Connection::_handleStartElement() noexcept {
    if(_isAtArgumentTag()) {
        _readingArgument = _handleBeginArgument();
    } else if(_isAtMessageTag()) {
        _readingMessage = _handleBeginMessage();
    } else if(_isAtFormatTag()) {
        _readingFormat = true;
    } else if(_isAtLogTag()) {
        _readingLog = true;
    }
}
//------------------------------------------------------------------------------
void Connection::_handleEndElement() noexcept {
    if(_isAtArgumentTag()) {
        if(_readingArgument) {
            _readingArgument = false;
        }
    } else if(_isAtMessageTag()) {
        if(_readingMessage) {
            _readingMessage = false;
            _backend.entryLog().addEntry(_currentEntry);
        }
    } else if(_isAtFormatTag()) {
        _readingFormat = false;
    } else if(_isAtLogTag()) {
        _readingLog = false;
    }
}
//------------------------------------------------------------------------------
void Connection::_handleFormatText() noexcept {
    _currentEntry.format = _cacheString(_xmlReader.text());
}
//------------------------------------------------------------------------------
auto Connection::_isBoolArg() const noexcept -> bool {
    return _currentArgTag == EAGINE_ID(bool);
}
//------------------------------------------------------------------------------
auto Connection::_isIntegerArg() const noexcept -> bool {
    return _currentArgTag == EAGINE_ID(int64) ||
           _currentArgTag == EAGINE_ID(int32) ||
           _currentArgTag == EAGINE_ID(int16);
}
//------------------------------------------------------------------------------
auto Connection::_isUnsignedArg() const noexcept -> bool {
    return _currentArgTag == EAGINE_ID(uint64) ||
           _currentArgTag == EAGINE_ID(uint32) ||
           _currentArgTag == EAGINE_ID(uint16);
}
//------------------------------------------------------------------------------
auto Connection::_isFloatArg() const noexcept -> bool {
    return _currentArgTag == EAGINE_ID(real);
}
//------------------------------------------------------------------------------
auto Connection::_isDurationArg() const noexcept -> bool {
    return _currentArgTag == EAGINE_ID(duration);
}
//------------------------------------------------------------------------------
void Connection::_handleArgumentValue() noexcept {
    const auto s = _xmlReader.text();
    if(_isBoolArg()) {
        _currentEntry.args[_currentArgName] = {
          _currentArgTag, s == QLatin1String("true")};
    } else if(_isUnsignedArg()) {
        if(const auto value{
             eagine::from_string<std::uintmax_t>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isIntegerArg()) {
        if(const auto value{
             eagine::from_string<std::intmax_t>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isFloatArg()) {
        if(const auto value{
             eagine::from_string<float>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isDurationArg()) {
        if(const auto value{
             eagine::from_string<float>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, std::chrono::duration<float>{extract(value)}};
        }
    } else {
        _currentEntry.args[_currentArgName] = {_currentArgTag, _cacheString(s)};
    }
}
//------------------------------------------------------------------------------
void Connection::_handleElementText() noexcept {
    if(_readingMessage) {
        if(_readingFormat) {
            _handleFormatText();
        } else if(_readingArgument) {
            _handleArgumentValue();
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
