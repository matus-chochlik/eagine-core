/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.value_tree:interface;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.valid_if;
import eagine.core.console;

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Value tree value element data type enumeration.
/// @ingroup valtree
export enum class value_type : std::uint8_t {
    /// @brief Unknown value type.
    unknown,
    /// @brief Boolean value type.
    bool_type,
    /// @brief Byte/BLOB value type.
    byte_type,
    /// @brief 16-bit integer value type.
    int16_type,
    /// @brief 32-bit integer value type.
    int32_type,
    /// @brief 64-bit integer value type.
    int64_type,
    /// @brief floating-point number value type.
    float_type,
    /// @brief time duration value type.
    duration_type,
    /// @brief string value type.
    string_type,
    /// @brief composite structure type.
    composite
};
//------------------------------------------------------------------------------
export template <typename Selector>
constexpr auto enumerator_mapping(
  std::type_identity<value_type>,
  Selector) noexcept {
    return enumerator_map_type<value_type, 10>{
      {{"unknown", value_type::unknown},
       {"bool_type", value_type::bool_type},
       {"byte_type", value_type::byte_type},
       {"int16_type", value_type::int16_type},
       {"int32_type", value_type::int32_type},
       {"int64_type", value_type::int64_type},
       {"float_type", value_type::float_type},
       {"duration_type", value_type::duration_type},
       {"string_type", value_type::string_type},
       {"composite", value_type::composite}}};
}
//------------------------------------------------------------------------------
export struct compound_interface;

/// @brief Interface for value tree attribute implementations.
/// @ingroup valtree
/// @note Do not use directly in client code, use attribute instead.
/// @see attribute
/// @see compound_interface
export struct attribute_interface : interface<attribute_interface> {

    /// @brief Returns the implementation type identifier of this attribute.
    virtual auto type_id() const noexcept -> identifier = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for value tree compound implementations.
/// @ingroup valtree
/// @note Do not use directly in client code, use compound instead.
/// @see compound
/// @see attribute_interface
export struct compound_interface : interface<compound_interface> {

    /// @brief Returns the implementation type identifier of this compound.
    virtual auto type_id() const noexcept -> identifier = 0;

    /// @brief Add usage reference to the specified attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual void add_ref(attribute_interface& attr) noexcept = 0;

    /// @brief Release usage reference to the specified attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual void release(attribute_interface& attr) noexcept = 0;

    /// @brief Returns the root of attribute hierarch, describing the tree structure.
    virtual auto structure() -> optional_reference<attribute_interface> = 0;

    /// @brief Returns the name of the specified attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual auto attribute_name(attribute_interface& attr) -> string_view = 0;

    /// @brief Returns the value of the specified attribute as string.
    /// @pre this->type_id() == attr.type_id()
    virtual auto attribute_preview(attribute_interface& attr)
      -> optionally_valid<string_view> = 0;

    /// @brief Returns the value type of the specified attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual auto canonical_type(attribute_interface& attr) -> value_type = 0;

    /// @brief Indicates if the attribute value and structure is immutable.
    /// @pre this->type_id() == attr.type_id()
    virtual auto is_immutable(attribute_interface& attr) -> bool = 0;

    /// @brief Indicates if the specified attribute is a reference or link in the tree.
    /// @pre this->type_id() == attr.type_id()
    virtual auto is_link(attribute_interface& attr) -> bool = 0;

    /// @brief Returns the count of nested attributes in an attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual auto nested_count(attribute_interface& attr) -> span_size_t = 0;

    /// @brief Returns the nested attribute of an attribute at given @p index.
    /// @pre this->type_id() == attr.type_id()
    virtual auto nested(attribute_interface& attr, span_size_t index)
      -> optional_reference<attribute_interface> = 0;

    /// @brief Returns the nested attribute of an attribute with specified @p name.
    /// @pre this->type_id() == attr.type_id()
    virtual auto nested(attribute_interface&, string_view name)
      -> optional_reference<attribute_interface> = 0;

    /// @brief Finds the nested attribute of an attribute at the given @p path.
    /// @pre this->type_id() == attr.type_id()
    virtual auto find(attribute_interface& attr, const basic_string_path& path)
      -> optional_reference<attribute_interface> {
        return find(attr, path, {});
    }

    /// @brief Finds the nested attribute of an attribute at @p path with @p tags.
    /// @pre this->type_id() == attr.type_id()
    virtual auto find(
      attribute_interface& attr,
      const basic_string_path&,
      span<const string_view> tags)
      -> optional_reference<attribute_interface> = 0;

    /// @brief Returns the count of individual values stored in an attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual auto value_count(attribute_interface& attr) -> span_size_t = 0;

    /// @brief Fetches boolean values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<bool> dest) -> span_size_t = 0;

    /// @brief Fetches boolean values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<tribool> dest) -> span_size_t = 0;

    /// @brief Fetches char values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<char> dest) -> span_size_t = 0;

    /// @brief Fetches byte values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<byte> dest) -> span_size_t = 0;

    /// @brief Fetches 16-bit int values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::int16_t> dest) -> span_size_t = 0;

    /// @brief Fetches 32-bit int values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::int32_t> dest) -> span_size_t = 0;

    /// @brief Fetches 64-bit int values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::int64_t> dest) -> span_size_t = 0;

    /// @brief Fetches 16-bit uint values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::uint16_t> dest) -> span_size_t = 0;

    /// @brief Fetches 32-bit uint values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::uint32_t> dest) -> span_size_t = 0;

    /// @brief Fetches 64-bit uint values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::uint64_t> dest) -> span_size_t = 0;

    /// @brief Fetches float values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<float> dest) -> span_size_t = 0;

