/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.value_tree;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.logging;

namespace eagine::valtree {
//------------------------------------------------------------------------------
class filesystem_compound;
class filesystem_node;
//------------------------------------------------------------------------------
[[nodiscard]] static auto filesystem_make_node(
  filesystem_compound& owner,
  const std::filesystem::path& fs_path)
  -> optional_reference<attribute_interface>;
[[nodiscard]] static auto get_log(filesystem_compound& owner) -> const logger&;
//------------------------------------------------------------------------------
class filesystem_node : public attribute_interface {
public:
    [[nodiscard]] filesystem_node(
      std::filesystem::path node_path,
      std::filesystem::path real_path)
      : _node_path{std::move(node_path)}
      , _real_path{std::move(real_path)}
      , _name{_node_path.filename()} {}

    [[nodiscard]] filesystem_node(const std::filesystem::path& fs_path)
      : filesystem_node{fs_path, canonical(fs_path)} {}

    friend auto operator==(
      const filesystem_node& l,
      const filesystem_node& r) noexcept -> bool {
        return l._node_path == r._node_path;
    }

    auto type_id() const noexcept -> identifier final {
        return "filesystem";
    }

    auto name() -> string_view {
        return {_name};
    }

    auto is_link() const -> bool {
        return is_symlink(_node_path);
    }

    auto nested_count(filesystem_compound& owner) -> span_size_t {
        if(is_directory(_real_path)) {
            try {
                const auto iter =
                  std::filesystem::directory_iterator(_node_path);
                return span_size(std::distance(begin(iter), end(iter)));
            } catch(const std::filesystem::filesystem_error& err) {
                get_log(owner)
                  .debug("failed to get filesystem node count")
                  .arg("path", _node_path)
                  .arg("error", err);
            }
        }
        return 0;
    }

    auto nested(filesystem_compound& owner, span_size_t index)
      -> optional_reference<attribute_interface> {
        try {
            if(is_directory(_real_path)) {
                for(auto& ent :
                    std::filesystem::directory_iterator(_node_path)) {
                    if(index > 0) {
                        --index;
                    } else {
                        return filesystem_make_node(owner, ent);
                    }
                }
            }
        } catch(const std::filesystem::filesystem_error& err) {
            get_log(owner)
              .debug("failed to get nested filesystem node for '${path}'")
              .arg("index", index)
              .arg("path", _node_path)
              .arg("error", err);
        }
        return {};
    }

    auto nested(filesystem_compound& owner, string_view name)
      -> optional_reference<attribute_interface> {
        try {
            if(is_directory(_real_path)) {
                for(auto& ent :
                    std::filesystem::directory_iterator(_node_path)) {
                    const std::filesystem::path& epath{ent};
                    const std::string& ename{epath.filename()};
                    if(are_equal(name, string_view(ename))) {
                        return filesystem_make_node(owner, epath);
                    }
                }
            }
        } catch(const std::filesystem::filesystem_error& err) {
            get_log(owner)
              .debug("failed to get nested filesystem node for '${path}'")
              .arg("name", name)
              .arg("path", _node_path)
              .arg("error", err);
        }
        return {};
    }

    auto find(filesystem_compound& owner, const basic_string_path& path)
      -> optional_reference<attribute_interface> {
        auto spath{_node_path};
        for(auto ent : path) {
            spath.append(std::string_view(ent));
        }
        if(exists(spath)) {
            return filesystem_make_node(owner, spath);
        }
        return {};
    }

    auto find(
      filesystem_compound& owner,
      const basic_string_path& path,
      span<const string_view> tags) -> optional_reference<attribute_interface> {
        std::string temp_str;
        auto _cat =
          [&](string_view a, string_view b, string_view c) mutable -> auto& {
            return append_to(c, append_to(b, assign_to(a, temp_str)));
        };

        auto spath{_node_path};
        for(auto ent : path) {
            bool found = false;
            for(auto tag : tags) {
                auto tpath{spath};
                tpath.append(_cat(ent, "@", tag));
                if(exists(tpath)) {
                    spath = std::move(tpath);
                    found = true;
                    break;
                }
            }
            if(not found) {
                spath.append(std::string_view(ent));
            }
        }
        if(exists(spath)) {
            return filesystem_make_node(owner, spath);
        }
        return {};
    }

    auto value_count() -> span_size_t {
        if(is_regular_file(_real_path)) {
            return limit_cast<span_size_t>(file_size(_real_path));
        }
        return 0;
    }

    auto fetch_values(span_size_t offset, memory::block dest) -> span_size_t {
        if(exists(_real_path) and is_regular_file(_real_path)) {
            std::ifstream file;
            file.open(_real_path, std::ios::in | std::ios::binary);
            file.seekg(offset, std::ios::beg);
            if(not file
                     .read(
                       reinterpret_cast<char*>(dest.data()),
                       static_cast<std::streamsize>(dest.size()))
                     .bad()) {
                return span_size(file.gcount());
            }
        }
        return 0;
    }

    auto fetch_values(span_size_t offset, span<char> dest) -> span_size_t {
        if(exists(_real_path) and is_regular_file(_real_path)) {
            std::ifstream file;
            file.open(_real_path, std::ios::in);
            file.seekg(offset, std::ios::beg);
            if(not file
                     .read(
                       reinterpret_cast<char*>(dest.data()),
                       static_cast<std::streamsize>(dest.size()))
                     .bad()) {
                return span_size(file.gcount());
            }
        }
        return 0;
    }

