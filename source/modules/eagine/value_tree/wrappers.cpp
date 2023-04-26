/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.value_tree:wrappers;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.identifier;
import eagine.core.reflection;
import :interface;

namespace eagine::valtree {
//------------------------------------------------------------------------------
export class compound;

/// @brief Handle class for value tree attributes.
/// @ingroup valtree
/// @see compound
/// @see compound_attribute
///
/// Attributes act as key that allow access values and nested attributes
/// of some structured tree data hierarchy (JSON, YAML, XML, filesystem
/// hierarchy, etc.). An attribute represents a single node in such hierarchy
/// and can be used to retrieve the values or data stored at that level in the
/// tree hierarchy and enumerate or find nested nodes.
export class attribute {
public:
    /// @brief Default constructor. Constructs empty attribute refering to nothing.
    attribute() noexcept = default;

    /// @brief Move constructor.
    attribute(attribute&& temp) noexcept
      : _owner{std::move(temp._owner)}
      , _pimpl{std::exchange(temp._pimpl, nullptr)} {}

    /// @brief Copy constructor. Handles attribute reference counting.
    attribute(const attribute& that)
      : _owner{that._owner}
      , _pimpl{that._pimpl} {
        if(_owner and _pimpl) {
            _owner->add_ref(*_pimpl);
        }
    }

    /// @brief Move assignment operator.
    auto operator=(attribute&& temp) noexcept -> attribute& {
        if(this != std::addressof(temp)) {
            using std::swap;
            swap(_owner, temp._owner);
            swap(_pimpl, temp._pimpl);
        }
        return *this;
    }

    /// @brief Copy assignment operator. Handles attribute reference counting.
    auto operator=(const attribute& that) -> attribute& {
        if(this != std::addressof(that)) {
            using std::swap;
            attribute temp{std::move(*this)};
            _owner = that._owner;
            _pimpl = that._pimpl;
            if(_owner and _pimpl) {
                _owner->add_ref(*_pimpl);
            }
        }
        return *this;
    }

    /// @brief Destructor. Handles attribute reference counting.
    ~attribute() noexcept {
        if(_pimpl) {
            assert(_owner);
            _owner->release(*_pimpl);
        }
    }

    /// @brief Indicates if this attribute actually refers to something.
    [[nodiscard]] explicit operator bool() const {
        return _owner and _pimpl;
    }

    /// @brief Returns the implementation type id of this attribute.
    [[nodiscard]] auto type_id() const noexcept -> identifier {
        return _pimpl.member(&attribute_interface::type_id)
          .value_or(identifier{});
    }

    /// @brief Returns the implementation type id of this attribute.
    [[nodiscard]] auto name() const -> string_view {
        if(_owner and _pimpl) {
            return _owner->attribute_name(*_pimpl);
        }
        return {};
    }

    template <typename Implementation>
        requires(std::is_base_of_v<attribute_interface, Implementation>)
    [[nodiscard]] auto as() noexcept -> optional_reference<Implementation> {
        return _pimpl.as<Implementation>();
    }

private:
    friend class compound;

    attribute(
      std::shared_ptr<compound_interface> owner,
      optional_reference<attribute_interface> impl) noexcept
      : _owner{std::move(owner)}
      , _pimpl{impl} {}

    std::shared_ptr<compound_interface> _owner;
    optional_reference<attribute_interface> _pimpl{};
};
//------------------------------------------------------------------------------
export class compound_attribute;
//------------------------------------------------------------------------------
export template <typename T>
struct not_converted_value {
    constexpr not_converted_value(T& dest) noexcept
      : _dest{dest} {}

    constexpr auto dest() noexcept -> auto& {
        return _dest;
    }

    template <identifier_t V>
    constexpr auto apply(const selector<V>) const noexcept {
        return true;
    }

private:
    T& _dest;
};
//------------------------------------------------------------------------------
export template <typename T>
struct converted_enum_value {
    static_assert(default_mapped_enum<T>);

public:
    constexpr converted_enum_value(T& dest) noexcept
      : _dest{dest} {}

    constexpr auto dest() noexcept -> auto& {
        return _temp;
    }

