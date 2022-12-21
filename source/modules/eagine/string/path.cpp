/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.string:path;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :multi_byte;
import :list;
import <compare>;
import <iostream>;
import <filesystem>;
import <string>;
import <tuple>;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Basic class storing paths made of string elements.
/// @ingroup string_utils
///
/// Efficiently stores a sequence of string elements in a single block of memory.
export class basic_string_path {
public:
    /// @brief The element value type.
    using value_type = string_view;

    /// @brief The element count type.
    using size_type = span_size_t;

    /// @brief Iterator type.
    using iterator = string_list::iterator<const char*>;

    /// @brief Reverse iterator type.
    using reverse_iterator = string_list::rev_iterator<const char*>;

    /// @brief Default constructor. Creates an empty path.
    /// @post empty()
    basic_string_path() noexcept = default;

    /// @brief Move constructor.
    basic_string_path(basic_string_path&&) noexcept = default;

    /// @brief Copy constructor.
    basic_string_path(const basic_string_path&) = default;

    /// @brief Move assignment operator.
    auto operator=(basic_string_path&&) noexcept
      -> basic_string_path& = default;

    /// @brief Copy assignment operator.
    auto operator=(const basic_string_path&) -> basic_string_path& = default;

    ~basic_string_path() noexcept = default;

    basic_string_path(const string_view str, const span_size_t size) noexcept
      : _size{size}
      , _str{str.data(), str.std_size()} {}

    basic_string_path(std::string str, const span_size_t size) noexcept
      : _size{size}
      , _str{std::move(str)} {}

    basic_string_path(std::tuple<std::string, span_size_t>&& init) noexcept
      : basic_string_path{std::move(std::get<0>(init)), std::get<1>(init)} {}

    /// @brief Construction from a path string and a separator string.
    basic_string_path(
      const string_view path,
      split_by_t,
      const string_view sep) noexcept
      : basic_string_path{string_list::split(path, sep)} {}

    basic_string_path(const string_view path) noexcept
      : basic_string_path(path, split_by, {"/"}) {}

    explicit basic_string_path(
      const basic_string_path& a,
      const basic_string_path& b) noexcept
      : _size{safe_add(a._size, b._size)}
      , _str{a._str + b._str} {}

    /// @brief Construction from a list of path element names.
    explicit basic_string_path(
      const memory::span<const string_view> names) noexcept {
        _init(names);
    }

    /// @brief Construction from a list of path element names.
    template <std::size_t N>
    explicit basic_string_path(const std::array<string_view, N>& names) noexcept
      : basic_string_path(view(names)) {}

    /// @brief Construction from a pack of path element names.
    template <typename... Str>
    explicit basic_string_path(
      from_pack_t,
      const string_view name,
      const Str&... names) noexcept
      : basic_string_path(_pack_names(name, view(names)...)) {}

    /// @brief Conversion to filesystem path.
    [[nodiscard]] operator std::filesystem::path() const {
        return {as_string()};
    }

    /// @brief Comparison.
    /// @see like
    [[nodiscard]] auto operator<=>(
      const basic_string_path& that) const noexcept {
        return _str.compare(that._str);
    }

    [[nodiscard]] auto operator==(const basic_string_path&) const noexcept
      -> bool = default;

    [[nodiscard]] static auto elements_match(
      const string_view elem,
      const string_view patt) noexcept -> bool {
        return (elem == patt) or (patt == string_view{"*"});
    }

    /// @brief Tests if this path matches a pattern.
    /// @see starts_with
    [[nodiscard]] auto like(const basic_string_path& pattern) const noexcept
      -> bool {
        if(size() != pattern.size()) {
            return false;
        }
        auto tp{begin()};
        auto pp{pattern.begin()};
        while(tp != end() and pp != pattern.end()) {
            if(not elements_match(*tp, *pp)) {
                return false;
            }
            ++tp;
            ++pp;
        }
        return true;
    }

    /// @brief Tests if the beginning of this path matches a pattern.
    /// @see like
    [[nodiscard]] auto starts_with(
      const basic_string_path& pattern) const noexcept -> bool {
        auto tp{begin()};
        auto pp{pattern.begin()};
        while(tp != end() and pp != pattern.end()) {
            if(not elements_match(*tp, *pp)) {
                return false;
            }
            ++tp;
            ++pp;
        }
        return tp == end();
    }

    /// @brief Concatenates two paths.
    [[nodiscard]] friend auto operator+(
      const basic_string_path& a,
      const basic_string_path& b) noexcept {
        return basic_string_path(a, b);
    }

    /// @brief Indicates if this path is empty.
    /// @see size
    /// @see clear
    [[nodiscard]] auto empty() const noexcept -> bool {
        assert((size() == 0) == _str.empty());
        return _str.empty();
    }

    /// @brief Clears this path.
    /// @see empty
    void clear() noexcept {
        _size = 0;
        _str.clear();
    }

    /// @brief Returns the number of elements in this path.
    /// @see empty
    [[nodiscard]] auto size() const noexcept -> size_type {
        return _size;
    }

    [[nodiscard]] static auto required_bytes(const size_type l) noexcept
      -> size_type {
        using namespace multi_byte;
        return safe_add(
          l, 2 * required_sequence_length(code_point_t(l)).value());
    }

    [[nodiscard]] static auto required_bytes(const string_view str) noexcept
      -> size_type {
        return required_bytes(size_type(str.size()));
    }

    /// @brief Reserves the specified number of bytes in the storage.
    void reserve_bytes(const size_type s) noexcept {
        _str.reserve(std_size(s));
    }

