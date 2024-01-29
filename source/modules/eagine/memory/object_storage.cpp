/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:object_storage;

import std;
import eagine.core.types;
import :align;
import :address;
import :span;
import :byte_allocator;
import :std_allocator;

namespace eagine::memory {
//------------------------------------------------------------------------------
export class object_storage {
public:
    template <typename X>
        requires(std::is_convertible_v<std::decay_t<X>*, byte_allocator*>)
    object_storage(X&& x) noexcept
      : _alloc(std::forward<X>(x))
      , _blks{std_allocator<owned_block>{_alloc}}
      , _alns{std_allocator<span_size_t>{_alloc}}
      , _dtrs{std_allocator<void (*)(block)>{_alloc}} {}

    object_storage(shared_byte_allocator a) noexcept
      : _alloc(std::move(a))
      , _blks{std_allocator<owned_block>{_alloc}}
      , _alns{std_allocator<span_size_t>{_alloc}}
      , _dtrs{std_allocator<void (*)(block)>{_alloc}} {}

    object_storage() noexcept
      : object_storage{default_byte_allocator()} {}

    object_storage(object_storage&&) = delete;
    object_storage(const object_storage&) = delete;
    auto operator=(object_storage&&) = delete;
    auto operator=(const object_storage&) = delete;
    ~object_storage() noexcept {
        clear();
    }

    void reserve(span_size_t n) {
        const integer sz{n};
        _blks.reserve(sz);
        _alns.reserve(sz);
        _dtrs.reserve(sz);
    }

    auto is_empty() const noexcept {
        assert(_blks.size() == _alns.size());
        assert(_blks.size() == _dtrs.size());
        return _blks.empty();
    }

    template <typename T, typename... Args>
    auto emplace(Args&&... args) -> std::remove_const_t<T>& {
        using A = std::remove_const_t<T>;
        const auto size = span_size_of<A>();
        const auto align = span_align_of<A>();
        owned_block b = _alloc.allocate(size, align);

        if(b.empty()) {
            throw std::bad_alloc();
        }

        assert(is_aligned_to(b.addr(), align));
        assert(b.size() >= size);

        _blks.push_back(std::move(b));
        _alns.push_back(align);
        _dtrs.push_back(&_destroy<A>);

        A* result = new(_blks.back().data()) A(std::forward<Args>(args)...);
        assert(result != nullptr);
        return *result;
    }

    void clear() noexcept {
        assert(_blks.size() == _alns.size());
        assert(_blks.size() == _dtrs.size());

        const auto n{_blks.size()};

        for(const auto i : index_range(_blks)) {
            _dtrs[n - i - 1](_blks[n - i - 1]);
        }

        for(const auto i : index_range(_blks)) {
            _alloc.deallocate(std::move(_blks[i]), _alns[i]);
        }

        _dtrs.clear();
        _alns.clear();
        _blks.clear();
    }

protected:
    shared_byte_allocator _alloc{};

    template <typename T>
    static void _destroy(block blk) noexcept {
        auto spn{accommodate<T>(blk)};
        assert(spn);
        auto& x{*spn};
        x.~T();
    }

    std::vector<owned_block, std_allocator<owned_block>> _blks;
    std::vector<span_size_t, std_allocator<span_size_t>> _alns;
    std::vector<void (*)(block), std_allocator<void (*)(block)>> _dtrs;

    template <typename Func>
    void for_each_block(Func& func) noexcept {
        for(const auto i : index_range(_blks)) {
            func(i, block(_blks[i]));
        }
    }
};
//------------------------------------------------------------------------------
export template <typename Signature>
class callable_storage;

export template <typename... Params>
class callable_storage<void(Params...)> : private memory::object_storage {
public:
    using base = memory::object_storage;

    template <typename X>
        requires(std::is_convertible_v<std::decay_t<X>*, byte_allocator*>)
    callable_storage(X&& x) noexcept
      : base(std::forward<X>(x))
      , _clrs{std_allocator<void (*)(block, Params...)>{base::_alloc}} {}

    callable_storage(const shared_byte_allocator& a) noexcept
      : base(a)
      , _clrs{std_allocator<void (*)(block, Params...)>{base::_alloc}} {}

    callable_storage() noexcept
      : callable_storage(default_byte_allocator()) {}

    void reserve(span_size_t n) {
        base::reserve(n);
        _clrs.reserve(integer(n));
    }

    template <typename T>
    auto add(T x) -> auto& requires(std::is_invocable_v<T, Params...>) {
        using A = std::remove_const_t<T>;
        auto& result = base::template emplace<A>(std::move(x));
        _clrs.push_back(&_call<A>);
        return result;
    }

    auto is_empty() const noexcept {
        assert(_blks.size() == _clrs.size());
        return base::is_empty();
    }

    void clear() noexcept {
        base::clear();
        _clrs.clear();
    }

    void operator()(Params... params) noexcept {
        auto fn = [&](auto i, block blk) {
            this->_clrs[i](blk, params...);
        };
        base::for_each_block(fn);
    }

private:
    std::vector<
      void (*)(block, Params...),
      std_allocator<void (*)(block, Params...)>>
      _clrs{};

    template <typename T>
    static void _call(block blk, Params... params) noexcept {
        auto spn{accommodate<T>(blk)};
        assert(spn);
        auto& x{*spn};
        x(params...);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::memory
