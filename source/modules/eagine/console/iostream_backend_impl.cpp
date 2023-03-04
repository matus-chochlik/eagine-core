/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.console;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.container;
import eagine.core.utility;
import :backend;
import :entry_kind;
import std;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable>
class iostream_console_backend : public console_backend {
public:
    iostream_console_backend(std::ostream& out) noexcept
      : _out{out} {}

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return "InOutStrm";
    }

    auto entry_backend(const identifier, const console_entry_kind) noexcept
      -> std::tuple<console_backend*, console_entry_id_t> override {
        const std::lock_guard lock{_lockable};
        do {
            ++_entry_id_seq;
        } while(_entry_id_seq == 0);
        return {this, _entry_id_seq};
    }

    auto begin_message(
      const identifier source,
      const console_entry_id_t parent_id,
      const console_entry_id_t,
      const console_entry_kind kind,
      const string_view format) noexcept -> bool final {
        _lockable.lock();
        assign_to(format, _message.back());
        _message.swap();
        _source = source;
        _kind = kind;
        _separate = false;
        if(const auto pos{_hierarchy.find(parent_id)};
           pos != _hierarchy.end()) {
            _depth = std::get<1>(*pos);
        } else {
            _depth = 0;
        }
        return true;
    }

    void add_identifier(
      const identifier arg,
      const identifier,
      const identifier value) noexcept final {
        _add(arg, value.name().view());
    }

    void add_bool(
      const identifier arg,
      const identifier,
      const bool value) noexcept final {
        if(value) {
            _add(arg, string_view{"true"});
        } else {
            _add(arg, string_view{"false"});
        }
    }

    void add_integer(
      const identifier arg,
      const identifier,
      const std::intmax_t value) noexcept final {
        return _add(arg, value);
    }

    void add_unsigned(
      const identifier arg,
      const identifier,
      const std::uintmax_t value) noexcept final {
        return _add(arg, value);
    }

    void add_float(
      const identifier arg,
      const identifier,
      const double value) noexcept final {
        return _add(arg, value);
    }

    void add_duration(
      const identifier arg,
      const identifier,
      const std::chrono::duration<float> value) noexcept final {
        // TODO
        return _add(arg, value.count());
    }

    void add_string(
      const identifier arg,
      const identifier,
      const string_view value) noexcept final {
        _add(arg, value);
    }

    void add_blob(
      const identifier arg,
      const identifier,
      const memory::const_block value) noexcept final {
        _out << arg.name() << hexdump(value) << '\n';
    }

    void add_separator() noexcept final {
        _separate = true;
    }

    void finish_message() noexcept final {
        try {
            _out << std::string(_depth * 2, ' ') << _message.front() << '\n';
            if(_separate) {
                _out << '\n';
            }
            _lockable.unlock();
        } catch(...) {
        }
    }

    void to_be_continued(
      const console_entry_id_t parent_id,
      const console_entry_id_t id) noexcept final {
        std::size_t depth{0};
        if(const auto pos{_hierarchy.find(parent_id)};
           pos != _hierarchy.end()) {
            depth = std::get<1>(*pos);
        }
        _hierarchy.emplace(id, depth + 1);
    }

    void concluded(const console_entry_id_t id) noexcept final {
        _hierarchy.erase(id);
    }

private:
    template <typename T>
    auto _to_str(T value) noexcept -> std::optional<string_view> {
        auto b{_temp.data()};
        auto e{b + _temp.size()};
        if(const auto conv{std::to_chars(b, e, value)}; conv.ec == std::errc())
          [[likely]] {
            return {head(view(_temp), std::distance(b, conv.ptr))};
        }
        return {};
    }

    void _add(const identifier arg, const string_view value) noexcept {
        _message.back().clear();
        substitute_variable_into(
          _message.back(), _message.front(), arg.name(), value);
        _message.swap();
    }

    void _add(const identifier arg, const std::string& value) noexcept {
        _add(arg, string_view{value});
    }

    template <typename T>
    void _add(const identifier arg, T value) noexcept {
        if(const auto str_val{_to_str(value)}) {
            _add(arg, extract(str_val));
        }
    }

    Lockable _lockable{};
    std::ostream& _out;
    std::array<char, 64> _temp{};
    double_buffer<std::string> _message;
    console_entry_id_t _entry_id_seq{0};
    identifier _source{};
    std::size_t _depth{0};
    flat_map<console_entry_id_t, std::size_t> _hierarchy{};
    console_entry_kind _kind{console_entry_kind::info};
    bool _separate{false};
};
//------------------------------------------------------------------------------
auto make_iostream_console_backend(std::istream&, std::ostream& out)
  -> std::unique_ptr<console_backend> {
    return std::make_unique<iostream_console_backend<std::mutex>>(out);
}
//------------------------------------------------------------------------------
} // namespace eagine
