///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

import eagine.core;
#include "Theme.hpp"
//------------------------------------------------------------------------------
Theme::Theme(eagine::main_ctx_parent parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{"Theme", parent} {
    setLight(
      extract_or(app_config().get<bool>("log.viewer.theme.light"), false));
}
//------------------------------------------------------------------------------
void Theme::setLight(bool value) {
    _light = value;
    emit lightChanged();
}
//------------------------------------------------------------------------------
auto Theme::getLight() const -> bool {
    return _light;
}
//------------------------------------------------------------------------------
auto Theme::getEntrySourceWidth() const -> int {
    return 90;
}
//------------------------------------------------------------------------------
auto Theme::getEntrySeverityWidth() const -> int {
    return 80;
}
//------------------------------------------------------------------------------
auto Theme::getEntryReltimeWidth() const -> int {
    return 100;
}
//------------------------------------------------------------------------------
auto Theme::getEntryProgressWidth() const -> int {
    return 60;
}
//------------------------------------------------------------------------------
auto Theme::getEntryHeaderHeight() const -> int {
    return 19;
}
//------------------------------------------------------------------------------
auto Theme::getSeverityColor(eagine::log_event_severity severity) -> QColor {
    if(_light) {
        switch(severity) {
            case eagine::log_event_severity::fatal:
                return {255, 164, 164};
            case eagine::log_event_severity::error:
                return {255, 192, 192};
            case eagine::log_event_severity::warning:
                return {225, 192, 164};
            case eagine::log_event_severity::stat:
                return {192, 225, 164};
            case eagine::log_event_severity::debug:
                return {225, 225, 148};
            case eagine::log_event_severity::info:
                return {225, 225, 225};
            case eagine::log_event_severity::trace:
            case eagine::log_event_severity::backtrace:
                return {164, 192, 225};
        }
    } else {
        switch(severity) {
            case eagine::log_event_severity::fatal:
                return {208, 64, 64};
            case eagine::log_event_severity::error:
                return {225, 96, 96};
            case eagine::log_event_severity::warning:
                return {205, 192, 96};
            case eagine::log_event_severity::stat:
                return {192, 205, 96};
            case eagine::log_event_severity::debug:
                return {205, 205, 108};
            case eagine::log_event_severity::info:
                return {108, 108, 108};
            case eagine::log_event_severity::trace:
            case eagine::log_event_severity::backtrace:
                return {96, 192, 205};
        }
    }
    return {128, 128, 128};
}
//------------------------------------------------------------------------------
