///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Theme.hpp"
#include <eagine/app_config.hpp>
//------------------------------------------------------------------------------
Theme::Theme(eagine::main_ctx_parent parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(Theme), parent} {
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
