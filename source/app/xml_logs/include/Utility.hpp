///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_UTILITY
#define EAGINE_XML_LOG_UTILITY

#include <eagine/is_within_limits.hpp>
#include <eagine/string_span.hpp>
#include <QString>

//------------------------------------------------------------------------------
static inline auto toQString(eagine::string_view s) noexcept -> QString {
    return QString::fromUtf8(s.data(), eagine::limit_cast<int>(s.size()));
}
//------------------------------------------------------------------------------
#endif
