/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_ALIGN_ALLOC_HPP
#define EAGINE_MEMORY_ALIGN_ALLOC_HPP

#include "../assert.hpp"
#include "../instead_of.hpp"
#include "../int_constant.hpp"
#include "default_alloc.hpp"
#include "shared_alloc.hpp"
#include <array>
#include <utility>

namespace eagine::memory {

// multi_align_byte_allocator - fwd
template <typename AlignSeq, typename Policy = default_byte_allocator_policy>
class multi_align_byte_allocator;

// multi_align_byte_allocator
template <std::size_t... Align, typename Policy>
class multi_align_byte_allocator<std::index_sequence<Align...>, Policy>
  : public byte_allocator_impl<
      Policy,
      multi_align_byte_allocator,
      std::index_sequence<Align...>> {
public:
    multi_align_byte_allocator(
      instead_of_t<size_constant<Align>, shared_byte_allocator>... aligned_alloc,
      shared_byte_allocator fallback_alloc = default_byte_allocator())
      : _alignment{{Align...}}
      , _aligned_alloc{{std::move(aligned_alloc)...}}
      , _fallback_alloc(std::move(fallback_alloc)) {}

    using value_type = byte;
    using size_type = span_size_t;

    auto equal(byte_allocator* a) const noexcept -> bool override {
        if(auto* that = dynamic_cast<multi_align_byte_allocator*>(a)) {
            for(std::size_t i = 0; i < _aligned_alloc.size(); ++i) {
                if(_aligned_alloc[i] != that->_aligned_alloc[i]) {
                    return false;
                }
            }

            return _fallback_alloc == that->_fallback_alloc;
        }
        return false;
    }

    auto max_size(span_size_t a) noexcept -> size_type override {
        return _get_alloc(a).max_size(a);
    }

    auto has_allocated(const owned_block& b, span_size_t a) noexcept
      -> tribool override {
        for(std::size_t i = 0; i < _aligned_alloc.size(); ++i) {
            if(_aligned_alloc[i].has_allocated(b, a)) {
                return true;
            }
        }
        return _fallback_alloc.has_allocated(b, a);
    }

    auto allocate(size_type n, size_type a) noexcept -> owned_block override {
        return _get_alloc(a).allocate(n, a);
    }

    void deallocate(owned_block&& b, size_type a) noexcept override {
        EAGINE_ASSERT(!!_get_alloc(a).has_allocated(b, a));
        _get_alloc(a).deallocate(std::move(b), a);
    }

private:
    std::array<span_size_t, sizeof...(Align)> _alignment;

    std::array<shared_byte_allocator, sizeof...(Align)> _aligned_alloc;

    shared_byte_allocator _fallback_alloc;

    auto _get_alloc(span_size_t align) -> shared_byte_allocator& {
        EAGINE_ASSERT(_alignment.size() == _aligned_alloc.size());
        for(std::size_t i = 0; i < _alignment.size(); ++i) {
            if(_alignment[i] == align) {
                return _aligned_alloc[i];
            }
        }
        return _fallback_alloc;
    }
};

} // namespace eagine::memory

#endif // EAGINE_MEMORY_ALIGN_ALLOC_HPP
