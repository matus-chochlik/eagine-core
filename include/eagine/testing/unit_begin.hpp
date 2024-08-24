/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
///
import std;

namespace eagitest {
//------------------------------------------------------------------------------
class random_generator {
public:
    random_generator(int argc, const char** argv) noexcept;

    template <typename T>
    auto get_between(T min, T max, std::type_identity<T> = {}) noexcept -> T;

    auto get_span_size(std::ptrdiff_t min, std::ptrdiff_t max) noexcept {
        return get_between(min, max);
    }

    auto get_std_size(std::size_t min, std::size_t max) noexcept {
        return get_between(min, max);
    }

    auto get_int(int min, int max) noexcept {
        return get_between(min, max);
    }

    auto get_char(char min, char max) noexcept {
        return get_between(min, max);
    }

    auto get_char_from(std::string_view chars) noexcept {
        return chars[get_std_size(0U, chars.size() - 1U)];
    }

    auto get_byte(unsigned char min, unsigned char max) noexcept {
        return get_between(min, max);
    }

    auto get_string(std::size_t min, std::size_t max) noexcept -> std::string;

    auto get_string_made_of(
      std::size_t min,
      std::size_t max,
      std::string_view chars) noexcept -> std::string;

    template <typename T>
    auto get_any(std::type_identity<T> = {}) noexcept -> T;

    auto get_bool() noexcept {
        return get_any(std::type_identity<bool>{});
    }

    auto one_of(int count) noexcept -> bool {
        return get_int(0, count - 1) == 0;
    }

    template <typename T>
    auto fill(std::vector<T>& dest) noexcept {
        for(auto& e : dest) {
            e = get_any<T>();
        }
    }

    auto fill_with_bytes(auto& dest) noexcept {
        for(auto& e : dest) {
            e = get_any<unsigned char>();
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
using suite_case_t = std::uint64_t;
class suite {
public:
    suite(
      int argc,
      const char** argv,
      std::string_view name,
      suite_case_t cases) noexcept;
    suite(suite&&) = delete;
    suite(const suite&) = delete;
    auto operator=(suite&&) = delete;
    auto operator=(const suite&) = delete;
    ~suite() noexcept;

    auto executable_path() const noexcept -> std::string_view;

    auto random() noexcept -> random_generator&;

    auto once(void (*func)(eagitest::suite&)) noexcept -> suite&;
    auto repeat(unsigned, void (*func)(unsigned, eagitest::suite&)) noexcept
      -> suite&;
    auto tested_case(suite_case_t) noexcept -> suite&;

    auto exit_code() const noexcept -> int;

protected:
    void checks_failed() noexcept {
        _checks_failed = true;
    }

private:
    friend class case_;
    friend class track;

    const int _argc;
    const char** _argv;
    random_generator _rand_gen;
    const std::string_view _name;
    const suite_case_t _expected_cases;
    suite_case_t _tested_cases;
    bool _is_verbose{false};
    bool _checks_failed{false};
};
//------------------------------------------------------------------------------
class case_ {
public:
    case_(suite&, suite_case_t case_idx, std::string_view name) noexcept;
    case_(case_&&) = delete;
    case_(const case_&) = delete;
    auto operator=(case_&&) = delete;
    auto operator=(const case_&) = delete;
    ~case_() noexcept;

    auto executable_path() const noexcept -> std::string_view {
        return _parent.executable_path();
    }

    auto random() const noexcept -> random_generator& {
        return _parent.random();
    }

    auto repeats(unsigned count) noexcept -> unsigned;

    auto parameter(const auto&, std::string_view name) noexcept -> case_&;
    auto constructed(const auto&, std::string_view name) noexcept -> case_&;
    auto checkpoint(std::string_view label) noexcept -> case_&;

    auto ensure(bool condition, std::string_view label, const auto&...)
      -> case_&;

    auto fail(std::string_view label, const auto&...) -> case_&;

    auto check(bool condition, std::string_view label, const auto&...) noexcept
      -> case_&;

    template <typename L, typename R>
    auto check_equal(const L& l, const R& r, std::string_view label) noexcept
      -> case_&;

    template <typename L, typename R>
    auto check_close(
      const L& l,
      const R& r,
      const std::common_type_t<L, R>& eps,
      std::string_view label) noexcept -> case_&;

    template <typename L, typename R>
    auto check_close(const L& l, const R& r, std::string_view label) noexcept
      -> case_&;

private:
    friend class track;

    suite& _parent;
    const std::string_view _name;
};
//------------------------------------------------------------------------------
using track_part_t = std::uint64_t;
class track {
public:
    track(
      case_&,
      std::string_view name,
      std::size_t expected_points,
      track_part_t expected_parts = 0U) noexcept;
    track(
      case_& parent,
      std::size_t expected_points,
      track_part_t expected_parts = 0U) noexcept
      : track{parent, "default", expected_points, expected_parts} {}

    track(track&&) = delete;
    track(const track&) = delete;
    auto operator=(track&&) = delete;
    auto operator=(const track&) = delete;
    ~track() noexcept;

    auto checkpoint(track_part_t) noexcept -> track&;

private:
    case_& _parent;
    const std::string_view _name;
    const std::size_t _expected_points;
    const track_part_t _expected_parts;
    std::size_t _passed_points{0U};
    track_part_t _checkpoints{0U};
};
//------------------------------------------------------------------------------
} // namespace eagitest

