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
import eagine.core.types;
import :small_vector;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Object, std::size_t N>
class object_pool;
export template <typename Object, std::size_t N>
class pool_object : public optional_like_crtp<pool_object<Object, N>, Object> {
public:
    pool_object() noexcept = default;

    pool_object(object_pool<Object, N>& pool);

    pool_object(pool_object&& temp) noexcept
      : _pool{std::exchange(temp._pool, nullptr)}
      , _object{std::exchange(temp._object, nullptr)} {}

    pool_object(const pool_object&) = delete;

    auto operator=(pool_object&& temp) noexcept -> pool_object& {
        if(this != &temp) {
            std::swap(_pool, temp._pool);
            std::swap(_object, temp._object);
        }
        return *this;
    }

    auto operator=(const pool_object&) noexcept = delete;

    ~pool_object() noexcept;

    auto has_value() const noexcept -> bool {
        return bool(_object);
    }

    auto get() const noexcept -> Object* {
        return _object;
    }

    auto operator*() const noexcept -> Object& {
        assert(has_value());
        return *_object;
    }

private:
    object_pool<Object, N>* _pool{nullptr};
    Object* _object{nullptr};
};
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

    [[nodiscard]] auto get_object() -> pool_object<Object, N> {
        return {*this};
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

    auto eat(const Object* obj) noexcept -> bool {
        if(obj) [[likely]] {
            return eat(*obj);
        }
        return false;
    }

    template <typename Base>
        requires(std::derived_from<Object, Base>)
    auto eat(const Base* base) noexcept -> bool {
        if(const auto obj{dynamic_cast<const Object*>(base)}) [[likely]] {
            return eat(*obj);
        }
        return false;
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
template <typename Object, std::size_t N>
pool_object<Object, N>::pool_object(object_pool<Object, N>& pool)
  : _pool{&pool}
  , _object{&_pool->get()} {}
//------------------------------------------------------------------------------
template <typename Object, std::size_t N>
pool_object<Object, N>::~pool_object() noexcept {
    if(_pool) {
        _pool->eat(_object);
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