    /// @brief Fetches duration values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::chrono::duration<float>> dest) -> span_size_t = 0;

    /// @brief Fetches string values from attribute into dest, starting at offset.
    virtual auto fetch_values(
      attribute_interface&,
      span_size_t offset,
      span<std::string> dest) -> span_size_t = 0;
};
//------------------------------------------------------------------------------
struct value_tree_stream_parser : interface<value_tree_stream_parser> {
    virtual auto begin() noexcept -> bool = 0;
    virtual auto parse_data(memory::const_block data) noexcept -> bool = 0;
    virtual auto finish() noexcept -> bool = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for value tree structured traversal visitors.
/// @ingroup valtree
export struct value_tree_visitor : interface<value_tree_visitor> {
    /// @brief Called by the driver to check if the traversal should continue.
    virtual auto should_continue() noexcept -> bool = 0;

    /// @brief Called when starting the traversal of the whole tree.
    /// @see finish
    virtual void begin() noexcept = 0;

    /// @brief Consume a consecutive sequence of nil values.
    virtual void consume(span<const nothing_t>) noexcept = 0;

    /// @brief Consume a consecutive sequence of boolean values.
    virtual void consume(span<const bool>) noexcept = 0;

    /// @brief Consume a consecutive sequence of integer values.
    virtual void consume(span<const std::int64_t>) noexcept = 0;

    /// @brief Consume a consecutive sequence of unsigned values.
    virtual void consume(span<const std::uint64_t>) noexcept = 0;

    /// @brief Consume a consecutive sequence of float values.
    virtual void consume(span<const float>) noexcept = 0;

    /// @brief Consume a consecutive sequence of double values.
    virtual void consume(span<const double>) noexcept = 0;

    /// @brief Consume a consecutive sequence of string values.
    virtual void consume(span<const string_view>) noexcept = 0;

    /// @brief Called when entering a nested structure.
    virtual void begin_struct() noexcept = 0;

    /// @brief Called when entering a structure attribute with the given name.
    virtual void begin_attribute(const string_view key) noexcept = 0;

    /// @brief Called when leaving a structure attribute with the given name.
    virtual void finish_attribute(const string_view key) noexcept = 0;

    /// @brief Called when leaving a nested structure.
    virtual void finish_struct() noexcept = 0;

    /// @brief Called when entering a nested list.
    virtual void begin_list() noexcept = 0;

    /// @brief Called when leaving a nested list.
    virtual void finish_list() noexcept = 0;

    /// @brief Called when current chunk of input is consumed.
    /// @see begin
    virtual void flush() noexcept = 0;

    /// @brief Called when unparsed data is consumed by the input object.
    virtual void unparsed_data(span<const memory::const_block>) noexcept = 0;

    /// @brief Called when the traversal of the whole tree finished.
    /// @see begin
    virtual auto finish() noexcept -> bool = 0;

    /// @brief Called when the parsing and traversal failed.
    /// @see begin
    virtual void failed() noexcept = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for classes that initialize or change values in an object.
/// @ingroup valtree
/// @see value_tree_visitor
/// @see make_building_value_tree_visitor
///
/// Implementation of this interface are typically plugged into a value tree
/// visitor that handles the traversal of a value tree, keeps track of the
/// current position in the tree structure and the associated path and
/// calls the functions from the object builder's interface. Such
/// implementations can be used to initialize or modify values in an structured
/// object from values in a value tree.
export struct object_builder : interface<object_builder> {

    /// @brief Called by the driver to check if the traversal should continue.
    virtual auto should_continue() noexcept -> bool {
        return true;
    }

    /// @brief Returns the maximum token size length needed by this builder.
    virtual auto max_token_size() noexcept -> span_size_t = 0;

    /// @brief Called when the tree traversal begins.
    /// @see finish
    virtual void begin() noexcept {}

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      const span<const nothing_t> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const bool> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const std::int64_t> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const std::uint64_t> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const float> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const double> data) noexcept = 0;

    /// @brief Called when values at the specified path in the tree are consumed.
    virtual void add(
      const basic_string_path& path,
      span<const string_view> data) noexcept = 0;

    /// @brief Called when a new structured attribute or array element should be added.
    /// @see finish_object
    virtual void add_object(const basic_string_path&) noexcept {}

    /// @brief Called when a structured attribute or array element is finalized.
    /// @see add_object
    virtual void finish_object(const basic_string_path&) noexcept {}

    /// @brief Called when unparsed data is consumed by the input
    virtual void unparsed_data(span<const memory::const_block>) noexcept {}

    /// @brief Called when the tree traversal finished.
    /// @see begin
    /// @see failed
    virtual auto finish() noexcept -> bool {
        return true;
    }

    /// @brief Called when the tree traversal failed.
    /// @see finish
    virtual void failed() noexcept = 0;
};
//------------------------------------------------------------------------------
/// @brief Make a value tree visitor that combines two other visitors.
/// @ingroup valtree
export auto make_combined_value_tree_visitor(
  std::shared_ptr<value_tree_visitor> left,
  std::shared_ptr<value_tree_visitor> right)
  -> std::unique_ptr<value_tree_visitor>;
//------------------------------------------------------------------------------
/// @brief Make a value tree visitor printing the visited items to the console.
/// @ingroup valtree
export auto make_printing_value_tree_visitor(const console&)
  -> std::unique_ptr<value_tree_visitor>;
//------------------------------------------------------------------------------
/// @brief Make a value tree visitor driving an object builder.
/// @ingroup valtree
export auto make_building_value_tree_visitor(
  std::shared_ptr<object_builder> builder)
  -> std::unique_ptr<value_tree_visitor>;
//------------------------------------------------------------------------------
} // namespace eagine::valtree
