///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryFormat.hpp"
#include "EntryStorage.hpp"
#include <eagine/str_var_subst.hpp>
#include <sstream>

//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::nothing_t) noexcept -> result_type {
    return {};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::identifier i) noexcept
  -> result_type {
    return {{eagine::assign_to(_arg, i.name().view())}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::message_id& m) noexcept
  -> result_type {
    _arg.clear();
    eagine::append_to(_arg, m.class_().name().view());
    _arg.append(".");
    eagine::append_to(_arg, m.method().name().view());
    return {{_arg}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const bool b) noexcept -> result_type {
    const eagine::string_view t{"true"};
    const eagine::string_view f{"false"};
    return {b ? t : f, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const std::intmax_t i) noexcept -> result_type {
    _arg = std::to_string(i);
    return {{_arg}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const std::uintmax_t i) noexcept -> result_type {
    _arg = std::to_string(i);
    return {{_arg}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const float f) noexcept -> result_type {
    _arg = std::to_string(f);
    return {{_arg}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const std::tuple<float, float, float>&) noexcept
  -> result_type {
    return {};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const std::chrono::duration<float> d) noexcept
  -> result_type {
    std::stringstream s;
    s << d.count() << "[s]";
    _arg = s.str();
    return {{_arg}, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::string_view s) noexcept
  -> result_type {
    return {s, true};
}
//------------------------------------------------------------------------------
auto EntryFormat::format(const LogEntryData& entry) noexcept
  -> eagine::string_view {
    const auto translate = [&](const eagine::string_view arg) -> result_type {
        const eagine::identifier key{arg};
        const auto pos = entry.args.find(key);
        if(pos != entry.args.end()) {
            return std::visit<result_type>(
              *this, std::get<1>(std::get<1>(*pos)));
        }
        return {};
    };
    return eagine::substitute_variables_into(
      _msg, entry.format, {eagine::construct_from, translate});
}
//------------------------------------------------------------------------------
