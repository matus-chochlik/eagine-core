///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Backend.hpp"
//------------------------------------------------------------------------------
Backend::Backend(eagine::main_ctx_parent parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(Backend), parent}
  , _server{*this}
  , _entryLog{*this}
  , _theme{*this} {}
//------------------------------------------------------------------------------
auto Backend::entryLog() noexcept -> EntryLog& {
    return _entryLog;
}
//------------------------------------------------------------------------------
auto Backend::theme() noexcept -> Theme& {
    return _theme;
}
//------------------------------------------------------------------------------
auto Backend::getEntryLog() noexcept -> EntryLog* {
    return &_entryLog;
}
//------------------------------------------------------------------------------
auto Backend::getTheme() noexcept -> Theme* {
    return &_theme;
}
//------------------------------------------------------------------------------
