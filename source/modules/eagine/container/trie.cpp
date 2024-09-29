/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:trie;
import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Common base class for trie implementation
/// @ingroup container
/// @note Do not use this class directly, use basic_trie instead.
class basic_trie_utils {
public:
    using char_index_t = std::uint8_t;
    static constexpr auto invalid_char_index() noexcept -> char_index_t {
        return std::numeric_limits<char_index_t>::max();
    }

private:
    template <std::size_t I>
    using index_constant = std::integral_constant<std::size_t, I>;

    template <std::size_t N>
    static constexpr auto _pivot() noexcept -> std::size_t {
        if constexpr(N == 0) {
            return 0;
        } else if constexpr(N == 1) {
            return 0;
        } else {
            return N / 2;
        }
    }

    template <memory::string_literal chars, std::size_t Ofs, std::size_t Len>
    static constexpr auto _do_find_char_index(
      const char,
      index_constant<Ofs>,
      index_constant<Len>,
      index_constant<Len>) noexcept -> char_index_t {
        return invalid_char_index();
    }

    template <memory::string_literal chars, std::size_t Ofs>
    static constexpr auto _do_find_char_index(
      const char c,
      index_constant<Ofs>,
      index_constant<0>,
      index_constant<1>) noexcept -> char_index_t {
        if(c == chars[Ofs]) {
            return static_cast<char_index_t>(Ofs);
        }
        return invalid_char_index();
    }

    template <
      memory::string_literal chars,
      std::size_t Ofs,
      std::size_t Piv,
      std::size_t Len>
    static constexpr auto _do_find_char_index(
      const char c,
      index_constant<Ofs>,
      index_constant<Piv>,
      index_constant<Len>) noexcept -> char_index_t {
        if(c < chars[Ofs + Piv]) {
            if constexpr(Piv > 0) {
                return _do_find_char_index<chars>(
                  c,
                  index_constant<Ofs>{},
                  index_constant<_pivot<Piv>()>{},
                  index_constant<Piv>{});
            } else {
                return invalid_char_index();
            }
        } else if(c > chars[Ofs + Piv]) {
            if constexpr(Piv + 1 < Len) {
                return _do_find_char_index<chars>(
                  c,
                  index_constant<Ofs + Piv + 1>{},
                  index_constant<_pivot<Len - Piv - 1>()>{},
                  index_constant<Len - Piv - 1>{});
            } else {
                return invalid_char_index();
            }
        } else {
            return static_cast<char_index_t>(Ofs + Piv);
        }
    }
    template <memory::string_literal chars>
    [[nodiscard]] static constexpr auto _find_char_index(const char c) noexcept
      -> char_index_t {
        return _do_find_char_index<chars>(
          c,
          index_constant<0>{},
          index_constant<_pivot<chars.size() - 1U>()>{},
          index_constant<chars.size() - 1U>{});
    }

    template <memory::string_literal chars, std::size_t... I>
    static consteval auto _get_char_map(std::index_sequence<I...>) noexcept
      -> std::array<char_index_t, sizeof...(I)> {
        return {{_find_char_index<chars>(char(I))...}};
    }

public:
    template <memory::string_literal chars>
    static constexpr auto is_valid_char(const char c) noexcept -> bool {
        return _find_char_index<chars>(c) != invalid_char_index();
    }

    template <memory::string_literal chars, std::size_t N>
    static constexpr auto get_char_map() noexcept {
        return _get_char_map<chars>(std::make_index_sequence<N>{});
    }
};
//------------------------------------------------------------------------------
export template <typename Value, typename Index, std::size_t N>
struct basic_trie_node_type {
    std::array<Index, N> next{};
    std::optional<Value> object{};
};
//------------------------------------------------------------------------------
/// @brief Implementation template for basic_trie.
/// @ingroup container
/// @note Do not use directly, use basic_trie instead.
export template <
  memory::string_literal chars,
  std::size_t N,
  typename Value,
  typename Index = std::uint32_t,
  typename Container = std::vector<basic_trie_node_type<Value, Index, N>>>
class basic_trie_impl {
    static_assert(std::is_sorted(chars.begin(), chars.begin() + N - 1));

