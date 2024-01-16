/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<eagine/config/os_info.hpp>)
#include <eagine/config/os_info.hpp>
#endif

#if __has_include(<eagine/config/git_info.hpp>)
#include <eagine/config/git_info.hpp>
#endif

#if __has_include(<eagine/config/build_info.hpp>)
#include <eagine/config/build_info.hpp>
#endif

#ifndef EAGINE_INSTALL_PREFIX
#define EAGINE_INSTALL_PREFIX ""
#endif

module eagine.core.build_info;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
// git
//------------------------------------------------------------------------------
auto config_git_branch() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {string_view{EAGINE_GIT_BRANCH}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_git_hash_id() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {string_view{EAGINE_GIT_HASH_ID}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_git_date() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {string_view{EAGINE_GIT_DATE}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_git_describe() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {string_view{EAGINE_GIT_DESCRIBE}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_git_version() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {string_view{EAGINE_GIT_VERSION}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_git_version_major() noexcept -> valid_if_nonnegative<int> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {EAGINE_GIT_VERSION_MAJOR};
#else
    return -1;
#endif
}
//------------------------------------------------------------------------------
auto config_git_version_minor() noexcept -> valid_if_nonnegative<int> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {EAGINE_GIT_VERSION_MINOR};
#else
    return -1;
#endif
}
//------------------------------------------------------------------------------
auto config_git_version_patch() noexcept -> valid_if_nonnegative<int> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {EAGINE_GIT_VERSION_PATCH};
#else
    return -1;
#endif
}
//------------------------------------------------------------------------------
auto config_git_version_commit() noexcept -> valid_if_nonnegative<int> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {EAGINE_GIT_VERSION_COMMIT};
#else
    return -1;
#endif
}
//------------------------------------------------------------------------------
auto config_git_version_tuple() noexcept -> std::tuple<int, int, int, int> {
#if defined(EAGINE_CONFIG_GIT_INFO_HPP)
    return {
      EAGINE_GIT_VERSION_MAJOR,
      EAGINE_GIT_VERSION_MINOR,
      EAGINE_GIT_VERSION_PATCH,
      EAGINE_GIT_VERSION_COMMIT};
#else
    return {-1, -1, -1, -1};
#endif
}
//------------------------------------------------------------------------------
// compiler
//------------------------------------------------------------------------------
auto architecture_tag() noexcept -> valid_if_not_empty<string_view> {
#if defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64) || \
  defined(_M_X64)
    return {"amd64"};
#elif defined(__i386__) || defined(__i386) || defined(__i486__) || \
  defined(__i586__) || defined(__i686__) || defined(__X86__) || defined(_X86_)
    return {"i386"};
#elif defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || \
  defined(__itanium__)
    return {"ia64"};
#elif defined(__aarch64__)
    return {"arm64"};
#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || \
  defined(_M_ARMT)
    return {"arm"};
#elif defined(__powerpc64__) || defined(__powerpc__) || defined(__powerpc) || \
  defined(_M_PPC) || defined(_ARCH_PPC)
    return {"powerpc"};
#elif defined(__sparc__) || defined(__sparc)
    return {"sparc"};
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
    return {"mips"};
#elif defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
    return {"alpha"};
#endif
    return {};
}
//------------------------------------------------------------------------------
auto architecture_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64) || \
  defined(_M_X64)
    return {"AMD64/x86-64"};
#elif defined(__i386__) || defined(__i386) || defined(__i486__) || \
  defined(__i586__) || defined(__i686__) || defined(__X86__) || defined(_X86_)
    return {"Intel x86"};
#elif defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || \
  defined(__itanium__)
    return {"Intel IA-64"};
#elif defined(__aarch64__)
    return {"ARM64"};
#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || \
  defined(_M_ARMT)
    return {"ARM"};
#elif defined(__powerpc64__) || defined(__powerpc__) || defined(__powerpc) || \
  defined(_M_PPC) || defined(_ARCH_PPC)
    return {"PowerPC"};
#elif defined(__sparc__) || defined(__sparc)
    return {"SPARC"};
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
    return {"MIPS"};
#elif defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
    return {"Alpha"};