    /// @brief Returns the element at the front of the path.
    [[nodiscard]] auto front() const noexcept -> string_view {
        assert(not empty());
        return string_list::front_value(_str);
    }

    /// @brief Returns the element at the back of the path.
    [[nodiscard]] auto back() const noexcept -> string_view {
        assert(not empty());
        return string_list::back_value(_str);
    }

    /// @brief Indicates if the path starts with the specified entry.
    [[nodiscard]] auto starts_with(const string_view entry) const noexcept
      -> bool {
        return not empty() and (front() == entry);
    }

    /// @brief Indicates if the path ends with the specified entry.
    [[nodiscard]] auto ends_with(const string_view entry) const noexcept
      -> bool {
        return not empty() and (back() == entry);
    }

    /// @brief Indicates if the path has a single specified entry.
    [[nodiscard]] auto is(const string_view entry) const noexcept -> bool {
        return (size() == 1) and (back() == entry);
    }

    /// @brief Appends a new element with the specified name to the end.
    /// @see pop_back
    void push_back(const string_view name) {
        string_list::push_back(_str, _fix(name));
        ++_size;
    }

    void push_back_elem(const string_list::element& elem) noexcept {
        append_to(elem, _str);
        ++_size;
    }

    /// @brief Removes a single element from the end of this path.
    /// @see push_back
    /// @see parent
    void pop_back() noexcept {
        assert(not empty());
        string_list::pop_back(_str);
        --_size;
    }

    /// @brief Returns a new path that is a copy of this without the last n elements.
    /// @see pop_back
    [[nodiscard]] auto parent(span_size_t n = 1) const noexcept {
        basic_string_path result{*this};
        while((n-- > 0) and not empty()) {
            result.pop_back();
        }
        return result;
    }

    /// @brief Returns an iterator pointing to the start of the path.
    /// @see end
    /// @see rbegin
    [[nodiscard]] auto begin() const noexcept -> iterator {
        return empty() ? iterator(nullptr) : iterator(_str.data());
    }

    /// @brief Returns an iterator pointing past the end of the path.
    /// @see begin
    /// @see rend
    [[nodiscard]] auto end() const noexcept -> iterator {
        return empty() ? iterator(nullptr)
                       : iterator(_str.data() + _str.size());
    }

    /// @brief Returns a reverse iterator pointing to the end of the path.
    /// @see rend
    /// @see begin
    [[nodiscard]] auto rbegin() const noexcept -> reverse_iterator {
        return empty() ? reverse_iterator(nullptr)
                       : reverse_iterator(_str.data() + _str.size() - 1);
    }

    /// @brief Returns an iterator pointing past the start of the path.
    /// @see rbegin
    /// @see end
    [[nodiscard]] auto rend() const noexcept -> reverse_iterator {
        return empty() ? reverse_iterator(nullptr)
                       : reverse_iterator(_str.data() - 1);
    }

    [[nodiscard]] auto operator[](span_size_t idx) const noexcept
      -> iterator::reference {
        assert(idx < size());
        auto it{begin()};
        std::advance(it, idx);
        return *it;
    }

    /// @brief Calls the specified function for each element of this path.
    template <typename Func>
    void for_each_elem(Func func) const {
        string_list::for_each_elem(memory::view(_str), func);
    }

    template <typename Func>
    void for_each(Func func) const {
        string_list::for_each(view(_str), func);
    }

    /// @brief Calls the specified function for each element in reverse order.
    template <typename Func>
    void rev_for_each_elem(Func func) const {
        string_list::rev_for_each_elem(memory::view(_str), func);
    }

    template <typename Func>
    void rev_for_each(Func func) const {
        string_list::rev_for_each(memory::view(_str), func);
    }

    /// @brief Returns this path as string with elements separated by @p sep.
    [[nodiscard]] auto as_string(const string_view sep, const bool trail_sep)
      const noexcept -> std::string {
        return string_list::join(view(_str), sep, trail_sep);
    }

    /// @brief Returns this path as string with elements separated by '/'
    [[nodiscard]] auto as_string() const noexcept -> std::string {
        return as_string({"/"}, false);
    }

    /// @brief Returns a block covering the internal representation of this path.
    [[nodiscard]] auto block() noexcept -> memory::const_block {
        return as_bytes(view(_str));
    }

private:
    span_size_t _size{0};
    std::string _str{};

    template <typename Str>
    void _init(const memory::span<Str> names) noexcept {
        span_size_t len = 2 * names.size();
        for(const auto& name : names) {
            len += span_size(name.size());
        }
        _str.reserve(std_size(len));

        for(const auto& name : names) {
            push_back(name);
        }
        assert(span_size(_str.size()) == len);
    }

    static auto _fix(string_view str) noexcept -> string_view {
        while(not str.empty()) {
            if(str.back() == '\0') {
                str = string_view(str.data(), str.size() - 1);
            } else {
                break;
            }
        }
        return str;
    }

    template <typename... Str>
    static auto _pack_names(const Str&... n) noexcept
      -> std::array<string_view, sizeof...(Str)> {
        return {{_fix(n)...}};
    }
};
//------------------------------------------------------------------------------
export [[nodiscard]] auto to_string(const basic_string_path& path)
  -> std::string {
    return path.as_string();
}
//------------------------------------------------------------------------------
export auto operator<<(std::ostream& out, const basic_string_path& path)
  -> std::ostream& {
    return out << to_string(path);
}
//------------------------------------------------------------------------------
} // namespace eagine
