/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:workshop;

import std;
import eagine.core.types;

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
export class workshop;
export template <typename Function>
struct inplace_work_unit : latched_work_unit {
    inplace_work_unit(workshop& w, std::latch& l, Function function) noexcept;

    auto do_it() noexcept -> bool final {
        return _function();
    }

private:
    const Function _function;
};

export template <typename Function>
inplace_work_unit(workshop&, std::latch&, Function)
  -> inplace_work_unit<Function>;
//------------------------------------------------------------------------------
export template <typename Function>
class inplace_work_batch {
public:
    inplace_work_batch(
      workshop& ws,
      std::size_t size,
      const Function& function) noexcept
      : _completed{limit_cast<std::ptrdiff_t>(size)} {
        _units.reserve(size);
        for(const auto i : integer_range(size)) {
            _units.emplace_back(ws, _completed, function);
        }
    }

    inplace_work_batch(workshop& ws, const Function& function) noexcept
      : inplace_work_batch{ws, std::thread::hardware_concurrency(), function} {}

    ~inplace_work_batch() noexcept {
        _completed.wait();
    }

private:
    std::latch _completed;
    std::vector<inplace_work_unit<Function>> _units;
};

export template <typename Function>
inplace_work_batch(workshop&, const Function&) -> inplace_work_batch<Function>;
//------------------------------------------------------------------------------
export class workshop {
private:
    std::vector<std::thread> _workers{};
    std::queue<work_unit*> _work_queue{};
    std::mutex _queue_lockable{};
    std::condition_variable _cond{};
    bool _shutdown{false};

    auto _fetch() noexcept -> std::tuple<optional_reference<work_unit>, bool>;
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
template <typename Function>
inplace_work_unit<Function>::inplace_work_unit(
  workshop& w,
  std::latch& l,
  Function function) noexcept
  : latched_work_unit{l}
  , _function{std::move(function)} {
    w.enqueue(*this);
}
//------------------------------------------------------------------------------
} // namespace eagine

