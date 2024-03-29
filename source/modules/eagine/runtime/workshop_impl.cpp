/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
auto workshop::_fetch() noexcept
  -> std::tuple<optional_reference<work_unit>, bool> {
    optional_reference<work_unit> work;
    std::unique_lock lock{_queue_lockable};
    if(not _shutdown) [[likely]] {
        _cond.wait(
          lock, [this] { return not _work_queue.empty() or _shutdown; });
        if(not _work_queue.empty()) {
            work = _work_queue.front();
            _work_queue.pop();
        }
    }
    return {work, _shutdown};
}
//------------------------------------------------------------------------------
void workshop::_employ() noexcept {
    while(true) {
        auto [opt_work, shutdown] = _fetch();
        opt_work.and_then([this](auto& work) {
            if(work.do_it()) {
                std::unique_lock lock{_queue_lockable};
                work.deliver();
                _cond.notify_all();
            } else {
                enqueue(work);
            }
            return noopt{};
        });
        if(shutdown) {
            break;
        }
    }
}
//------------------------------------------------------------------------------
auto workshop::shutdown() noexcept -> workshop& {
    if(const std::lock_guard lock{_queue_lockable}; true) {
        _shutdown = true;
        _cond.notify_all();
    }
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::wait_until_closed() noexcept -> workshop& {
    for(auto& worker : _workers) {
        worker.join();
    }
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::wait_until_idle() noexcept -> workshop& {
    std::unique_lock lock{_queue_lockable};
    _cond.wait(lock, [this]() { return _work_queue.empty(); });
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::add_worker() -> workshop& {
    _workers.emplace_back([this]() { this->_employ(); });
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::add_workers(span_size_t n) -> workshop& {
    _workers.reserve(safe_add(_workers.size(), std_size(n)));
    for([[maybe_unused]] const auto i : integer_range(n)) {
        add_worker();
    }
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::ensure_workers(span_size_t n) -> workshop& {
    const auto c = span_size(_workers.size());
    if(n > c) {
        add_workers(n - c);
    }
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::populate() -> workshop& {
    return ensure_workers(span_size(std::thread::hardware_concurrency()));
}
//------------------------------------------------------------------------------
auto workshop::release_worker() noexcept -> workshop& {
    _workers.pop_back();
    return *this;
}
//------------------------------------------------------------------------------
auto workshop::enqueue(work_unit& work) -> workshop& {
    const std::lock_guard lock{_queue_lockable};
    if(_workers.empty()) [[unlikely]] {
        ensure_workers(std::max(
          span_size(std::thread::hardware_concurrency() / 2), span_size(2)));
    }
    _work_queue.push(&work);
    _cond.notify_one();
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagine

