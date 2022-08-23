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

    auto _fetch() noexcept -> std::tuple<work_unit*, bool>;
    void _employ() noexcept;

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

    auto shutdown() noexcept -> workshop&;

    auto wait_until_closed() noexcept -> workshop&;
    auto wait_until_idle() noexcept -> workshop&;

    auto add_worker() -> workshop&;
    auto add_workers(span_size_t n) -> workshop&;

    auto ensure_workers(span_size_t n) -> workshop&;

    auto populate() -> workshop&;

    auto release_worker() noexcept -> workshop&;

    auto enqueue(work_unit& work) -> workshop&;
};
//------------------------------------------------------------------------------
} // namespace eagine