#endif
    return {};
}
//------------------------------------------------------------------------------
auto compiler_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(__clang__)
    return {{"Clang"}};
#elif defined(__INTEL_COMPILER)
    return {{"Intel C/C++"}};
#elif defined(__DMC__)
    return {{"Digital Mars"}};
#elif defined(__EDG__)
    return {{"EDG C++ frontend"}};
#elif defined(__CWCC__)
    return {{"Metrowerks CodeWarrior"}};
#elif defined(_MSC_VER)
    return {{"Microsoft Visual C++"}};
#elif defined(__COMO__)
    return {{"Comeau C++"}};
#elif defined(__FCC_VERSION)
    return {{"Fujitsu C++"}};
#elif defined(__MINGW64__)
    return {{"MinGW 64bit"}};
#elif defined(__MINGW32__)
    return {{"MinGW 32bit"}};
#elif defined(_ACC_)
    return {{"ACC"}};
#elif defined(__CC_ARM)
    return {{"ARM Compiler"}};
#elif defined(__BORLANDC__) || defined(__CODEGEARC__)
    return {{"Borland C++"}};
#elif defined(__DECCXX)
    return {{"Compaq C++"}};
#elif defined(__DJGPP__)
    return {{"DJGPP"}};
#elif defined(__IBMCPP__)
    return {{"IBM z/OS XL C++"}};
#elif defined(__IBMCPP__)
    return {{"IBM z/OS XL C++"}};
#elif defined(__WATCOMC__)
    return {{"Watcom C++"}};
#elif defined(__ZTC__)
    return {{"Zortech C++"}};
#elif defined(__GNUC__)
    return {{"GCC"}};
