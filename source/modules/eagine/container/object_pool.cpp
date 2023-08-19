/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:object_pool;

import std;
import :small_vector;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Object, std::size_t N>
class object_pool {
    static constexpr auto _get_empty() noexcept {
        return [](const auto& node) {
            return not std::get<1>(node);
        };
    }

public:
    template <typename... Args>
    [[nodiscard]] auto make(Args&&... args) -> Object& {
        auto pos{std::find_if(_storage.begin(), _storage.end(), _get_empty())};
        if(pos == _storage.end()) {
            pos = _storage.emplace(
              _storage.end(),
              std::make_unique<Object>(std::forward<Args>(args)...),
              true);
        } else {
            *(std::get<0>(*pos)) = Object(std::forward<Args>(args)...);
        }
        ++_count;
        return *(std::get<0>(*pos));
    }

    [[nodiscard]] auto get() -> Object& {
        auto pos{std::find_if(_storage.begin(), _storage.end(), _get_empty())};
        if(pos == _storage.end()) {
            pos = _storage.emplace(
              _storage.end(), std::make_unique<Object>(), true);
        }
        ++_count;
        return *(std::get<0>(*pos));
    }

    auto eat(const Object& obj) noexcept -> bool {
        auto pos{
          std::find_if(_storage.begin(), _storage.end(), [&](const auto& node) {
              return &obj == std::get<0>(node).get();
          })};
        if(pos == _storage.end()) {
            return false;
        }
        std::get<1>(*pos) = false;
        --_count;
        if(_storage.size() >= _count * 2U) {
            _storage.erase(
              std::remove_if(_storage.begin(), _storage.end(), _get_empty()),
              _storage.end());
        }
        return true;
    }

    auto empty() const noexcept -> bool {
        return _count == 0U;
    }

    auto size() const noexcept -> std::size_t {
        return _count;
    }

    auto capacity() const noexcept -> std::size_t {
        return _storage.size();
    }

private:
    small_vector<std::tuple<std::unique_ptr<Object>, bool>, N> _storage;
    std::size_t _count{0U};
};
//------------------------------------------------------------------------------
} // namespace eagine