    using key_type = std::array<Index, N>;

    using node_type = basic_trie_node_type<Value, Index, N>;

    static constexpr auto _whole_key_done(
      const string_view key,
      const span_size_t offs) noexcept -> bool {
        return offs == key.size();
    }

    constexpr auto _get_char_index(const char c) const noexcept
      -> std::optional<std::size_t> {
        const auto idx{_char_map[static_cast<std::size_t>(c)]};
        if(idx != basic_trie_utils::invalid_char_index()) [[likely]] {
            return {static_cast<std::size_t>(idx)};
        }
        return {};
    }

    static constexpr auto _is_no_index(const std::size_t idx) noexcept {
        return idx == 0;
    }

    constexpr auto _is_valid_index(const std::size_t idx) const noexcept {
        return idx < _nodes.size();
    }

    auto _find_insert_pos(const string_view key) const noexcept
      -> std::tuple<std::size_t, span_size_t, bool> {
        std::size_t iidx{0U};
        span_size_t offs{0};
        bool can_insert{true};

        while(not _whole_key_done(key, offs)) {
            if(const auto cidx{_get_char_index(key[offs])}) [[likely]] {
                const auto next{std_size(_nodes[iidx].next[*cidx])};
                if(_is_no_index(next)) {
                    break;
                } else {
                    assert(_is_valid_index(next));
                    iidx = next;
                    ++offs;
                }
            } else {
                can_insert = false;
                break;
            }
        }
        return {iidx, offs, can_insert};
    }

    auto _get_next_idx(const std::size_t cidx) noexcept -> std::size_t {
        const std::size_t nidx{_nodes.size()};
        _nodes.emplace_back(node_type{});
        return nidx;
    }

    template <typename Function, typename Nodes>
    static void _do_traverse(Function&& function, Nodes& nodes) noexcept {
        std::string str;
        str.reserve(16U);
        std::vector<std::array<std::size_t, 2U>> idx;
        idx.reserve(16U);
        idx.push_back({0U, 0U});
        while(not idx.empty()) {
            auto& [cidx, nidx] = idx.back();
            while(cidx < chars.size()) {
                if(not _is_no_index(nodes[nidx].next[cidx])) {
                    break;
                }
                ++cidx;
            }
            if(nodes[nidx].object.has_value()) {
                function(string_view{str}, *nodes[nidx].object);
            }
            if(cidx == chars.size()) {
                str.pop_back();
                idx.pop_back();
                if(not idx.empty()) {
                    ++idx.back()[0];
                }
            } else {
                idx.push_back({0U, nodes[nidx].next[cidx]});
                str.push_back(chars[cidx]);
            }
        }
    }

public:
    /// @brief Default constructor.
    basic_trie_impl() noexcept {
        _nodes.emplace_back(node_type{});
    }

    /// @brief Returns a range of chars that valid keys that this trie can use.
    /// @see is_valid_char
    [[nodiscard]] static constexpr auto valid_chars() noexcept -> string_view {
        return chars;
    }

    /// @brief Indicates if the specified char can be used by this trie.
    /// @see valid_chars
    /// @see is_valid_key
    [[nodiscard]] static constexpr auto is_valid_char(const char c) noexcept
      -> bool {
        return basic_trie_utils::is_valid_char<chars>(c);
    }

    /// @brief Indicates if the specified string can be used as key by this trie.
    /// @see valid_chars
    /// @see is_valid_char
    [[nodiscard]] static constexpr auto is_valid_key(
      const string_view key) noexcept -> bool {
        return std::all_of(key.begin(), key.end(), [](const char c) {
            return is_valid_char(c);
        });
    }

    /// @brief Reserves space for the specified count of nodes.
    /// @see add
    auto reserve(span_size_t count) -> auto& {
        _nodes.reserve(std_size(count));
        return *this;
    }