#endif
    return {};
}
//------------------------------------------------------------------------------
auto compiler_version_major() noexcept -> valid_if_nonnegative<int> {
#if defined(__clang_major__)
    return {__clang_major__};
#elif defined(__INTEL_COMPILER)
    return {(__INTEL_COMPILER % 1000) / 100};
#elif defined(__DMC__)
    return {(__DMC__ >> 8U) & 0xFU};
#elif defined(__EDG_VERSION__)
    return {(__EDG_VERSION__ % 1000) / 100};
#elif defined(__CWCC__)
    return {(__CWCC__ % 10000) / 1000};
#elif defined(_MSC_VER)
    return {(_MSC_VER % 10000) / 100};
#elif defined(__COMO_VERSION__)
    return {(__COMO_VERSION__ % 1000) / 100};
#elif defined(__MINGW64_VERSION_MAJOR)
    return {__MINGW64_VERSION_MAJOR};
#elif defined(__MINGW32_VERSION_MAJOR)
    return {__MINGW32_VERSION_MAJOR};
#elif defined(__GNUC__)
    return {__GNUC__};
#endif
    return {-1};
}
//------------------------------------------------------------------------------
auto compiler_version_minor() noexcept -> valid_if_nonnegative<int> {
#if defined(__clang_minor__)
    return {__clang_minor__};
#elif defined(__INTEL_COMPILER)
    return {(__INTEL_COMPILER % 100) / 10};
#elif defined(__DMC__)
    return {(__DMC__ >> 4U) & 0xFU};
#elif defined(__EDG_VERSION__)
    return {(__EDG_VERSION__ % 100) / 1};
#elif defined(__CWCC__)
    return {(__CWCC__ % 1000) / 100};
#elif defined(_MSC_VER)
    return {(_MSC_VER % 100) / 1};
#elif defined(__COMO_VERSION__)
    return {(__COMO_VERSION__ % 100) / 10};
#elif defined(__MINGW64_VERSION_MINOR)
    return {__MINGW64_VERSION_MINOR};
#elif defined(__MINGW32_VERSION_MINOR)
    return {__MINGW32_VERSION_MINOR};
#elif defined(__GNUC_MINOR__)
    return {__GNUC_MINOR__};
#endif
    return {-1};
}
//------------------------------------------------------------------------------
auto compiler_version_patch() noexcept -> valid_if_nonnegative<int> {
#if defined(__clang_patchlevel__)
    return {__clang_patchlevel__};
#elif defined(__INTEL_COMPILER)
    return {(__INTEL_COMPILER % 10) / 1};
#elif defined(__DMC__)
    return {(__DMC__ >> 0U) & 0xFU};
#elif defined(__CWCC__)
    return {(__CWCC__ % 100) / 1};
#elif defined(__GNUC_PATCHLEVEL__)
    return {__GNUC_PATCHLEVEL__};
#endif
    return {-1};
}
//------------------------------------------------------------------------------
// os
//------------------------------------------------------------------------------
auto config_os_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_OS_INFO_HPP)
    return {string_view{EAGINE_OS_NAME}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_os_code_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_OS_INFO_HPP)
    return {string_view{EAGINE_OS_CODE_NAME}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
// version
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor) const -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(opt_maj < major) {
            return false;
        }
        if(opt_maj > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(opt_min < minor) {
                return false;
            }
            return true;
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor, int patch) const
  -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(opt_maj < major) {
            return false;
        }
        if(opt_maj > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(opt_min < minor) {
                return false;
            }
            if(opt_min > minor) {
                return true;
            }
            if(const auto opt_ptch{version_patch()}) {
                if(opt_ptch < patch) {
                    return false;
                }
                return true;
            }
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor, int patch, int commit)
  const -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(opt_maj < major) {
            return false;
        }
        if(opt_maj > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(opt_min < minor) {
                return false;
            }
            if(opt_min > minor) {
                return true;
            }
            if(const auto opt_ptch{version_patch()}) {
                if(opt_ptch < patch) {
                    return false;
                }
                if(opt_ptch > patch) {
                    return true;
                }
                if(const auto opt_cmit{version_commit()}) {
                    if(opt_cmit < commit) {
                        return false;
                    }
                    return true;
                }
            }
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto parse_version_string(
  const string_view str,
  int& major,
  int& minor,
  int& patch,
  int& commit) noexcept -> bool {
    static const std::regex re{R"(([0-9]+)\.([0-9]+)\.([0-9]+)[.-]([0-9]+))"};
    std::match_results<string_view::iterator> match;
    if(std::regex_match(str.begin(), str.end(), match, re)) {
        major = std::atoi(match[1].str().c_str());
        minor = std::atoi(match[2].str().c_str());
        patch = std::atoi(match[3].str().c_str());
        commit = std::atoi(match[4].str().c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
auto parse_version_string(
  const string_view str,
  int& major,
  int& minor,
  int& patch) noexcept -> bool {
    static const std::regex re{R"(([0-9]+)\.([0-9]+)\.([0-9]+))"};
    std::match_results<string_view::iterator> match;
    if(std::regex_match(str.begin(), str.end(), match, re)) {
        major = std::atoi(match[1].str().c_str());
        minor = std::atoi(match[2].str().c_str());
        patch = std::atoi(match[3].str().c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
auto parse_version_string(const string_view str, int& major, int& minor) noexcept
  -> bool {
    static const std::regex re{R"(([0-9]+)\.([0-9]+))"};
    std::match_results<string_view::iterator> match;
    if(std::regex_match(str.begin(), str.end(), match, re)) {
        major = std::atoi(match[1].str().c_str());
        minor = std::atoi(match[2].str().c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
auto version_info::version_at_least(const string_view str) const -> tribool {
    int major{0}, minor{0}, patch{0}, commit{0};
    if(parse_version_string(str, major, minor, patch, commit)) {
        return version_at_least(major, minor, patch, commit);
    }
    if(parse_version_string(str, major, minor, patch)) {
        return version_at_least(major, minor, patch);
    }
    if(parse_version_string(str, major, minor)) {
        return version_at_least(major, minor);
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
// build
//------------------------------------------------------------------------------
auto build_info::install_prefix() const noexcept
  -> valid_if_not_empty<string_view> {
    return {EAGINE_INSTALL_PREFIX};
}
//------------------------------------------------------------------------------
auto build_info::config_dir_path() const noexcept
  -> valid_if_not_empty<string_view> {
    return {EAGINE_INSTALL_PREFIX "/etc"};
}
//------------------------------------------------------------------------------
} // namespace eagine
