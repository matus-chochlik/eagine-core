/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:chunk_list;

import std;
import eagine.core.types;
import :static_vector;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T, std::size_t N>
class chunk_list;

export template <typename T, std::size_t N, bool isConst>
class chunk_list_iterator {
    friend class chunk_list<T, N>;
    friend class chunk_list_iterator<T, N, not isConst>;

    using iter_c_t = std::conditional_t<
      isConst,
      typename std::vector<unique_keeper<static_vector<T, N>>>::const_iterator,
      typename std::vector<unique_keeper<static_vector<T, N>>>::iterator>;

    using iter_e_t = std::conditional_t<
      isConst,
      typename static_vector<T, N>::const_iterator,
      typename static_vector<T, N>::iterator>;

    auto _as_tup() const noexcept {
        return std::make_tuple(_iter_c_p, _iter_e);
    }

public:
    using difference_type = std::ptrdiff_t;

    using iterator_category = std::forward_iterator_tag;

    constexpr chunk_list_iterator() noexcept = default;

    constexpr chunk_list_iterator(iter_c_t iter_c_p, iter_c_t iter_c_e) noexcept
      : _iter_c_p{iter_c_p}
      , _iter_c_e{iter_c_e} {}

    constexpr chunk_list_iterator(
      iter_c_t iter_c_p,
      iter_c_t iter_c_e,
      iter_e_t iter_e) noexcept
      : _iter_c_p{iter_c_p}
      , _iter_c_e{iter_c_e}
      , _iter_e{iter_e} {}

    template <bool thatConst>
        requires(not thatConst or isConst)
    constexpr chunk_list_iterator(
      const chunk_list_iterator<T, N, thatConst>& that) noexcept
      : _iter_c_p{that._iter_c_p}
      , _iter_c_e{that._iter_c_e}
      , _iter_e{that._iter_e} {}

    [[nodiscard]] constexpr auto operator==(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() == that._as_tup();
    }

    [[nodiscard]] constexpr auto operator!=(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() != that._as_tup();
    }

    [[nodiscard]] constexpr auto operator<(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() < that._as_tup();
    }

    [[nodiscard]] constexpr auto operator<=(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() <= that._as_tup();
    }

    [[nodiscard]] constexpr auto operator>(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() > that._as_tup();
    }

    [[nodiscard]] constexpr auto operator>=(
      const chunk_list_iterator& that) const noexcept {
        return _as_tup() >= that._as_tup();
    }

    constexpr auto operator*() const noexcept -> auto& {
        assert(_iter_c_p != _iter_c_e);
        return *_iter_e;
    }

    constexpr auto operator->() const noexcept -> auto* {
        assert(_iter_c_p != _iter_c_e);
        return &(*_iter_e);
    }

    constexpr auto adjust() noexcept -> auto& {
        while(_iter_c_p != _iter_c_e) {
            if(_iter_e != (*_iter_c_p)->end()) {
                break;
            }
            if(++_iter_c_p != _iter_c_e) {
                _iter_e = (*_iter_c_p)->begin();
            } else {
                _iter_e = {};
                break;
            }
        }
        return *this;
    }

    constexpr auto operator++() noexcept -> auto& {
        assert(_iter_c_p != _iter_c_e);
        ++_iter_e;
        return adjust();
    }

    constexpr auto operator++(int) noexcept -> auto {
        auto that{*this};
        ++(*this);
        return that;
    }

    constexpr auto operator+=(difference_type d) noexcept -> auto& {
        assert(_iter_c_p != _iter_c_e);
        const auto dist{[](auto l, auto r) {
            using I = std::common_type_t<decltype(l), decltype(r)>;
            using std::distance;
            return limit_cast<difference_type>(distance(I(l), I(r)));
        }};

        auto curr_d{dist(_iter_e, (*_iter_c_p)->end())};
        if(d <= curr_d) {
            std::advance(_iter_e, d);
        } else {
            while(true) {
                assert(_iter_c_p != _iter_c_e);
                d -= curr_d;
                ++_iter_c_p;

                if(_iter_c_p != _iter_c_e) [[likely]] {
                    curr_d = dist((*_iter_c_p)->begin(), (*_iter_c_p)->end());
                } else {
                    break;
                }
                if(d <= curr_d) {
                    break;
                }
            }

            if(_iter_c_p != _iter_c_e) {
                _iter_e = (*_iter_c_p)->begin();
                std::advance(_iter_e, d);
            } else {
                assert(d == 0);
                _iter_e = {};
            }
        }
        return adjust();
    }