    /// @brief Inserts a new value under the specified key string.
    /// @see add
    /// @see valid_chars
    /// The return value indicates if the value was successfully inserted.
    auto insert(const string_view key, Value value = {}) noexcept -> bool {
        auto [iidx, offs, can_insert] = _find_insert_pos(key);
        if(can_insert) {
            while(not _whole_key_done(key, offs)) {
                const auto cidx{_get_char_index(key[offs])};
                if(not cidx) [[unlikely]] {
                    can_insert = false;
                    break;
                }
                const auto next{_get_next_idx(*cidx)};
                _nodes[iidx].next[*cidx] = limit_cast<Index>(next);
                iidx = next;
                ++offs;
            }
            if(_whole_key_done(key, offs)) [[likely]] {
                _nodes[iidx].object = std::move(value);
            }
        }
        return can_insert;
    }

    /// @brief Inserts a new value under the specified key string.
    /// @see insert
    /// @see valid_chars
    auto add(const string_view key, Value value = {}) noexcept
      -> basic_trie_impl& {
        insert(key, std::move(value));
        return *this;
    }

    /// @brief Checks if the specified key is contained in this trie.
    [[nodiscard]] auto contains(const string_view key) const noexcept -> bool {
        const auto [iidx, offs, is_valid] = _find_insert_pos(key);
        if(is_valid and _whole_key_done(key, offs)) {
            return _nodes[iidx].object.has_value();
        }
        return false;
    }

    /// @brief Tries to find the value stored under the specified key.
    [[nodiscard]] auto find(const string_view key) const noexcept
      -> optional_reference<const Value> {
        const auto [fidx, offs, is_valid] = _find_insert_pos(key);
        if(is_valid and _whole_key_done(key, offs)) {
            return {_nodes[fidx].object};
        }
        return {nothing};
    }

    /// @brief Marks the value stored under the specified key as removed.
    [[nodiscard]] auto remove(const string_view key) noexcept -> bool {
        const auto [fidx, offs, is_valid] = _find_insert_pos(key);
        if(is_valid and _whole_key_done(key, offs)) {
            if(auto& object{_nodes[fidx].object}) {
                object.reset();
                return true;
            }
        }
        return false;
    }

    template <typename Function>
    void traverse(Function&& function) noexcept {
        _do_traverse(std::forward<Function>(function), _nodes);
    }

    template <typename Function>
    void traverse(Function&& function) const noexcept {
        _do_traverse(std::forward<Function>(function), _nodes);
    }

    [[nodiscard]] auto underlying() const noexcept -> span<const node_type> {
        return view(_nodes);
    }

    [[nodiscard]] auto node_count() const noexcept -> span_size_t {
        return span_size(_nodes.size());
    }

    [[nodiscard]] auto value_node_count() const noexcept -> span_size_t {
        return span_size(
          std::count_if(_nodes.begin(), _nodes.end(), [](const auto& n) {
              return n.object.has_value();
          }));
    }

    [[nodiscard]] auto internal_node_count() const noexcept -> span_size_t {
        return span_size(
          std::count_if(_nodes.begin(), _nodes.end(), [](const auto& n) {
              return not n.object.has_value();
          }));
    }

    [[nodiscard]] auto overhead() const noexcept -> float {
        if(const auto c{value_node_count()}; c > 0) {
            return float(internal_node_count()) / float(c);
        }
        return 1.F;
    }

    [[nodiscard]] auto storage_bytes() const noexcept -> std::size_t {
        return sizeof(_nodes) + sizeof(node_type) * _nodes.size();
    }

private:
    std::array<std::uint8_t, 128U> _char_map{
      basic_trie_utils::get_char_map<chars, 128U>()};
    Container _nodes;
};
//------------------------------------------------------------------------------
/// @brief Template for tries, string dictionaries optionally mapping to values.
/// @ingroup container
export template <memory::string_literal chars, typename Value = nothing_t>
using basic_trie = basic_trie_impl<chars, chars.size(), Value>;
//------------------------------------------------------------------------------
export template <typename Value = nothing_t>
using basic_dec_identifier_trie = basic_trie<"0123456789", Value>;

export template <typename Value = nothing_t>
using basic_lc_identifier_trie =
  basic_trie<"0123456789_abcdefghijklmnopqrstuvwxyz", Value>;

export template <typename Value = nothing_t>
using basic_identifier_trie = basic_trie<
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz",
  Value>;

export using identifier_trie = basic_identifier_trie<>;
//------------------------------------------------------------------------------
} // namespace eagine
