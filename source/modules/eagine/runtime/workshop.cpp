/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:workshop;

import eagine.core.types;
import <algorithm>;
import <condition_variable>;
import <memory>;
import <mutex>;
import <latch>;
import <queue>;
import <thread>;
import <tuple>;
import <vector>;

namespace eagine {
//------------------------------------------------------------------------------
export struct work_unit {
    work_unit() noexcept = default;
    work_unit(work_unit&&) noexcept = default;
    work_unit(const work_unit&) = delete;
    auto operator=(work_unit&&) noexcept -> work_unit& = default;
    auto operator=(const work_unit&) -> work_unit& = delete;
    virtual ~work_unit() noexcept = default;
    virtual auto do_it() noexcept -> bool = 0;
    virtual void deliver() noexcept = 0;
};
//------------------------------------------------------------------------------
export struct latched_work_unit : work_unit {

    latched_work_unit() noexcept = default;
    latched_work_unit(std::latch& l) noexcept
      : _completed{&l} {}

    void deliver() noexcept final {
        _completed->count_down();
    }

private:
    std::latch* _completed{nullptr};
};
//------------------------------------------------------------------------------
export class workshop {
private:
    std::vector<std::thread> _workers{};
    std::queue<work_unit*> _work_queue{};
    std::mutex _mutex{};
    std::condition_variable _cond{};
    bool _shutdown{false};

    auto _fetch() noexcept -> std::tuple<work_unit*, bool> {
        work_unit* work = nullptr;
        std::unique_lock lock{_mutex};
        if(!_shutdown) [[likely]] {
            _cond.wait(
              lock, [this] { return !_work_queue.empty() || _shutdown; });
            if(!_work_queue.empty()) {
                work = _work_queue.front();
                _work_queue.pop();
            }
        }
        return {work, _shutdown};
    }

    void _employ() noexcept {
        while(true) {
            auto [opt_work, shutdown] = _fetch();
            if(opt_work) {
                auto& work = extract(opt_work);
                if(work.do_it()) {
                    const std::unique_lock lock{_mutex};
                    work.deliver();
                    _cond.notify_all();
                } else {
                    enqueue(work);
                }
            }
            if(shutdown) {
                break;
            }
        }
    }

public:
    workshop() noexcept = default;
    workshop(workshop&&) = delete;
    workshop(const workshop&) = delete;
    auto operator=(workshop&&) = delete;
    auto operator=(const workshop&) = delete;
    ~workshop() noexcept {
        try {
            shutdown();
            wait_until_closed();
        } catch(...) {
        }
    }

    auto shutdown() noexcept -> workshop& {
        const std::unique_lock lock{_mutex};
        _shutdown = true;
        _cond.notify_all();
        return *this;
    }

    auto wait_until_closed() noexcept -> workshop& {
        for(auto& worker : _workers) {
            worker.join();
        }
        return *this;
    }

    auto wait_until_idle() noexcept -> workshop& {
        std::unique_lock lock{_mutex};
        _cond.wait(lock, [this]() { return _work_queue.empty(); });
        return *this;
    }

    auto add_worker() -> workshop& {
        _workers.emplace_back([this]() { this->_employ(); });
        return *this;
    }

    auto add_workers(span_size_t n) -> workshop& {
        _workers.reserve(safe_add(_workers.size(), std_size(n)));
        for([[maybe_unused]] const auto i : integer_range(n)) {
            add_worker();
        }
        return *this;
    }

    auto ensure_workers(span_size_t n) -> workshop& {
        const auto c = span_size(_workers.size());
        if(n > c) {
            add_workers(n - c);
        }
        return *this;
    }

    auto populate() -> workshop& {
        return ensure_workers(span_size(std::thread::hardware_concurrency()));
    }

    auto release_worker() noexcept -> workshop& {
        _workers.pop_back();
        return *this;
    }

    auto enqueue(work_unit& work) -> workshop& {
        const std::unique_lock lock{_mutex};
        if(_workers.empty()) [[unlikely]] {
            ensure_workers(std::max(
              span_size(std::thread::hardware_concurrency() / 2),
              span_size(2)));
        }
        _work_queue.push(&work);
        _cond.notify_one();
        return *this;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