    template <identifier_t V>
    auto apply(const selector<V> sel) const noexcept -> bool {
        return assign_if_fits(_temp, _dest, sel);
    }

private:
    std::string _temp;
    T& _dest;
};
//------------------------------------------------------------------------------
export template <typename T>
struct converted_value
  : std::conditional_t<
      default_mapped_enum<T>,
      converted_enum_value<T>,
      not_converted_value<T>> {

    using base = std::conditional_t<
      default_mapped_enum<T>,
      converted_enum_value<T>,
      not_converted_value<T>>;

    using base::base;
};
//------------------------------------------------------------------------------
export template <>
struct converted_value<std::chrono::duration<float>>
  : not_converted_value<std::chrono::duration<float>> {
    using base = not_converted_value<std::chrono::duration<float>>;
    using base::base;
};
//------------------------------------------------------------------------------
export template <typename R, typename P>
class converted_value<std::chrono::duration<R, P>> {
    using T = std::chrono::duration<R, P>;

public:
    constexpr converted_value(T& dest) noexcept
      : _dest{dest} {}

    constexpr auto dest() noexcept -> auto& {
        return _temp;
    }

    template <identifier_t V>
    auto apply(const selector<V>) const {
        _dest = std::chrono::duration_cast<T>(_temp);
        return true;
    }

private:
    std::chrono::duration<float> _temp{};
    T& _dest;
};
//------------------------------------------------------------------------------
/// @brief Handle class for value tree compounds.
/// @ingroup valtree
/// @see compound
/// @see compound_attribute
///
/// Compounds own, manage and parse the actual tree data
/// of some structured tree hierarchy (JSON, YAML, XML, filesystem
/// hierarchy, etc.). A compound represents the parsed or "open" instance
/// of such a tree and allows to traverse and explore the tree structure
/// and access the stored data through an unified interface.
export class compound {
public:
    /// @brief Default constructor. Constructs an empty compound.
    compound() noexcept = default;

    /// @brief Instantiates a particular implementation.
    /// @note Do not use directly in client code. Use one of the constructor
    /// functions that know which implementation to pick and how to initialize it.
    template <typename Compound, typename... Args>
    [[nodiscard]] static auto make(Args&&... args) -> compound
        requires(std::is_base_of_v<compound_interface, Compound>)
    {
        return {Compound::make_shared(std::forward<Args>(args)...)};
    }

    /// @brief Indicates if this compound actually refers to some tree.
    [[nodiscard]] explicit operator bool() const noexcept {
        return bool(_pimpl);
    }

    /// @brief Returns the implementation type id of this attribute.
    [[nodiscard]] auto type_id() const noexcept -> identifier {
        if(_pimpl) {
            return _pimpl->type_id();
        }
        return {};
    }

    /// @brief Returns the root of attribute hierarchy describing the tree structure.
    /// @see root
    ///
    /// The returned attribute can be used to explore and traverse the tree
    /// node hierarchy by getting the names, value types and nested nodes.
    [[nodiscard]] auto structure() const -> attribute {
        if(_pimpl) {
            return {_pimpl, _pimpl->structure()};
        }
        return {};
    }

    /// @brief Returns the structure root as an compound_attribute.
    /// @see structure
    [[nodiscard]] auto root() const -> compound_attribute;

    /// @brief Returns the name of an attribute.
    /// @pre this->type_id() == attrib.type_id().
    [[nodiscard]] auto attribute_name(const attribute& attrib) const
      -> string_view {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->attribute_name(*attrib._pimpl);
        }
        return {};
    }

