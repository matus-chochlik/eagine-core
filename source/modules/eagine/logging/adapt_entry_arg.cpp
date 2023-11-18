/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_entry_arg;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;
import eagine.core.build_info;
import eagine.core.valid_if;
import eagine.core.string;
import eagine.core.c_api;

namespace eagine {
//------------------------------------------------------------------------------
export struct yes_no_maybe : tribool {
    constexpr yes_no_maybe(tribool v) noexcept
      : tribool{v} {}
};
//------------------------------------------------------------------------------
struct yes_no_maybe_entry_adapter {
    const identifier name;
    const yes_no_maybe value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(
          name,
          "YesNoMaybe",
          value.is(true)    ? string_view{"yes"}
          : value.is(false) ? string_view{"no"}
                            : string_view{"maybe"});
    }
};
export auto adapt_entry_arg(const identifier name, yes_no_maybe value) noexcept {
    return yes_no_maybe_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
constexpr auto adapt_entry_arg(
  const identifier name,
  const char* value) noexcept {
    return [=](auto& backend) {
        backend.add_string(name, "str", value);
    };
}
//------------------------------------------------------------------------------
constexpr auto adapt_entry_arg(
  const identifier name,
  const string_view value) noexcept {
    return [=](auto& backend) {
        backend.add_string(name, "str", value);
    };
}
//------------------------------------------------------------------------------
constexpr auto adapt_entry_arg(
  const identifier name,
  const std::string& value) noexcept {
    return [=](auto& backend) {
        backend.add_string(name, "str", value);
    };
}
//------------------------------------------------------------------------------
export template <typename Self, typename T, identifier_t LibId, identifier_t Id>
constexpr auto adapt_entry_arg(
  const identifier name,
  const c_api::enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept
    requires(std::is_signed_v<T>)
{
    return [=](auto& backend) {
        backend.add_integer(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
export template <typename Self, typename T, identifier_t LibId, identifier_t Id>
constexpr auto adapt_entry_arg(
  const identifier name,
  const c_api::enum_class<Self, T, LibId, Id> ec,
  const T = {}) noexcept
    requires(std::is_unsigned_v<T>)
{
    return [=](auto& backend) {
        backend.add_unsigned(name, identifier{Id}, ec._value);
    };
}
//------------------------------------------------------------------------------
struct std_exception_entry_adapter {
    const identifier name;
    const std::exception& value;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "Exception", string_view(value.what()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::exception& value) noexcept {
    return std_exception_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct std_runtime_error_entry_adapter {
    const identifier name;
    const std::runtime_error& value;
    void operator()(auto& backend) noexcept {
        backend.add_string(name, "RuntmError", string_view(value.what()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::runtime_error& value) noexcept {
    return std_runtime_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct std_system_error_entry_adapter {
    const identifier name;
    const std::system_error& value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "SystmError", string_view(value.what()));
        backend.add_string(
          "category",
          "ErrorCtgry",
          string_view(value.code().category().name()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::system_error& value) noexcept {
    return std_system_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct string_path_entry_adapter {
    const identifier name;
    const basic_string_path value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "StringPath", value.as_string());
    }
};
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const basic_string_path& value) noexcept {
    return string_path_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct std_filesystem_path_entry_adapter {
    const identifier name;
    const std::filesystem::path fsp;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "FsPath", string_view(fsp.native()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::path& fsp) noexcept {
    return std_filesystem_path_entry_adapter{.name = name, .fsp = fsp};
}
//------------------------------------------------------------------------------
struct std_filesystem_error_entry_adapter {
    const identifier name;
    const std::filesystem::filesystem_error value;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "FlSysError", string_view(value.what()));
        backend.add_string(
          "category",
          "ErrorCtgry",
          string_view(value.code().category().name()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::filesystem_error& value) noexcept {
    return std_filesystem_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct program_arg_entry_adapter {
    const identifier name;
    const program_arg& value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "ProgramArg", string_view(value));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const program_arg& value) noexcept {
    return program_arg_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
struct version_info_entry_adapter {
    const identifier name;
    const version_info& value;

    void operator()(auto& backend) const noexcept {
        if(const ok maj{value.version_major()}) {
            backend.add_integer(name, "VerMajor", maj);
            if(const ok min{value.version_minor()}) {
                backend.add_integer(name, "VerMinor", min);
                if(const ok ptch{value.version_patch()}) {
                    backend.add_integer(name, "VerPatch", ptch);
                    if(const ok cmit{value.version_commit()}) {
                        backend.add_integer(name, "VerCommit", cmit);
                    }
                }
            }
        }
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const version_info& value) noexcept {
    return version_info_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