    friend constexpr auto operator+(
      chunk_list_iterator pos,
      difference_type ofs) noexcept -> auto {
        pos += ofs;
        return pos;
    }

    friend constexpr auto forward_distance(
      const chunk_list_iterator& first,
      const chunk_list_iterator& second) noexcept -> difference_type {
        assert(first._iter_c_e == second._iter_c_e);
        assert(first <= second);
        const auto dist{[](auto l, auto r) {
            using I = std::common_type_t<decltype(l), decltype(r)>;
            using std::distance;
            return limit_cast<difference_type>(distance(I(l), I(r)));
        }};
        auto c_p{first._iter_c_p};

        if(c_p == first._iter_c_e) {
            return 0;
        } else if(c_p == second._iter_c_p) {
            return dist(first._iter_e, second._iter_e);
        } else {
            auto r{dist(first._iter_e, (*c_p)->cend())};
            while(++c_p != second._iter_c_p) {
                if(*c_p) {
                    r += dist((*c_p)->cbegin(), (*c_p)->cend());
                }
            }
            if(c_p != first._iter_c_e) {
                r += dist((*c_p)->cbegin(), second._iter_e);
            }
            return r;
        }
    }

    friend constexpr auto distance(
      const chunk_list_iterator& first,
      const chunk_list_iterator& second) noexcept -> difference_type {
        assert(first._iter_c_e == second._iter_c_e);
        if(first <= second) {
            return forward_distance(first, second);
        } else {
            return -forward_distance(second, first);
        }
    }

    friend constexpr auto next(
      chunk_list_iterator pos,
      difference_type ofs = 1) noexcept -> chunk_list_iterator {
        pos += ofs;
        return pos;
    }

    friend constexpr auto lower_bound(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const auto& value,
      auto comp) noexcept -> chunk_list_iterator {
        auto count{distance(first, last)};

        while(count > 0) {
            const auto step{count / 2};
            auto it{first + step};

            if(comp(*it, value)) {
                first = ++it;
                count -= step + 1;
            } else {
                count = step;
            }
        }

        return first;
    }

    friend constexpr auto lower_bound(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const T& value) noexcept -> chunk_list_iterator {
        return lower_bound(first, last, value, std::less<T>{});
    }

    friend constexpr auto upper_bound(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const auto& value,
      auto comp) noexcept -> chunk_list_iterator {
        auto count{distance(first, last)};

        while(count > 0) {
            const auto step{count / 2};
            auto it{first + step};

            if(not comp(value, *it)) {
                first = ++it;
                count -= step + 1;
            } else {
                count = step;
            }
        }

        return first;
    }

    friend constexpr auto upper_bound(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const T& value) noexcept -> chunk_list_iterator {
        return upper_bound(first, last, value, std::less<T>{});
    }

    friend constexpr auto equal_range(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const auto& value,
      auto comp) noexcept
      -> std::pair<chunk_list_iterator, chunk_list_iterator> {
        return std::make_pair(
          lower_bound(first, last, value, comp),
          upper_bound(first, last, value, comp));
    }

    friend constexpr auto equal_range(
      chunk_list_iterator first,
      chunk_list_iterator last,
      const auto& value) noexcept
      -> std::pair<chunk_list_iterator, chunk_list_iterator> {
        return equal_range(first, last, value, std::less<T>{});
    }

    constexpr auto operator-(const chunk_list_iterator& that) const noexcept
      -> difference_type {
        return distance(that, *this);
    }

private:
    iter_c_t _iter_c_p{};
    iter_c_t _iter_c_e{};
    iter_e_t _iter_e{};
};
//------------------------------------------------------------------------------
/// @brief Container made of pre-allocated chunks of specified size.
/// @ingroup container
/// @see small_vector
export template <typename T, std::size_t N>
class chunk_list {
    static_assert(N >= 4, "chunk cannot be empty");

public:
    /// @brief Stored value type.
    using value_type = std::remove_const_t<T>;

    /// @brief Size type.
    using size_type = std::size_t;

    /// @brief Difference type.
    using difference_type = std::ptrdiff_t;

    /// @brief Reference type
    using reference = T&;

    /// @brief Constant reference type
    using const_reference = const T&;

    /// @brief Iterator type.
    using iterator = chunk_list_iterator<T, N, false>;

    /// @brief Constant iterator type.
    using const_iterator = chunk_list_iterator<T, N, true>;

    using allocator_type = std::allocator<T>;

    /// @brief Default constructor.
    constexpr chunk_list() noexcept = default;

    /// @brief Construction with specified number of elements.
    constexpr chunk_list(size_type n) noexcept {
        resize(n);
    }