    /// @brief Returns the caninical value type of an attribute.
    /// @pre this->type_id() == attrib.type_id().
    ///
    /// Tree data referred-to by an attribute can be fetched either using
    /// the canonical type or using a different, related value type, if the
    /// necessary conversion is implemented.
    [[nodiscard]] auto canonical_type(const attribute& attrib) const
      -> value_type {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->canonical_type(*attrib._pimpl);
        }
        return value_type::unknown;
    }

    /// @brief Indicates if the specified attribute is immutable.
    /// @pre this->type_id() == attrib.type_id()
    [[nodiscard]] auto is_immutable(const attribute& attrib) const -> bool {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->is_immutable(*attrib._pimpl);
        }
        return false;
    }

    /// @brief Indicates if the specified attribute is a reference or link in the tree.
    /// @pre this->type_id() == attrib.type_id()
    [[nodiscard]] auto is_link(const attribute& attrib) const -> bool {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->is_link(*attrib._pimpl);
        }
        return false;
    }

    /// @brief Returns the count of nested attributes of an attribute.
    /// @pre this->type_id() == attrib.type_id()
    /// @note Some implementations may return zero here even if there are
    /// nested attributes. In such implementations the nested nodes can be
    /// traversed only by name.
    [[nodiscard]] auto nested_count(const attribute& attrib) const
      -> span_size_t {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->nested_count(*attrib._pimpl);
        }
        return 0;
    }

    /// @brief Indicates if an attribute has nested attribute accessible by index.
    /// @pre this->type_id() == attrib.type_id()
    [[nodiscard]] auto has_nested(const attribute& attrib) const -> bool {
        return nested_count(attrib) != 0;
    }

    /// @brief Returns nested attribute of an attribute at the specified index.
    /// @pre this->type_id() == attrib.type_id()
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto nested(const attribute& attrib, const span_size_t index)
      const -> attribute {
        if(_pimpl and attrib._pimpl) {
            return {_pimpl, _pimpl->nested(*attrib._pimpl, index)};
        }
        return {};
    }

    /// @brief Returns nested attribute of an attribute with the specified name.
    /// @pre this->type_id() == attrib.type_id()
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto nested(const attribute& attrib, const string_view name)
      const -> attribute {
        if(_pimpl and attrib._pimpl) {
            return {_pimpl, _pimpl->nested(*attrib._pimpl, name)};
        }
        return {};
    }

    /// @brief Returns nested attribute of the root attribute with the specified name.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto nested(const string_view name) const -> attribute {
        return nested(structure(), name);
    }

    /// @brief Returns nested attribute of an attribute at the specified path.
    /// @pre this->type_id() == attrib.type_id()
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    auto find(const attribute& attrib, const basic_string_path& path) const
      -> attribute {
        if(_pimpl and attrib._pimpl) {
            return {_pimpl, _pimpl->find(*attrib._pimpl, path)};
        }
        return {};
    }

    /// @brief Returns nested attribute of an attribute at path with tags.
    /// @pre this->type_id() == attrib.type_id()
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto find(
      const attribute& attrib,
      const basic_string_path& path,
      const memory::span<const string_view> tags) const -> attribute {
        if(_pimpl and attrib._pimpl) {
            return {_pimpl, _pimpl->find(*attrib._pimpl, path, tags)};
        }
        return {};
    }

    /// @brief Returns nested attribute of root attribute at the specified path.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto find(const basic_string_path& path) const -> attribute {
        return find(structure(), path);
    }

    /// @brief Returns nested attribute of root attribute at path with tags.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto find(
      const basic_string_path& path,
      const memory::span<const string_view> tags) const -> attribute {
        return find(structure(), path, tags);
    }

    /// @brief Returns the number of value elements accessible through an attribute.
    [[nodiscard]] auto value_count(const attribute& attrib) const
      -> span_size_t {
        if(_pimpl and attrib._pimpl) {
            return _pimpl->value_count(*attrib._pimpl);
        }
        return 0;
    }

    /// @brief Returns the number of value elements at the specified path.
    [[nodiscard]] auto value_count(const basic_string_path& path) const
      -> span_size_t {
        return value_count(find(path));
    }

    /// @brief Returns the number of value elements at attribute with the given name.
    [[nodiscard]] auto value_count(const string_view name) -> span_size_t {
        return value_count(nested(name));
    }

    /// @brief Fetches values at the given attribute, starting at offset into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const attribute& attrib,
      const span_size_t offset,
      memory::span<T> dest) const -> memory::span<T> {
        if(_pimpl and attrib._pimpl) {
            return head(
              dest, _pimpl->fetch_values(*attrib._pimpl, offset, dest));
        }
        return {};
    }

    /// @brief Fetches values at the given path, starting at offset into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const basic_string_path& path,
      const span_size_t offset,
      memory::span<T> dest) const -> memory::span<T> {
        return fetch_values(find(path), offset, dest);
    }

    /// @brief Fetches values at the given name, starting at offset into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const string_view name,
      const span_size_t offset,
      memory::span<T> dest) const -> memory::span<T> {
        return fetch_values(nested(name), offset, dest);
    }

    /// @brief Fetches values at the given attribute, into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const attribute& attrib,
      memory::span<T> dest) const -> memory::span<T> {
        return fetch_values(attrib, 0, dest);
    }

    /// @brief Fetches values at the attribute with the specified path, into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const basic_string_path& path,
      memory::span<T> dest) const -> memory::span<T> {
        return fetch_values(path, 0, dest);
    }

    /// @brief Fetches values at the attribute with the specified name, into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const string_view name,
      memory::span<T> dest) const -> memory::span<T> {
        return fetch_values(name, 0, dest);
    }

    /// @brief Fetches a BLOB at the given attribute, into @p dest.
    [[nodiscard]] auto fetch_blob(const attribute& attrib, memory::block dest)
      const -> memory::block {
        return fetch_values(attrib, dest);
    }

    /// @brief Fetches a BLOB at the attribute with the specified path, into @p dest.
    [[nodiscard]] auto fetch_blob(
      const basic_string_path& path,
      memory::block dest) const -> memory::block {
        return fetch_values(path, dest);
    }

    /// @brief Fetches a BLOB at the attribute with the specified name, into @p dest.
    [[nodiscard]] auto fetch_blob(const string_view name, memory::block dest)
      const -> memory::block {
        return fetch_values(name, dest);
    }

    /// @brief Fetches a single value at the specified attribute, with a selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const attribute& attrib,
      const span_size_t offset,
      T& dest,
      const selector<V> sel) const -> bool {
        converted_value<T> conv{dest};
        if(not fetch_values(attrib, offset, cover_one(conv.dest())).empty()) {
            return conv.apply(sel);
        }
        return false;
    }

    /// @brief Fetches a single value at the specified attribute, at offset into @p dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(
      const attribute& attrib,
      const span_size_t offset,
      T& dest) const -> bool {
        return select_value(attrib, offset, dest, default_selector);
    }

    /// @brief Fetches values at the specified attribute, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_values(
      const attribute& attrib,
      const span_size_t offset,
      memory::span<T> dest,
      const selector<V> sel) const -> memory::span<T> {
        span_size_t index = 0;
        for(T& elem : dest) {
            if(not select_value(attrib, offset + index, elem, sel)) {
                index = 0;
                break;
            }
            ++index;
        }
        return head(dest, index);
    }

    /// @brief Fetches values through the specified name, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const string_view name,
      const span_size_t offset,
      T& dest,
      const selector<V> sel) const -> bool {
        return select_value(nested(name), offset, dest, sel);
    }

    /// @brief Fetches values through the specified name, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(
      const string_view name,
      const span_size_t offset,
      T& dest) const -> bool {
        return select_value(name, offset, dest, default_selector);
    }

    /// @brief Fetches values through the specified path, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const basic_string_path& path,
      const span_size_t offset,
      T& dest,
      const selector<V> sel) const -> bool {
        return select_value(find(path), offset, dest, sel);
    }

    /// @brief Fetches values through the specified path, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(
      const basic_string_path& path,
      const span_size_t offset,
      T& dest) const -> bool {
        return select_value(path, offset, dest, default_selector);
    }

    /// @brief Fetches values through the specified name, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const string_view name,
      T& dest,
      const selector<V> sel) const -> bool {
        return select_value(name, 0, dest, sel);
    }

    /// @brief Fetches values through the specified name, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(const string_view name, T& dest) const
      -> bool {
        return select_value(name, 0, dest, default_selector);
    }

    /// @brief Fetches a value at the specified attribute, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const attribute& attrib,
      T& dest,
      const selector<V> sel) const -> bool {
        return select_value(attrib, 0, dest, sel);
    }

    /// @brief Fetches values at the specified attribute, with a selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_values(
      const attribute& attrib,
      memory::span<T> dest,
      selector<V> sel) const -> memory::span<T> {
        return select_values(attrib, 0, dest, sel);
    }

    /// @brief Fetches a single value at the specified attribute, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(const attribute& attrib, T& dest) const
      -> bool {
        return select_value(attrib, 0, dest, default_selector);
    }

    /// @brief Fetches a value through the specified path, with selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(
      const basic_string_path& path,
      T& dest,
      const selector<V> sel) const -> bool {
        return select_value(path, 0, dest, sel);
    }

    /// @brief Fetches a value through the specified path, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(const basic_string_path& path, T& dest) const
      -> bool {
        return selector_value(path, 0, dest, default_selector);
    }

    /// @brief Tests if there is an value at an attribute, that starts with @p what.
    template <std::size_t L>
    [[nodiscard]] auto has_value(const attribute& attrib, const char (&what)[L])
      const -> bool {
        char temp[L]{};
        if(fetch_values(attrib, 0, cover(temp))) {
            return starts_with(string_view(temp), string_view(what));
        }
        return false;
    }

    /// @brief Returns the value of type T at an attribute, at offset, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const attribute& attrib,
      const span_size_t offset,
      const std::type_identity<T>,
      const selector<V> sel) const -> std::optional<T> {
        T temp{};
        if(select_value(attrib, offset, temp, sel)) {
            return {std::move(temp)};
        }
        return {};
    }

    /// @brief Returns the value of type T at an attribute, at the given offset.
    template <typename T>
    [[nodiscard]] auto get(
      const attribute& attrib,
      const span_size_t offset,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(attrib, offset, tid, default_selector);
    }

    /// @brief Returns the value of type T at path, at given offset, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const basic_string_path& path,
      const span_size_t offset,
      const std::type_identity<T>,
      const selector<V> sel) const -> std::optional<T> {
        T temp{};
        if(select_value(path, offset, temp, sel)) {
            return {std::move(temp)};
        }
        return {};
    }

    /// @brief Returns the value of type T at path, at given offset.
    template <typename T>
    [[nodiscard]] auto get(
      const basic_string_path& path,
      const span_size_t offset,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(path, offset, tid, default_selector);
    }

    /// @brief Returns the value of type T at name, at given offset, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const string_view name,
      const span_size_t offset,
      const std::type_identity<T>,
      const selector<V> sel) const -> std::optional<T> {
        T temp{};
        if(select_value(name, offset, temp, sel)) {
            return {std::move(temp)};
        }
        return {};
    }

    /// @brief Returns the value of type T at name, at given offset.
    template <typename T>
    [[nodiscard]] auto get(
      const string_view name,
      const span_size_t offset,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(name, offset, tid, default_selector);
    }

    /// @brief Returns the value of type T at an attribute, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const attribute& attrib,
      const std::type_identity<T> tid,
      const selector<V> sel) const -> std::optional<T> {
        return get<T>(attrib, 0, tid, sel);
    }

    /// @brief Returns the value of type T at an attribute.
    template <typename T>
    [[nodiscard]] auto get(
      const attribute& attrib,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(attrib, tid, default_selector);
    }

    /// @brief Returns the value of type T at path, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const basic_string_path& path,
      const std::type_identity<T> tid,
      const selector<V> sel) const -> std::optional<T> {
        return get<T>(path, 0, tid, sel);
    }

    /// @brief Returns the value of type T at path.
    template <typename T>
    [[nodiscard]] auto get(
      const basic_string_path& path,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(path, tid, default_selector);
    }

    /// @brief Returns the value of type T at name, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto get(
      const string_view name,
      const std::type_identity<T> tid,
      const selector<V> sel) const -> std::optional<T> {
        return get<T>(name, 0, tid, sel);
    }

    /// @brief Returns the value of type T at name.
    template <typename T>
    [[nodiscard]] auto get(
      const string_view name,
      const std::type_identity<T> tid = {}) const -> std::optional<T> {
        return get<T>(name, tid, default_selector);
    }

    /// @brief Type of traverse/visit handler.
    using visit_handler = callable_ref<
      bool(const compound&, const attribute&, const basic_string_path&)>;

    /// @brief Traverses the tree, calls the @p visitor function on each node.
    void traverse(const visit_handler visitor) const;

    /// @brief Type of traverse/visit handler using full attribute stack.
    using stack_visit_handler = callable_ref<bool(
      const compound&,
      const attribute&,
      const basic_string_path&,
      memory::span<const attribute>)>;

    /// @brief Traverses the tree, calls the @p visitor function on each node.
    void traverse(const stack_visit_handler visitor) const;

    template <std::derived_from<compound_interface> Implementation>
    [[nodiscard]] auto as() noexcept -> optional_reference<Implementation> {
        return dynamic_cast<Implementation*>(_pimpl.get());
    }

