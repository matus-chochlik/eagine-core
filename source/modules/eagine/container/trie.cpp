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
import eagine.core.types;
import eagine.core.memory;
import <cstdint>;
import <algorithm>;
import <array>;
import <optional>;
import <vector>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
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
    static constexpr auto do_get_char_index(
      char,
      index_constant<Ofs>,
      index_constant<Len>,
      index_constant<Len>) noexcept -> std::optional<std::size_t> {
        return {};
    }

    template <memory::string_literal Chars, std::size_t Ofs>
    static constexpr auto do_get_char_index(
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
    static constexpr auto do_get_char_index(
      char c,
      index_constant<Ofs>,
      index_constant<Piv>,
      index_constant<Len>) noexcept -> std::optional<std::size_t> {
        if(c < chars[Ofs + Piv]) {
            if constexpr(Piv > 0) {
                return do_get_char_index<chars>(
                  c,
                  index_constant<Ofs>{},
                  index_constant<_pivot<Piv>()>{},
                  index_constant<Piv>{});
            } else {
                return {};
            }
        } else if(c > chars[Ofs + Piv]) {
            if constexpr(Piv + 1 < Len) {
                return do_get_char_index<chars>(
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
    static constexpr auto get_char_index(char c) noexcept
      -> std::optional<std::size_t> {
        return do_get_char_index<chars>(
          c,
          index_constant<0>{},
          index_constant<_pivot<chars.size() - 1U>()>{},
          index_constant<chars.size() - 1U>{});
    }
};
//------------------------------------------------------------------------------
export template <memory::string_literal Chars, std::size_t N, typename Value>
class basic_trie_impl : private basic_trie_utils {
    static_assert(std::is_sorted(Chars.begin(), Chars.begin() + N - 1));
    using key_type = std::array<std::uint32_t, N>;
    using node_type = std::tuple<key_type, Value>;

    auto _find_insert_pos(const string_view str) const noexcept
      -> std::tuple<std::size_t, span_size_t, bool> {
        std::size_t iidx{0U};
        span_size_t offs{0};
        bool can_insert{true};
        while(offs < str.size()) {
            if(const auto cidx{this->get_char_index<Chars>(str[offs])})
              [[likely]] {
                const auto next{std_size(std::get<0>(_nodes[iidx])[*cidx])};
                if(next == 0) {
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

public:
    constexpr basic_trie_impl(Value v) noexcept
      : _default{v} {
        _nodes.emplace_back(node_type{});
    }

    basic_trie_impl() noexcept
      : basic_trie_impl{Value{}} {}

    auto insert(const string_view str, Value value = {}) noexcept -> bool {
        auto [iidx, offs, can_insert] = _find_insert_pos(str);
        if(can_insert) {
            while(offs < str.size()) {
                const auto next{_nodes.size()};
                _nodes.emplace_back(node_type{});
                if(const auto cidx{this->get_char_index<Chars>(str[offs])}) {
                    std::get<0>(_nodes[iidx])[*cidx] =
                      limit_cast<std::uint32_t>(next);
                    iidx = next;
                    ++offs;
                } else {
                    can_insert = false;
                    break;
                }
            }
            if(offs == str.size()) {
                std::get<1>(_nodes[iidx]) = std::move(value);
            }
        }
        return can_insert;
    }

    auto add(const string_view str, Value value = {}) noexcept
      -> basic_trie_impl& {
        insert(str, std::move(value));
        return *this;
    }

    auto contains(const string_view str) const noexcept -> bool {
        auto [iidx, offs, can_insert] = _find_insert_pos(str);
        (void)iidx;
        return (can_insert && (offs == str.size()));
    }

    auto find(const string_view str) const noexcept
      -> optional_reference_wrapper<const Value> {
        auto [fidx, offs, can_insert] = _find_insert_pos(str);
        if(can_insert && (offs == str.size())) {
            return {std::get<1>(_nodes[fidx])};
        }
        return {nothing};
    }

    auto underlying() const noexcept -> span<const node_type> {
        return view(_nodes);
    }

private:
    Value _default{};
    std::vector<node_type> _nodes;
};
//------------------------------------------------------------------------------
export template <memory::string_literal chars, typename Value = nothing_t>
using basic_trie = basic_trie_impl<chars, chars.size(), Value>;
//------------------------------------------------------------------------------
export template <typename Value = nothing_t>
using basic_lc_identifier_trie =
  basic_trie<"0123456789_abcdefghijklmnopqrstuvwxyz", Value>;
export template <typename Value = nothing_t>
using basic_identifier_trie = basic_trie<
  "0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
  Value>;

export using identifier_trie = basic_identifier_trie<>;
//------------------------------------------------------------------------------
} // namespace eagine
