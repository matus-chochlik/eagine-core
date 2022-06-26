/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:filesystem;

import eagine.core.concepts;
import eagine.core.memory;
import :decl;
import <string>;
import <filesystem>;

namespace eagine {

export template <typename Derived>
struct valid_if_filesystem_policy {
    auto self() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }

    auto operator()(const std::filesystem::path& value) const noexcept {
        return self().is_valid(value);
    }

    auto operator()(const std::string& value) const noexcept {
        return self().is_valid(value);
    }

    auto operator()(const std::string_view& value) const noexcept {
        return self().is_valid(value);
    }

    auto operator()(const string_view& value) const noexcept {
        return self().is_valid(value.std_view());
    }

    template <typename T>
    auto operator()(const T& value) const noexcept {
        return self().is_valid(std::filesystem::path{value});
    }
};

// existing file
export struct valid_if_existing_file_policy
  : valid_if_filesystem_policy<valid_if_existing_file_policy> {
    auto is_valid(const std::filesystem::path& path) const noexcept {
        return is_regular_file(path) ||
               (is_symlink(path) && is_regular_file(read_symlink(path)));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "is invalid because it is not an existing file path";
        }
    };
};

export template <typename T>
using valid_if_existing_file = valid_if<T, valid_if_existing_file_policy>;

// existing directory
export struct valid_if_existing_directory_policy
  : valid_if_filesystem_policy<valid_if_existing_directory_policy> {
    auto is_valid(const std::filesystem::path& path) const noexcept {
        return is_directory(path) ||
               (is_symlink(path) && is_directory(read_symlink(path)));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "is invalid because it is not an existing directory path";
        }
    };
};

export template <typename T>
using valid_if_existing_directory =
  valid_if<T, valid_if_existing_directory_policy>;

// in writable directory
export struct valid_if_in_writable_directory_policy
  : valid_if_filesystem_policy<valid_if_in_writable_directory_policy> {
    auto is_valid(std::filesystem::path path) const noexcept {
        path.remove_filename();
        const auto test = [](const auto& p) {
            return is_directory(p) && (status(p).permissions() &
                                       (std::filesystem::perms::owner_write |
                                        std::filesystem::perms::group_write)) !=
                                        std::filesystem::perms::none;
        };
        return test(path) || (is_symlink(path) && test(read_symlink(path)));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "is invalid because it is not a path in writable directory";
        }
    };
};

export template <typename T>
using valid_if_in_writable_directory =
  valid_if<T, valid_if_in_writable_directory_policy>;

} // namespace eagine