private:
    compound(std::shared_ptr<compound_interface> pimpl) noexcept
      : _pimpl{std::move(pimpl)} {}

    std::shared_ptr<compound_interface> _pimpl{};
};
//------------------------------------------------------------------------------
/// @brief Combination of a reference to a tree compound and a single attribute.
/// @ingroup valtree
///
/// This class provides a more convenient value tree access interface, at the
/// cost of storage space.
export class compound_attribute {
public:
    /// @brief Default constructor. Constructs empty attribute refering to nothing.
    compound_attribute() noexcept = default;

    /// @brief Construction from a compound and attribute pair.
    /// @pre c.type_id() == a.type_id()
    [[nodiscard]] compound_attribute(compound c, attribute a) noexcept
      : _c{std::move(c)}
      , _a{std::move(a)} {
        assert(not _c or not _a or (_c.type_id() == _a.type_id()));
    }

    /// @brief Indicates if this attribute actually refers to something.
    [[nodiscard]] explicit operator bool() const noexcept {
        return _c and _a;
    }

    /// @brief Returns the shared implementation type id of the attribute and compound.
    [[nodiscard]] auto type_id() const noexcept {
        return _c.type_id();
    }

    /// @brief Returns the name of this attribute.
    [[nodiscard]] auto name() const noexcept -> string_view {
        return _c.attribute_name(_a);
    }

