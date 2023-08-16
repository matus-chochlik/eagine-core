/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
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
export class basic_trie_utils {
    template <std::size_t I>
    using index_constant = std::integral_constant<std::size_t, I>;

    template <std::size_t N>
    static consteval auto _pivot() noexcept -> std::size_t {
        if constexpr(N == 0) {
            return 0;
        } else if constexpr(N == 1) {
            return 0;
        } else {
            return N / 2;
        }
    }

    template <memory::string_literal Chars, std::size_t Ofs, std::size_t Len>
    static constexpr auto do_find_char_index(
      char,
      index_constant<Ofs>,
      index_constant<Len>,
      index_constant<Len>) noexcept -> std::optional<std::size_t> {
        return {};
    }

    template <memory::string_literal Chars, std::size_t Ofs>
    static constexpr auto do_find_char_index(
      char c,
      index_constant<Ofs>,
      index_constant<0>,
      index_constant<1>) noexcept -> std::optional<std::size_t> {
        if(c == Chars[Ofs]) {
            return Ofs;
        }
        return {};
    }

    template <
      memory::string_literal chars,
      std::size_t Ofs,
      std::size_t Piv,
      std::size_t Len>
    static constexpr auto do_find_char_index(
      char c,
      index_constant<Ofs>,
      index_constant<Piv>,
      index_constant<Len>) noexcept -> std::optional<std::size_t> {
        if(c < chars[Ofs + Piv]) {
            if constexpr(Piv > 0) {
                return do_find_char_index<chars>(
                  c,
                  index_constant<Ofs>{},
                  index_constant<_pivot<Piv>()>{},
                  index_constant<Piv>{});
            } else {
                return {};
            }
        } else if(c > chars[Ofs + Piv]) {
            if constexpr(Piv + 1 < Len) {
                return do_find_char_index<chars>(
                  c,
                  index_constant<Ofs + Piv + 1>{},
                  index_constant<_pivot<Len - Piv - 1>()>{},
                  index_constant<Len - Piv - 1>{});
            } else {
                return {};
            }
        } else {
            return {Ofs + Piv};
        }
    }

public:
    template <memory::string_literal chars>
    [[nodiscard]] static constexpr auto get_char_index(char c) noexcept
      -> std::optional<std::size_t> {
        return do_find_char_index<chars>(
          c,
          index_constant<0>{},
          index_constant<_pivot<chars.size() - 1U>()>{},
          index_constant<chars.size() - 1U>{});
    }
};
//------------------------------------------------------------------------------
/// @brief Implementation template for basic_trie.
/// @ingroup container
/// @note Do not use directly, use basic_trie instead.
export template <
  memory::string_literal Chars,
  std::size_t N,
  typename Value,
  typename Index = std::uint32_t>
class basic_trie_impl : private basic_trie_utils {
    static_assert(std::is_sorted(Chars.begin(), Chars.begin() + N - 1));

    using key_type = std::array<Index, N>;
    struct node_type {
        key_type next{};
        std::optional<Value> object{};
    };

    static constexpr auto _whole_key_done(
      const string_view key,
      span_size_t offs) noexcept -> bool {
        return offs == key.size();
    }

    constexpr auto _get_char_index(const char c) const noexcept {
        return basic_trie_utils::get_char_index<Chars>(c);
    }

    static constexpr auto _is_no_index(const Index idx) noexcept {
        return idx == 0;
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
                    assert(next < _nodes.size());
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

public:
    /// @brief Default constructor.
    basic_trie_impl() noexcept {
        _nodes.emplace_back(node_type{});
    }

    /// @brief Returns a range of chars that valid keys for this trie can use.
    [[nodiscard]] constexpr auto valid_chars() const noexcept -> string_view {
        return Chars;
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
        auto [iidx, offs, is_valid] = _find_insert_pos(key);
        if(is_valid and _whole_key_done(key, offs)) {
            return _nodes[iidx].object.has_value();
        }
        return false;
    }

    /// @brief Tries to find the value stored under the specified key.
    [[nodiscard]] auto find(const string_view key) const noexcept
      -> optional_reference<const Value> {
        auto [fidx, offs, is_valid] = _find_insert_pos(key);
        if(is_valid and _whole_key_done(key, offs)) {
            return {_nodes[fidx].object};
        }
        return {nothing};
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
    std::vector<node_type> _nodes;
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