    /// @brief Returns the maximum number of elements that can be stored.
    /// @see size
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return std::numeric_limits<size_type>::max();
    }

    /// @brief Returns the current capacity of this chunk_list.
    /// @see size
    /// @see resize
    /// @see reserve
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return _chunks.capacity() * N;
    }

    /// @brief Reserves storage for the specified number of elements.
    /// @pre n <= max_size()
    /// @see size
    constexpr void reserve(const size_type n) noexcept {
        if(capacity() < n) {
            _chunks.reserve(n / N + (n % N != 0 ? 1 : 0));
        }
    }

    /// @brief Returns the current number of elements in this chunk_list.
    /// @see max_size
    /// @see resize
    /// @see empty
    /// @see full
    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return std::accumulate(
          _chunks.begin(),
          _chunks.end(),
          std_size(0),
          [](size_type total, const auto& chunk) {
              return total + chunk->size();
          });
    }

    /// @brief Changes the current size to the specified value.
    /// @pre n <= max_size()
    /// @see max_size
    /// @see size
    /// @see capacity
    constexpr void resize(size_type n) {
        const auto osz{size()};
        if(osz > n) {
            n = osz - n;
            while(n > 0) {
                assert(not empty());
                auto& cnk{*_chunks.back()};
                const auto csz{cnk.size()};
                const auto dec{std::min(n, csz)};
                cnk.resize(cnk.size() - dec);
                n -= dec;
                if(cnk.empty()) {
                    _chunks.pop_back();
                }
            }
        } else if(n > osz) {
            n = n - osz;
            while(n > 0) {
                if(_chunks.empty() or _chunks.back()->full()) {
                    _chunks.emplace_back();
                }
                auto& cnk{*_chunks.back()};
                const auto avl{cnk.available()};
                const auto inc{std::min(n, avl)};
                cnk.resize(cnk.size() + inc);
                n -= inc;
            }
        }
    }

    /// @brief Indicates if this chunk_list is empty.
    /// @see size
    /// @see full
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return size() == 0;
    }

    /// @brief Clears all elements.
    /// @post empty()
    /// @see empty
    constexpr void clear() noexcept {
        std::for_each(
          _chunks.begin(), _chunks.end(), [](auto& chunk) { chunk->clear(); });
    }

    /// @brief Pushes a new element to the back of this chunk_list.
    /// @pre not full()
    /// @see emplace_back
    /// @see pop_back
    /// @see insert
    constexpr void push_back(const value_type& value) {
        if(empty() or _chunks.back()->full()) {
            _chunks.emplace_back();
        }
        _chunks.back()->push_back(value);
    }

    /// @brief Constructs a new element in-place at the back of this static_vector.
    /// @pre not full()
    /// @see push_back
    /// @see pop_back
    /// @see emplace
    template <typename... Args>
    constexpr void emplace_back(Args&&... args) {
        if(empty() or _chunks.back()->full()) {
            _chunks.emplace_back();
        }
        _chunks.back()->emplace_back(std::forward<Args>(args)...);
    }

    /// @brief Removes a single element from the back of this chunk_list.
    /// @pre not empty()
    /// @see push_back
    /// @see emplace_back
    /// @see erase
    /// @see clear
    constexpr void pop_back() {
        assert(not empty());
        auto pos{_chunks.rbegin()};
        while((*pos)->empty()) {
            assert(pos != _chunks.rend());
            ++pos;
        }
        return (*pos)->pop_back();
    }

    /// @brief Copies the elements from the specified range into this chunk_list.
    /// @see clear
    /// @see push_back
    constexpr void assign(auto first, auto last) {
        using std::distance;
        clear();
        reserve(limit_cast<size_type>(distance(first, last)));
        while(first != last) {
            push_back(*first);
            ++first;
        }
    }

    /// @brief Returns an iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        if(_chunks.empty()) {
            return iterator{_chunks.begin(), _chunks.end()}.adjust();
        }
        return iterator{
          _chunks.begin(), _chunks.end(), _chunks.front()->begin()}
          .adjust();
    }

    /// @brief Returns a constant iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        if(_chunks.empty()) {
            return const_iterator{_chunks.begin(), _chunks.end()}.adjust();
        }
        return const_iterator{
          _chunks.begin(), _chunks.end(), _chunks.front()->begin()}
          .adjust();
    }

    /// @brief Returns an iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return {_chunks.end(), _chunks.end()};
    }

    /// @brief Returns a constant iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return {_chunks.end(), _chunks.end()};
    }

    constexpr auto get_insert_position(const_iterator pos) -> const_iterator {
        if(_chunks.empty()) {
            _chunks.emplace_back();
            return {_chunks.begin(), _chunks.end(), _chunks.back()->begin()};
        }

        assert(pos._iter_c_e == _chunks.end());

        if(pos._iter_c_p == _chunks.end()) {
            if(_chunks.back()->full()) {
                _chunks.emplace_back();
            }
            return {
              _chunks.begin() + _chunks.size() - 1,
              _chunks.end(),
              _chunks.back()->end()};
        }

        if((*pos._iter_c_p)->full()) {
            const auto e_p{pos._iter_e - (*pos._iter_c_p)->begin()};
            const auto i_p{pos._iter_c_p - _chunks.cbegin()};
            const auto c_n{_chunks.emplace(pos._iter_c_p + 1)};
            const auto c_p{_chunks.begin() + i_p};
            const auto split{limit_cast<difference_type>(N / 2)};
            (*c_n)->insert(
              (*c_n)->begin(), (*c_p)->cbegin() + split, (*c_p)->cend());
            (*c_p)->erase((*c_p)->cbegin() + split, (*c_p)->cend());

            if(e_p <= split) {
                return const_iterator{
                  c_p, _chunks.end(), (*c_p)->begin() + e_p};
            } else {
                return const_iterator{
                  c_n, _chunks.end(), (*c_n)->begin() + e_p - split};
            }
        }

        return pos;
    }

    /// @brief Stores elements from the specified range starting at given position.
    /// @see push_back
    /// @see emplace
    constexpr auto insert(const_iterator pos, const T& value) -> iterator {
        pos = get_insert_position(pos);
        const auto i_p{pos._iter_c_p - _chunks.cbegin()};
        const auto ins_p{(*pos._iter_c_p)->insert(pos._iter_e, value)};
        return iterator{_chunks.begin() + i_p, _chunks.end(), ins_p}.adjust();
    }

    /// @brief Stores elements from the specified range starting at given position.
    /// @see push_back
    /// @see insert
    template <typename... Args>
    constexpr auto emplace(const_iterator pos, Args&&... args) -> iterator {
        pos = get_insert_position(pos);
        const auto i_p{pos._iter_c_p - _chunks.cbegin()};
        const auto ins_p{
          (*pos._iter_c_p)->emplace(pos._iter_e, std::forward<Args>(args)...)};
        return iterator{_chunks.begin() + i_p, _chunks.end(), ins_p}.adjust();
    }

    /// @brief Erases the elements at the specified position.
    /// @see clear
    /// @see erase_if
    /// @see pop_back
    constexpr auto erase(const_iterator pos) noexcept -> iterator {
        if(pos._iter_c_p == pos._iter_c_e) {
            return end();
        }
        const auto i_p{pos._iter_c_p - _chunks.cbegin()};
        const auto del_p{(*pos._iter_c_p)->erase(pos._iter_e)};
        return iterator{_chunks.begin() + i_p, _chunks.end(), del_p}.adjust();
    }

    /// @brief Erases the elements satisfying the specified predicate.
    /// @see erase
    /// @see clear
    /// @see pop_back
    constexpr auto erase_if(const auto& predicate) -> size_type {
        size_type count{0};
        for(auto& chunk : _chunks) {
            using std::erase_if;
            count += limit_cast<size_type>(erase_if(*chunk, predicate));
        }
        return count;
    }

    friend constexpr auto erase_if(chunk_list& that, const auto& predicate)
      -> size_type {
        return that.erase_if(predicate);
    }

    /// @brief Returns reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() noexcept -> value_type& {
        assert(not empty());
        auto pos{_chunks.begin()};
        while((*pos)->empty()) {
            assert(pos != _chunks.end());
            ++pos;
        }
        return (*pos)->front();
    }

    /// @brief Returns const reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() const noexcept -> const value_type& {
        assert(not empty());
        auto pos{_chunks.begin()};
        while((*pos)->empty()) {
            assert(pos != _chunks.end());
            ++pos;
        }
        return (*pos)->front();
    }

    /// @brief Returns reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() noexcept -> value_type& {
        assert(not empty());
        auto pos{_chunks.rbegin()};
        while((*pos)->empty()) {
            assert(pos != _chunks.rend());
            ++pos;
        }
        return (*pos)->back();
    }

    /// @brief Returns const reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() const noexcept -> const value_type& {
        assert(not empty());
        auto pos{_chunks.rbegin()};
        while((*pos)->empty()) {
            assert(pos != _chunks.rend());
            ++pos;
        }
        return (*pos)->back();
    }

private:
    std::vector<unique_keeper<static_vector<T, N>>> _chunks;
};
//------------------------------------------------------------------------------
} // namespace eagine
