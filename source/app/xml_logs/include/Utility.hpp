///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_UTILITY
#define EAGINE_XML_LOG_UTILITY

import eagine.core;
#include <QString>

//------------------------------------------------------------------------------
static inline auto toQString(eagine::string_view s) noexcept -> QString {
    return QString::fromUtf8(s.data(), eagine::limit_cast<int>(s.size()));
}
//------------------------------------------------------------------------------
#endif
