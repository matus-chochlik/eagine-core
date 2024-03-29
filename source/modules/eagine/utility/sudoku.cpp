/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.utility:sudoku;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.container;

namespace eagine {
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_glyph;
export template <unsigned S>
class basic_sudoku_board;
export template <unsigned S>
class basic_sudoku_board_generator;
export template <unsigned S>
class basic_sudoku_tile_patch;
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_board_traits {
public:
    static constexpr const unsigned rank = S;
    static constexpr const unsigned glyph_count = S * S;

    using This = basic_sudoku_board_traits;
    using generator = basic_sudoku_board_generator<S>;
    using board_type = basic_sudoku_board<S>;

    basic_sudoku_board_traits() noexcept = default;

    basic_sudoku_board_traits(
      std::array<std::string, glyph_count> glyphs,
      std::array<std::string, S> multi_glyphs,
      std::string empty_glyph) noexcept
      : _glyphs{std::move(glyphs)}
      , _multi_glyphs{std::move(multi_glyphs)}
      , _empty_glyph{std::move(empty_glyph)} {}

    auto make_diagonal() const -> board_type;
    auto make_generator() const -> generator;

    auto to_string(const basic_sudoku_glyph<S>& glyph) const
      -> optional_reference<const std::string>;

    auto print(std::ostream&, const basic_sudoku_glyph<S>& glyph) const
      -> std::ostream&;

    auto print_empty(std::ostream& out) const -> std::ostream& {
        return out << _empty_glyph;
    }

    auto print(std::ostream&, const basic_sudoku_board<S>& board) const
      -> std::ostream&;

private:
    std::array<std::string, glyph_count> _glyphs{};
    std::array<std::string, S> _multi_glyphs{};
    std::string _empty_glyph{};
};
//------------------------------------------------------------------------------
export template <unsigned S>
class default_sudoku_board_traits;

export template <>
class default_sudoku_board_traits<2> : public basic_sudoku_board_traits<2> {
public:
    // clang-format off
    default_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<2>{
          {"1","2","3","4"},
          { "?", "?"}, "."} {}
    // clang-format on
};

export template <>
class default_sudoku_board_traits<3> : public basic_sudoku_board_traits<3> {
public:
    // clang-format off
    default_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<3>{
          {"1","2","3","4","5","6","7","8","9"},
          {"?", "?", "?"}, "."} {}
    // clang-format on
};

export template <>
class default_sudoku_board_traits<4> : public basic_sudoku_board_traits<4> {
public:
    // clang-format off
    default_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<4>{
          {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"},
          {"?", "?", "?", "?"}, "."} {}
    // clang-format on
};

export template <>
class default_sudoku_board_traits<5> : public basic_sudoku_board_traits<5> {
public:
    // clang-format off
    default_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<5>{
          {"A","B","C","D","E","F","G","H","I","J","K","L","M"
          ,"N","O","P","Q","R","S","T","U","V","W","X","Y"},
          {"?", "?", "?", "?", "?"}, "."} {}
    // clang-format on
};

export template <>
class default_sudoku_board_traits<6> : public basic_sudoku_board_traits<6> {
public:
    // clang-format off
    default_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<6>{
          {"0","1","2","3","4","5","6","7","8","9","A","B"
          ,"C","D","E","F","G","H","I","J","K","L","M","N"
          ,"O","P","Q","R","S","T","U","V","W","X","Y","Z"},
          {"?", "?", "?", "?", "?", "?"}, "."} {}
    // clang-format on
};
//------------------------------------------------------------------------------
export template <unsigned S>
class block_sudoku_board_traits;

export template <>
class block_sudoku_board_traits<2> : public basic_sudoku_board_traits<2> {
public:
    // clang-format off
    block_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<2>{
          {"▖","▗","▘","▝"},
          { "?", "?"}, "."} {}
    // clang-format on
};

export template <>
class block_sudoku_board_traits<3> : public basic_sudoku_board_traits<3> {
public:
    // clang-format off
    block_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<3>{
            {" ","▀","▄","▖","▗","▘","▝","▚","▞"},
            {"?","?","?"}, "."} {}
    // clang-format on
};

export template <>
class block_sudoku_board_traits<4> : public basic_sudoku_board_traits<4> {
public:
    // clang-format off
    block_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<4>{
            {" ","▀","▄","█","▌","▐","▖","▗","▘","▙","▚","▛","▜","▝","▞","▟"},
            {"?","?","?","?"}, "."} {}
    // clang-format on
};

export template <>
class block_sudoku_board_traits<5> : public basic_sudoku_board_traits<5> {
public:
    // clang-format off
    block_sudoku_board_traits() noexcept
      : basic_sudoku_board_traits<5>{
          {"▁","▂","▃","▄","▅","▆","▇","▏","▎","▍","▌","▋","▊"
          ,"▉","▐","▖","▗","▘","▙","▚","▛","▜","▝","▞","▟"},
          {"?", "?", "?", "?", "?"}, "."} {}
    // clang-format on
};
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_glyph {
public:
    using board_traits = basic_sudoku_board_traits<S>;
    using cell_type = std::conditional_t<
      (board_traits::glyph_count > 32U),
      std::uint64_t,
      std::conditional_t<
        (board_traits::glyph_count > 16),
        std::uint32_t,
        std::conditional_t<
          (board_traits::glyph_count > 8),
          std::uint16_t,
          std::uint8_t>>>;

    static constexpr const unsigned glyph_count = board_traits::glyph_count;

    static constexpr auto to_cell_type(const unsigned index) noexcept {
        assert(index < glyph_count);
        return cell_type(1U << index);
    }

    constexpr basic_sudoku_glyph() noexcept = default;
    constexpr basic_sudoku_glyph(const unsigned index) noexcept
      : _cell_val{to_cell_type(index)} {}

    constexpr auto is_empty() const noexcept {
        return _cell_val == 0U;
    }

    constexpr auto is_single() const noexcept {
        return _is_pot(_cell_val);
    }

    constexpr auto is_multiple() const noexcept {
        return not is_empty() and not is_single();
    }

    auto cell_value() const noexcept -> cell_type {
        return _cell_val;
    }

    auto get_index() const noexcept -> unsigned {
        assert(is_single());
        for(const auto index : integer_range(glyph_count)) {
            if(to_cell_type(index) == _cell_val) {
                return index;
            }
        }
        return S;
    }

    constexpr auto set(const unsigned index) noexcept -> auto& {
        assert(index < glyph_count);
        _cell_val = to_cell_type(index);
        return *this;
    }

    constexpr auto add(const unsigned index) noexcept -> auto& {
        assert(index < glyph_count);
        _cell_val |= to_cell_type(index);
        return *this;
    }

    template <typename Function>
    void for_each_alternative(Function func) const noexcept {
        for(const auto index : integer_range(glyph_count)) {
            const auto mask = to_cell_type(index);
            if((_cell_val & mask) == mask) {
                func(index);
            }
        }
    }

    auto alternative_count() const noexcept -> unsigned {
        unsigned count = 0U;
        cell_type bits = _cell_val;
        while(bits) {
            bits &= (bits - 1U);
            ++count;
        }
        return count;
    }

private:
    friend class basic_sudoku_board<S>;

    struct from_cell_value_tag {};
    constexpr basic_sudoku_glyph(
      const cell_type cell_val,
      from_cell_value_tag) noexcept
      : _cell_val{cell_val} {}

    static constexpr auto _is_pot(const cell_type v) noexcept {
        return (v != 0U) and ((v & (v - 1U)) == 0U);
    }

    cell_type _cell_val{0U};
};
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_board {
public:
    using This = basic_sudoku_board;
    using board_traits = basic_sudoku_board_traits<S>;
    using glyph_type = basic_sudoku_glyph<S>;
    using cell_type = typename glyph_type::cell_type;
    using coord_type = std::array<unsigned, 4>;
    static constexpr const unsigned glyph_count = board_traits::glyph_count;

    using block_type = std::array<cell_type, glyph_count>;
    using blocks_type = std::array<block_type, glyph_count>;

    static constexpr auto invalid_coord() noexcept -> coord_type {
        return {{S, S, S, S}};
    }

    template <typename Function>
    void for_each_coord(Function func) const noexcept {
        for(const auto by : integer_range(S)) {
            for(const auto bx : integer_range(S)) {
                for(const auto cy : integer_range(S)) {
                    for(const auto cx : integer_range(S)) {
                        if(not func(coord_type{{bx, by, cx, cy}})) {
                            return;
                        }
                    }
                }
            }
        }
    }

    basic_sudoku_board() noexcept {
        for_each_coord([&](const auto& coord) {
            set(coord, glyph_type());
            return true;
        });
    }

    auto blocks() noexcept -> blocks_type& {
        return _blocks;
    }

    auto blocks() const noexcept -> const blocks_type& {
        return _blocks;
    }

    auto is_possible(const coord_type& coord, const glyph_type value)
      const noexcept {
        const auto [cbx, cby, ccx, ccy] = coord;
        const auto cell_val = value.cell_value();

        for(const auto cy : integer_range(S)) {
            for(const auto cx : integer_range(S)) {
                if((cx != ccx) or (cy != ccy)) {
                    if(_cell_val({cbx, cby, cx, cy}) == cell_val) {
                        return false;
                    }
                }
            }
        }

        for(const auto bx : integer_range(S)) {
            for(const auto cx : integer_range(S)) {
                if((bx != cbx) or (cx != ccx)) {
                    if(_cell_val({bx, cby, cx, ccy}) == cell_val) {
                        return false;
                    }
                }
            }
        }

        for(const auto by : integer_range(S)) {
            for(const auto cy : integer_range(S)) {
                if((by != cby) or (cy != ccy)) {
                    if(_cell_val({cbx, by, ccx, cy}) == cell_val) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    auto is_solved() const noexcept {
        bool result = true;
        for_each_coord([&](const auto& coord) {
            if(const auto value{get(coord)}; not value.is_single()) {
                result = false;
                return false;
            }
            return true;
        });
        return result;
    }

    auto has_empty() const noexcept {
        bool result = false;
        for_each_coord([&](const auto& coord) {
            if(get(coord).is_empty()) {
                result = true;
                return false;
            }
            return true;
        });
        return result;
    }

    auto get(const coord_type& coord) const noexcept -> glyph_type {
        return {_cell_val(coord), typename glyph_type::from_cell_value_tag{}};
    }

    auto set(const coord_type& coord, const glyph_type value) noexcept
      -> auto& {
        _cell_ref(coord) = value.cell_value();
        return *this;
    }

    auto set_available_alternatives(const coord_type& coord) noexcept -> auto& {
        glyph_type alternatives;
        for(const auto value : integer_range(glyph_count)) {
            if(is_possible(coord, value)) {
                alternatives.add(value);
            }
        }
        return set(coord, alternatives);
    }

    auto calculate_alternatives() noexcept -> auto& {
        for_each_coord([&](const auto& coord) {
            if(not get(coord).is_single()) {
                set_available_alternatives(coord);
            }
            return true;
        });
        return *this;
    }

    auto find_unsolved() const noexcept -> coord_type {
        auto result = invalid_coord();
        auto min_alt = glyph_count + 1;
        for_each_coord([&](const auto& coord) {
            if(const auto num_alt{get(coord).alternative_count()}) {
                if((num_alt > 1) and (min_alt > num_alt)) {
                    min_alt = num_alt;
                    result = coord;
                }
            }
            return true;
        });
        return result;
    }

    template <typename Function>
    void for_each_alternative(const coord_type& coord, Function func) const {
        if(coord != invalid_coord()) {
            get(coord).for_each_alternative([&](glyph_type alt) {
                auto candidate = This(*this).set(coord, alt);
                if(candidate.is_possible(coord, alt)) {
                    if(not candidate.calculate_alternatives().has_empty()) {
                        func(candidate);
                    }
                }
            });
        }
    }

    auto alternative_count() const noexcept -> unsigned {
        unsigned count = 0U;
        for_each_coord([&](const auto& coord) {
            const auto& cell = get(coord);
            if(not cell.is_single()) {
                count += cell.alternative_count();
            }
            return true;
        });
        return count;
    }

    auto get_block(const unsigned bx, const unsigned by) const noexcept
      -> const block_type& {
        return _block(_blocks, bx, by);
    }

    auto set_block(
      const unsigned bx,
      const unsigned by,
      const block_type& block) noexcept -> auto& {
        _block(_blocks, bx, by) = block;
        return *this;
    }

private:
    auto _cell_val(const coord_type& coord) const noexcept {
        const auto [bx, by, cx, cy] = coord;
        return _cell(_block(_blocks, bx, by), cx, cy);
    }

    auto _cell_ref(const coord_type& coord) noexcept -> auto& {
        const auto [bx, by, cx, cy] = coord;
        return _cell(_block(_blocks, bx, by), cx, cy);
    }

    template <typename B>
    static auto _cell(B& block, const unsigned x, const unsigned y) noexcept
      -> auto& {
        return block[y * S + x];
    }

    template <typename B>
    static auto _block(B& blocks, const unsigned x, const unsigned y) noexcept
      -> auto& {
        return blocks[y * S + x];
    }

    blocks_type _blocks{};
};
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_board_generator {
public:
    using board_traits = basic_sudoku_board_traits<S>;
    using board_type = basic_sudoku_board<S>;

    basic_sudoku_board_generator(const board_traits& traits)
      : _traits{traits} {}

    auto generate(std::size_t count) -> board_type {
        board_type result{};
        result.calculate_alternatives();

        while(count) {
            const typename board_type::coord_type coord{
              _coord_dist(_rd),
              _coord_dist(_rd),
              _coord_dist(_rd),
              _coord_dist(_rd)};

            if(not result.get(coord).is_single()) {
                while(true) {
                    const auto value{_glyph_dist(_rd)};
                    if(result.is_possible(coord, value)) {
                        result.set(coord, value).calculate_alternatives();
                        --count;
                        break;
                    }
                }
            }
        }

        return result;
    }

    auto generate_one() {
        return generate(1);
    }

    auto generate_few() {
        return generate(S + S);
    }

    auto generate_medium() {
        return generate(S * S + S + S);
    }

    auto generate_many() {
        return generate(S * S * S + S + S + S);
    }

private:
    std::reference_wrapper<const board_traits> _traits;
    std::random_device _rd;
    std::uniform_int_distribution<unsigned> _coord_dist{0, S - 1U};
    std::uniform_int_distribution<unsigned> _glyph_dist{0, S* S - 1U};
};
//------------------------------------------------------------------------------
template <unsigned S>
auto basic_sudoku_board_traits<S>::make_diagonal() const -> board_type {
    board_type result{};
    unsigned g = 0;
    for(const auto b : integer_range(S)) {
        for(const auto c : integer_range(S)) {
            result.set({{b, b, c, c}}, g++);
        }
    }
    return result.calculate_alternatives();
}
//------------------------------------------------------------------------------
template <unsigned S>
auto basic_sudoku_board_traits<S>::make_generator() const -> generator {
    return {*this};
}
//------------------------------------------------------------------------------
template <unsigned S>
auto basic_sudoku_board_traits<S>::to_string(const basic_sudoku_glyph<S>& glyph)
  const -> optional_reference<const std::string> {
    if(glyph.is_single()) {
        return {_glyphs[glyph.get_index()]};
    }
    return {nullptr};
}
//------------------------------------------------------------------------------
template <unsigned S>
auto basic_sudoku_board_traits<S>::print(
  std::ostream& out,
  const basic_sudoku_glyph<S>& glyph) const -> std::ostream& {
    if(glyph.is_single()) {
        out << _glyphs[glyph.get_index()];
    } else if(glyph.is_empty()) {
        out << _empty_glyph;
    } else {
        out << _multi_glyphs[0U];
    }
    return out;
}
//------------------------------------------------------------------------------
template <unsigned S>
auto basic_sudoku_board_traits<S>::print(
  std::ostream& out,
  const basic_sudoku_board<S>& board) const -> std::ostream& {
    for(const auto by : integer_range(S)) {
        for(const auto cy : integer_range(S)) {
            for(const auto bx : integer_range(S)) {
                for(const auto cx : integer_range(S)) {
                    print(out << ' ', board.get({{bx, by, cx, cy}}));
                }
                if(bx + 1 < S) {
                    out << " |";
                }
            }
            out << '\n';
        }
        if(by + 1 < S) {
            for(const auto s1 : integer_range(S)) {
                for(const auto s2 : integer_range(S)) {
                    if(s1 == 0 and s2 == 0) {
                        out << " -";
                    } else {
                        out << "--";
                    }
                }
                if(s1 + 1 < S) {
                    out << "-+";
                }
            }
            out << '\n';
        }
    }
    return out;
}
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_solver {
public:
    using board_type = basic_sudoku_board<S>;

    class solution_state {
    public:
        solution_state(const board_type& board) noexcept {
            _solutions.push_back(board);
        }

        auto is_done() const noexcept -> bool {
            return _done or _solutions.empty();
        }

        auto is_solved() const noexcept -> bool {
            return not _solutions.empty() and _solutions.back().is_solved();
        }

        auto next() -> solution_state& {
            const auto current{_solutions.back()};
            _solutions.pop_back();

            current.for_each_alternative(
              current.find_unsolved(), [this](const auto& candidate) {
                  if(not _done) {
                      if(candidate.is_solved()) {
                          _done = true;
                      }
                      _solutions.push_back(candidate);
                  }
              });
            return *this;
        }

        auto reset(const board_type& board) noexcept -> board_type& {
            _solutions.clear();
            _solutions.push_back(board);
            _done = false;
            return *this;
        }

        auto board() const noexcept -> const board_type& {
            assert(not _solutions.empty());
            return _solutions.back();
        }

    private:
        chunk_list<board_type, 256> _solutions;
        bool _done{false};
    };

    auto solve(board_type board) -> board_type {
        solution_state solution{board};
        while(not solution.is_done()) {
            solution.next();
        }
        return solution.board();
    }
};
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_tiling;
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_tile_patch {
    static_assert(S > 2U);
    static constexpr const span_size_t M = span_size(S * (S - 2));

public:
    using glyph_type = basic_sudoku_glyph<S>;

    basic_sudoku_tile_patch(const span_size_t w, const span_size_t h)
      : _width{w % M ? (1 + w / M) * M : w}
      , _height{h % M ? (1 + w / M) * M : h} {
        assert(_width > 0);
        assert(_height > 0);
        _cells.resize(integer(_width * _height));
    }

    auto width() const noexcept -> span_size_t {
        return _width;
    }

    auto height() const noexcept -> span_size_t {
        return _height;
    }

    auto get(const span_size_t x, const span_size_t y) const noexcept
      -> unsigned {
        assert((x >= 0) and (x < width()));
        assert((y >= 0) and (y < height()));
        return _cells[integer(y * _width + x)];
    }

    auto get_glyph(const span_size_t x, const span_size_t y) const noexcept
      -> glyph_type {
        return {get(x, y)};
    }

    auto print(std::ostream& out) const -> std::ostream& {
        std::size_t k = 0;
        for([[maybe_unused]] const auto y : integer_range(_height)) {
            for([[maybe_unused]] const auto x : integer_range(_width)) {
                assert(k < _cells.size());
                out << std::setw(3) << unsigned(_cells[k++]);
            }
            out << '\n';
        }
        return out;
    }

    auto print(std::ostream& out, const basic_sudoku_board_traits<S>& traits)
      const -> std::ostream& {
        std::size_t k = 0;
        for([[maybe_unused]] const auto y : integer_range(_height)) {
            for([[maybe_unused]] const auto x : integer_range(_width)) {
                assert(k < _cells.size());
                traits.print(out, glyph_type{_cells[k++]});
            }
            out << '\n';
        }
        return out;
    }

    friend auto operator<<(
      std::ostream& out,
      const basic_sudoku_tile_patch& that) -> std::ostream& {
        return that.print(out);
    }

private:
    friend class basic_sudoku_tiling<S>;

    span_size_t _width{0};
    span_size_t _height{0};
    std::vector<std::uint8_t> _cells;
};
//------------------------------------------------------------------------------
export template <unsigned S>
class basic_sudoku_tiling {
    static_assert(S >= 2U);

    using solution_state = typename basic_sudoku_solver<S>::solution_state;

public:
    using board_traits = basic_sudoku_board_traits<S>;
    using board_type = basic_sudoku_board<S>;

    basic_sudoku_tiling(const board_traits& traits)
      : _traits{traits} {
        _do_enqueue(0, 0, traits.make_diagonal());
    }

    basic_sudoku_tiling(const board_traits& traits, board_type board)
      : _traits{traits} {
        _do_enqueue(0, 0, std::move(board));
    }

    auto generate(const int xmin, const int ymin, const int xmax, const int ymax)
      -> auto& {
        for(const auto y : integer_range(ymin, ymax + 1)) {
            for(const auto x : integer_range(xmin, xmax + 1)) {
                _get(x, y);
            }
        }
        return *this;
    }

    class patch_fill_state {
    public:
        patch_fill_state(
          basic_sudoku_tiling<S>& parent,
          basic_sudoku_tile_patch<S>& patch,
          const int xmin,
          const int ymin) noexcept
          : _parent{parent}
          , _patch{patch}
          , _xmin{xmin}
          , _ymin{ymin} {}

        auto current_count() const noexcept {
            return span_size(_k);
        }

        auto total_count() const noexcept {
            return span_size(_patch._cells.size());
        }

        auto is_done() const noexcept -> bool {
            return not(_k < total_count());
        }

        auto next() noexcept -> patch_fill_state& {
            const auto [gx, gy]{_get_cell_coord(_k)};
            const auto [x, y]{_get_board_coord(gx, gy)};
            if(auto board{_parent._get(x, y)}) {
                const auto [bx, by, cx, cy]{_get_in_board_coord(gx, gy)};
                _patch._cells[_k] = limit_cast<std::uint8_t>(
                  board->get({bx, by, cx, cy}).get_index());
                ++_k;
            }
            return *this;
        }

    private:
        static auto _flr_div(auto num, auto den) noexcept -> auto {
            if(num * den > 0) {
                return num / den;
            } else {
                const auto res{std::div(num, den)};
                return (res.rem) ? res.quot - 1 : res.quot;
            }
        }

        static auto _pos_mod(auto num, auto den) noexcept -> auto {
            return ((num % den) + den) % den;
        }

        auto _get_cell_coord(std::size_t k) const noexcept
          -> std::tuple<int, int> {
            return {
              _xmin + int(k) % int(_patch.width()),
              _ymin + int(k) / int(_patch.width())};
        }

        auto _get_board_coord(int kx, int ky) const noexcept
          -> std::tuple<int, int> {
            const auto block_size{int(S)};
            const auto block_per_side{int(S - 1U)};
            const auto cell_per_side{block_size * block_per_side};
            return {_flr_div(kx, cell_per_side), _flr_div(ky, cell_per_side)};
        }

        auto _get_in_board_coord(int kx, int ky) const noexcept
          -> std::tuple<unsigned, unsigned, unsigned, unsigned> {
            const auto block_size{int(S)};
            const auto block_per_side{int(S - 1U)};
            const auto cell_per_side{block_size * block_per_side};
            const auto blkx{_pos_mod(_flr_div(kx, block_size), block_per_side)};
            const auto blky{_pos_mod(_flr_div(ky, block_size), block_per_side)};
            const auto celx{_pos_mod(kx, block_size)};
            const auto cely{_pos_mod(ky, block_size)};
            return {
              unsigned(blkx), unsigned(blky), unsigned(celx), unsigned(cely)};
        }

        basic_sudoku_tiling<S>& _parent;
        basic_sudoku_tile_patch<S>& _patch;
        const int _xmin;
        const int _ymin;
        int _k{0};
    };

    auto fill(const int xmin, const int ymin, basic_sudoku_tile_patch<S>& patch)
      -> basic_sudoku_tile_patch<S>& {
        patch_fill_state state{*this, patch, xmin, ymin};
        while(not state.is_done()) {
            state.next();
        }
        return patch;
    }

    auto fill(basic_sudoku_tile_patch<S>& patch)
      -> basic_sudoku_tile_patch<S>& {
        return fill(0, 0, patch);
    }

    auto print(std::ostream& out, const int width, const int height)
      -> std::ostream& {
        basic_sudoku_tile_patch<S> patch{width, height};
        return fill(patch).print(out, _traits);
    }

private:
    friend class patch_fill_state;

    auto _get(const int x, const int y)
      -> optional_reference<const board_type> {
        auto pos{_boards.find(std::make_tuple(x, y))};
        if(pos != _boards.end()) {
            if(auto board{get_if<board_type>(pos->second)}) {
                return board.ref();
            }
            if(auto solution{get_if<solution_state>(pos->second)}) {
                if(solution->is_done()) {
                    if(solution->is_solved()) {
                        // we need a copy before assigning to the variant
                        auto board{solution->board()};
                        pos->second = std::move(board);
                    } else {
                        _boards.erase(pos);
                    }
                } else {
                    solution->next();
                }
            }
        } else {
            _enqueue(x, y);
        }
        return {};
    }

    void _enqueue(const int x, const int y) {
        board_type added{};
        if(y > 0) {
            if(x > 0) {
                auto l{_get(x - 1, y)};
                auto d{_get(x, y - 1)};
                if(l and d) {
                    for(const auto by : integer_range(S - 1U)) {
                        added.set_block(0U, by, l->get_block(S - 1U, by));
                    }
                    for(const auto bx : integer_range(1U, S)) {
                        added.set_block(bx, S - 1U, d->get_block(bx, 0U));
                    }
                    _do_enqueue(x, y, added);
                }
            } else if(x < 0) {
                auto r{_get(x + 1, y)};
                auto d{_get(x, y - 1)};
                if(r and d) {
                    for(const auto by : integer_range(S - 1U)) {
                        added.set_block(S - 1U, by, r->get_block(0U, by));
                    }
                    for(const auto bx : integer_range(S - 1U)) {
                        added.set_block(bx, S - 1U, d->get_block(bx, 0U));
                    }
                    _do_enqueue(x, y, added);
                }
            } else {
                auto d{_get(x, y - 1)};
                if(d) {
                    for(const auto bx : integer_range(S)) {
                        added.set_block(bx, S - 1U, d->get_block(bx, 0U));
                    }
                    _do_enqueue(x, y, added);
                }
            }
        } else if(y < 0) {
            if(x > 0) {
                auto l{_get(x - 1, y)};
                auto u{_get(x, y + 1)};
                if(l and u) {
                    for(const auto by : integer_range(1U, S)) {
                        added.set_block(0U, by, l->get_block(S - 1U, by));
                    }
                    for(const auto bx : integer_range(1U, S)) {
                        added.set_block(bx, 0U, u->get_block(bx, S - 1U));
                    }
                    _do_enqueue(x, y, added);
                }
            } else if(x < 0) {
                auto r{_get(x + 1, y)};
                auto u{_get(x, y + 1)};
                if(r and u) {
                    for(const auto by : integer_range(1U, S)) {
                        added.set_block(S - 1U, by, r->get_block(0U, by));
                    }
                    for(const auto bx : integer_range(S - 1U)) {
                        added.set_block(bx, 0U, u->get_block(bx, S - 1U));
                    }
                    _do_enqueue(x, y, added);
                }
            } else {
                auto u{_get(x, y + 1)};
                if(u) {
                    for(const auto bx : integer_range(S)) {
                        added.set_block(bx, 0U, u->get_block(bx, S - 1U));
                    }
                    _do_enqueue(x, y, added);
                }
            }
        } else {
            if(x > 0) {
                auto l{_get(x - 1, y)};
                if(l) {
                    for(const auto by : integer_range(S)) {
                        added.set_block(0U, by, l->get_block(S - 1U, by));
                    }
                    _do_enqueue(x, y, added);
                }
            } else if(x < 0) {
                auto r{_get(x + 1, y)};
                if(r) {
                    for(const auto by : integer_range(S)) {
                        added.set_block(S - 1U, by, r->get_block(0U, by));
                    }
                    _do_enqueue(x, y, added);
                }
            }
        }
    }

    void _do_enqueue(const int x, const int y, board_type board) {
        _boards.emplace(
          std::make_tuple(x, y),
          solution_state{board.calculate_alternatives()});
    }

    std::reference_wrapper<const board_traits> _traits;
    flat_map<std::tuple<int, int>, std::variant<solution_state, board_type>>
      _boards;
};
//------------------------------------------------------------------------------
} // namespace eagine

