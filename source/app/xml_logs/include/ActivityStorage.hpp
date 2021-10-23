///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP
#define EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP

#include <eagine/identifier.hpp>
#include <eagine/logging/fwd.hpp>
#include <eagine/logging/severity.hpp>
#include <vector>

class Backend;
//------------------------------------------------------------------------------
struct ActivityData {
    std::uintptr_t stream_id;
    eagine::identifier source;
    eagine::identifier tag;
    eagine::logger_instance_id instance;
    eagine::log_event_severity severity;
};
//------------------------------------------------------------------------------
class ActivityStorage {
public:
private:
    std::vector<ActivityData> _entries;
};
//------------------------------------------------------------------------------

#endif