    /// @brief Indicates if the specified attribute is immutable.
    [[nodiscard]] auto is_immutable() const noexcept -> bool {
        return _c.is_immutable(_a);
    }

    /// @brief Indicates if the specified attribute is a reference or link in the tree.
    [[nodiscard]] auto is_link() const noexcept -> bool {
        return _c.is_link(_a);
    }

    /// @brief Returns the canonical value type of this attribute.
    [[nodiscard]] auto canonical_type() const -> value_type {
        return _c.canonical_type(_a);
    }

    /// @brief Returns the count of nested attributes of an attribute.
    /// @note Some implementations may return zero here even if there are
    /// nested attributes. In such implementations the nested nodes can be
    /// traversed only by name.
    [[nodiscard]] auto nested_count() const -> span_size_t {
        return _c.nested_count(_a);
    }

    /// @brief Indicates if an attribute has nested attribute accessible by index.
    [[nodiscard]] auto has_nested() const -> span_size_t {
        return _c.has_nested(_a);
    }

    /// @brief Returns nested attribute of an attribute at the specified index.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto nested(const span_size_t index) const
      -> compound_attribute {
        return {_c, _c.nested(_a, index)};
    }

    /// @brief Returns nested attribute of an attribute with the specified name.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto nested(const string_view name) const
      -> compound_attribute {
        return {_c, _c.nested(_a, name)};
    }

