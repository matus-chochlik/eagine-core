/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.value_tree:interface;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.console;
import <chrono>;
import <cstdint>;
import <memory>;
import <string>;
import <type_traits>;

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Value tree value element data type enumeration.
/// @ingroup valtree
export enum class value_type {
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
    virtual auto structure() -> attribute_interface* = 0;

    /// @brief Returns the name of the specified attribute.
    /// @pre this->type_id() == attr.type_id()
    virtual auto attribute_name(attribute_interface& attr) -> string_view = 0;

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
      -> attribute_interface* = 0;

    /// @brief Returns the nested attribute of an attribute with specified @p name.
    /// @pre this->type_id() == attr.type_id()
    virtual auto nested(attribute_interface&, string_view name)
      -> attribute_interface* = 0;

    /// @brief Finds the nested attribute of an attribute at the given @p path.
    /// @pre this->type_id() == attr.type_id()
    virtual auto find(attribute_interface& attr, const basic_string_path& path)
      -> attribute_interface* {
        return find(attr, path, {});
    }

    /// @brief Finds the nested attribute of an attribute at @p path with @p tags.
    /// @pre this->type_id() == attr.type_id()
    virtual auto find(
      attribute_interface& attr,
      const basic_string_path&,
      span<const string_view> tags) -> attribute_interface* = 0;

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
    /// @brief Called when starting the traversal of the whole tree.
    /// @see finish
    virtual void begin() = 0;

    /// @brief Consume a consecutive sequence of nil values.
    virtual void consume(span<const nothing_t>) = 0;

    /// @brief Consume a consecutive sequence of boolean values.
    virtual void consume(span<const bool>) = 0;

    /// @brief Consume a consecutive sequence of integer values.
    virtual void consume(span<const std::int64_t>) = 0;

    /// @brief Consume a consecutive sequence of unsigned values.
    virtual void consume(span<const std::uint64_t>) = 0;

    /// @brief Consume a consecutive sequence of float values.
    virtual void consume(span<const float>) = 0;

    /// @brief Consume a consecutive sequence of double values.
    virtual void consume(span<const double>) = 0;

    /// @brief Consume a consecutive sequence of string values.
    virtual void consume(span<const string_view>) = 0;

    /// @brief Called when entering a nested structure.
    virtual void begin_struct() = 0;

    /// @brief Called when entering a structure attribute with the given name.
    virtual void begin_attribute(const string_view key) = 0;

    /// @brief Called when leaving a structure attribute with the given name.
    virtual void finish_attribute(const string_view key) = 0;

    /// @brief Called when leaving a nested structure.
    virtual void finish_struct() = 0;

    /// @brief Called when entering a nested list.
    virtual void begin_list() = 0;

    /// @brief Called when leaving a nested list.
    virtual void finish_list() = 0;

    /// @brief Called when current chunk of input is consumed.
    /// @see begin
    virtual void flush() = 0;

    /// @brief Called when the traversal of the whole tree finished.
    /// @see begin
    virtual void finish() = 0;

    /// @brief Called when the parsing and traversal failed.
    /// @see begin
    virtual void failed() = 0;
};
//------------------------------------------------------------------------------
/// @brief Make a value tree visitor printing the visited items to the console.
/// @ingroup valtree
export auto make_printing_value_tree_visitor(const console&)
  -> std::unique_ptr<value_tree_visitor>;
//------------------------------------------------------------------------------
} // namespace eagine::valtree
