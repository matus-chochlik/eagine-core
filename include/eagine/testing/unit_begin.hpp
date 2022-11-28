/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
import <cstdint>;
import <string_view>;
import <vector>;
import <random>;
import <type_traits>;
#if __cpp_lib_source_location >= 201907L
import <source_location>;
#endif

namespace eagitest {
//------------------------------------------------------------------------------
class random_generator {
public:
    random_generator(int argc, const char** argv) noexcept;

    template <typename T>
    auto get_between(T min, T max, std::type_identity<T> = {}) noexcept -> T;

    auto get_byte(unsigned char min, unsigned char max) noexcept {
        return get_between(min, max);
    }

    template <typename T>
    auto get_any(std::type_identity<T> = {}) noexcept -> T;

    template <typename T>
    auto fill(std::vector<T>& dest) noexcept {
        for(auto& e : dest) {
            e = get_any<T>();
        }
    }

private:
    std::default_random_engine _re{};
};
//------------------------------------------------------------------------------
struct abort_test_case {};
//------------------------------------------------------------------------------
class track;
class case_;
class suite {
public:
    suite(int argc, const char** argv, std::string_view name) noexcept;
    suite(suite&&) = delete;
    suite(const suite&) = delete;
    auto operator=(suite&&) = delete;
    auto operator=(const suite&) = delete;
    ~suite() noexcept;

    auto random() noexcept -> random_generator&;

    auto once(void (*func)(eagitest::suite&)) -> suite&;
    auto repeat(unsigned, void (*func)(unsigned, eagitest::suite&)) -> suite&;

    auto exit_code() const noexcept -> int;

private:
    friend class case_;
    friend class track;

    random_generator _rand_gen;
    const std::string_view _name;
    bool _is_verbose{false};
    bool _checks_failed{false};
};
//------------------------------------------------------------------------------
class case_ {
public:
    case_(suite&, std::string_view name) noexcept;
    case_(case_&&) = delete;
    case_(const case_&) = delete;
    auto operator=(case_&&) = delete;
    auto operator=(const case_&) = delete;
    ~case_() noexcept;

    auto random() noexcept -> random_generator& {
        return _parent.random();
    }

    auto parameter(const auto&, std::string_view name) noexcept -> case_&;
    auto constructed(const auto&, std::string_view name) noexcept -> case_&;
    auto checkpoint(
      std::string_view label
#if __cpp_lib_source_location >= 201907L
      ,
      const std::source_location loc = std::source_location::current()
#endif
        ) noexcept -> case_&;

    auto ensure(bool condition, std::string_view label, const auto&...)
      -> case_&;

    auto check(bool condition, std::string_view label, const auto&...) noexcept
      -> case_&;

    template <typename T>
    auto check_equal(T l, T r, std::string_view label) noexcept -> case_&;

private:
    friend class track;

    suite& _parent;
    const std::string_view _name;
};
//------------------------------------------------------------------------------
class track {
public:
    track(
      case_&,
      std::string_view name,
      std::uint64_t expected_points,
      std::uint64_t expected_parts = 0U) noexcept;
    track(
      case_& parent,
      std::uint64_t expected_points,
      std::uint64_t expected_parts = 0U) noexcept
      : track{parent, "default", expected_points, expected_parts} {}

    track(track&&) = delete;
    track(const track&) = delete;
    auto operator=(track&&) = delete;
    auto operator=(const track&) = delete;
    ~track() noexcept;

    auto passed_part(std::uint64_t) noexcept -> track&;
    auto passed() noexcept -> track& {
        return passed_part(0U);
    }

private:
    case_& _parent;
    const std::string_view _name;
    const std::uint64_t _expected_points;
    const std::uint64_t _expected_parts;
    std::uint64_t _passed_points{0U};
    std::uint64_t _passed_parts{0U};
};
//------------------------------------------------------------------------------
} // namespace eagitest