    /// @brief Returns nested attribute of an attribute at the specified path.
    ///
    /// Returns empty attribute handle if no such nested attribute exists.
    [[nodiscard]] auto find(const basic_string_path& path) const
      -> compound_attribute {
        return {_c, _c.find(_a, path)};
    }

    /// @brief Returns the number of value elements accessible through an attribute.
    [[nodiscard]] auto value_count() const -> span_size_t {
        return _c.value_count(_a);
    }

    /// @brief Fetches values from this attribute, starting at offset, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(
      const span_size_t offset,
      memory::span<T> dest) const {
        return _c.fetch_values(_a, offset, dest);
    }

    /// @brief Fetches values from this attribute, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_values(memory::span<T> dest) const {
        return _c.fetch_values(_a, dest);
    }

    /// @brief Fetches a BLOB from this attribute, into dest.
    [[nodiscard]] auto fetch_blob(memory::block dest) const {
        return _c.fetch_blob(_a, dest);
    }

    /// @brief Fetches a value from this attribute, starting at offset, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto fetch_value(
      const span_size_t offset,
      T& dest,
      const selector<V> sel = default_selector) const -> bool {
        return _c.select_value(_a, offset, dest, sel);
    }

    /// @brief Fetches a value from this attribute, with selector.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_value(T& dest, const selector<V> sel) const
      -> bool {
        return _c.select_value(_a, dest, sel);
    }

    /// @brief Fetches a value from this attribute, with selector, into dest.
    template <typename T, identifier_t V>
    [[nodiscard]] auto select_values(
      memory::span<T> dest,
      const selector<V> sel) const -> memory::span<T> {
        return _c.select_values(_a, dest, sel);
    }

    /// @brief Fetches a value from this attribute, into dest.
    template <typename T>
    [[nodiscard]] auto fetch_value(T& dest) const -> bool {
        return _c.select_value(_a, dest, default_selector);
    }

    /// @brief Returns a value of type T, from this attribute, at offset.
    template <typename T>
    [[nodiscard]] auto get(
      const span_size_t offset,
      const std::type_identity<T> tid = {}) const {
        return _c.get(_a, offset, tid);
    }

    /// @brief Returns a value of type T, from this attribute.
    template <typename T>
    [[nodiscard]] auto get(const std::type_identity<T> tid = {}) const {
        return _c.get(_a, tid);
    }

private:
    compound _c;
    attribute _a;
};
//------------------------------------------------------------------------------
auto compound::root() const -> compound_attribute {
    return {*this, structure()};
}
//------------------------------------------------------------------------------
/// @brief Operator for creating a compound_attribute from compound and attribute.
/// @ingroup valtree
export [[nodiscard]] auto operator/(compound c, attribute a) noexcept
  -> compound_attribute {
    return {std::move(c), std::move(a)};
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
