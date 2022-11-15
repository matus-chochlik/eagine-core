///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

import eagine.core;
import <sstream>;
#include "EntryFormat.hpp"
#include "EntryStorage.hpp"

//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const eagine::nothing_t) noexcept -> result_type {
    return {};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const eagine::identifier i) noexcept -> result_type {
    _arg.clear();
    _arg.append("<b>");
    eagine::append_to(i.name().view(), _arg);
    _arg.append("</b>");
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const eagine::message_id& m) noexcept -> result_type {
    _arg.clear();
    eagine::append_to(m.class_().name().view(), _arg);
    _arg.append(".");
    eagine::append_to(m.method().name().view(), _arg);
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::identifier, const bool b) noexcept
  -> result_type {
    const eagine::string_view t{"true"};
    const eagine::string_view f{"false"};
    return {b ? t : f};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const std::intmax_t i) noexcept -> result_type {
    _arg.clear();
    _arg.append(std::to_string(i));
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const std::uintmax_t i) noexcept -> result_type {
    _arg.clear();
    _arg.append(std::to_string(i));
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(const eagine::identifier, const float f) noexcept
  -> result_type {
    _arg.clear();
    _arg.append(std::to_string(f));
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const std::tuple<float, float, float>& t) noexcept -> result_type {
    const auto [min, val, max] = t;
    _arg.clear();
    _arg.append("<b>");
    _arg.append(std::to_string(100.F * (val - min) / (max - min)));
    _arg.append("%</b>");
    return {};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const std::chrono::duration<float> d) noexcept -> result_type {
    std::stringstream s;
    s << d.count() << "[s]";
    _arg = s.str();
    return {{_arg}};
}
//------------------------------------------------------------------------------
auto EntryFormat::operator()(
  const eagine::identifier,
  const eagine::string_view s) noexcept -> result_type {
    return {s};
}
//------------------------------------------------------------------------------
auto EntryFormat::format(const LogEntryData& entry) noexcept
  -> eagine::string_view {
    const auto translate = [&](const eagine::string_view arg) -> result_type {
        const eagine::identifier key{arg};
        const auto pos = entry.args.find(key);
        if(pos != entry.args.end()) {
            const auto& arg_info = std::get<1>(*pos);
            return std::visit<result_type>(
              [&](const auto& value) -> result_type {
                  return (*this)(std::get<0>(arg_info), value);
              },
              std::get<1>(arg_info));
        }
        return {};
    };
    return eagine::substitute_variables_into(
      _msg, entry.format, {eagine::construct_from, translate});
}
//------------------------------------------------------------------------------