    auto fetch_values(span_size_t offset, span<std::string> dest)
      -> span_size_t {
        if(dest.has_single_value()) {
            std::ifstream file;
            file.open(_real_path, std::ios::in);
            file.seekg(offset, std::ios::beg);
            std::array<char, 1024> temp{{}};
            auto& dst_str{dest.front()};
            dst_str.clear();
            while(not file.eof()) {
                const auto done{file
                                  .read(
                                    reinterpret_cast<char*>(temp.data()),
                                    static_cast<std::streamsize>(temp.size()))
                                  .gcount()};
                if(file.bad()) {
                    return 0;
                }
                if(not done) {
                    break;
                }
                dst_str.append(temp.data(), span_size(done));
            }
            return span_size_t(1);
        } else if(not dest.empty()) {
            std::ifstream file;
            file.open(_real_path, std::ios::in);
            file.seekg(offset, std::ios::beg);
            for(auto& dst_str : dest) {
                if(file.eof()) {
                    break;
                }
                if(file.bad()) {
                    return 0;
                }
                std::getline(file, dst_str);
            }
            return dest.size();
        }
        return 0;
    }

    template <typename T>
    auto fetch_values(span_size_t offset, span<T> dest) -> span_size_t {
        if(dest.has_single_value()) {
            std::array<char, 64> temp{{}};
            if(const string_view src{take_until(
                 head(memory::view(temp), fetch_values(offset, cover(temp))),
                 [](const char c) { return not c or std::isspace(c); })}) {
                if(assign_if_fits(src, dest.front())) {
                    return 1;
                }
            }
        }
        return 0;
    }

    auto preview() -> optionally_valid<string_view> {
        // TODO: fetch couple of bytes from the start of the file?
        return {};
    }

private:
    const std::filesystem::path _node_path;
    const std::filesystem::path _real_path;
    const std::string _name;
};
//------------------------------------------------------------------------------
class filesystem_compound
  : public compound_with_refcounted_node<filesystem_compound, filesystem_node> {
    using base =
      compound_with_refcounted_node<filesystem_compound, filesystem_node>;
    using base::_unwrap;

    logger _log;
    filesystem_node _root;
    shared_holder<file_compound_factory> _compound_factory;

public:
    [[nodiscard]] filesystem_compound(
      const logger& parent,
      string_view fs_path,
      shared_holder<file_compound_factory> factory)
      : _log{"FsVtCmpnd", parent}
      , _root{std::string_view{fs_path}}
      , _compound_factory{std::move(factory)} {}

    [[nodiscard]] static auto make_shared(
      const logger& parent,
      string_view fs_path,
      shared_holder<file_compound_factory> factory)
      -> shared_holder<filesystem_compound> {
        return {default_selector, parent, fs_path, std::move(factory)};
    }

    auto log() const noexcept -> const logger& {
        return _log;
    }

    auto type_id() const noexcept -> identifier final {
        return "filesystem";
    }

    auto structure() -> optional_reference<attribute_interface> final {
        return _root;
    }

    auto attribute_name(attribute_interface& attrib) -> string_view final {
        return _unwrap(attrib).name();
    }

    auto attribute_preview(attribute_interface& attrib)
      -> optionally_valid<string_view> final {
        return _unwrap(attrib).preview();
    }

    auto canonical_type(attribute_interface&) -> value_type final {
        return value_type::byte_type;
    }

    auto is_immutable(attribute_interface&) -> bool final {
        return false;
    }

    auto is_link(attribute_interface& attrib) -> bool final {
        return _unwrap(attrib).is_link();
    }

    auto nested_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).nested_count(*this);
    }

    auto nested(attribute_interface& attrib, span_size_t index)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, index);
    }

    auto nested(attribute_interface& attrib, string_view name)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, name);
    }

    auto find(attribute_interface& attrib, const basic_string_path& path)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).find(*this, path);
    }

    auto find(
      attribute_interface& attrib,
      const basic_string_path& path,
      span<const string_view> tags)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).find(*this, path, tags);
    }

    auto value_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).value_count();
    }

    template <typename T>
    auto do_fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<T> dest) -> span_size_t {
        return _unwrap(attrib).fetch_values(offset, dest);
    }
};
//------------------------------------------------------------------------------
static inline auto get_log(filesystem_compound& owner) -> const logger& {
    return owner.log();
}
//------------------------------------------------------------------------------
static inline auto filesystem_make_node(
  filesystem_compound& owner,
  const std::filesystem::path& fs_path)
  -> optional_reference<attribute_interface> {
    if(not fs_path.empty()) {
        try {
            return owner.make_node(fs_path, canonical(fs_path));
        } catch(const std::filesystem::filesystem_error&) {
            owner.log()
              .debug("failed to create filesystem node '${path}'")
              .arg("path", "FsPath", fs_path);
        }
    }
    return {};
}
//------------------------------------------------------------------------------
[[nodiscard]] auto from_filesystem_path(
  string_view fs_path,
  const logger& parent,
  shared_holder<file_compound_factory> factory) -> compound {
    return compound::make<filesystem_compound>(
      parent, fs_path, std::move(factory));
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
