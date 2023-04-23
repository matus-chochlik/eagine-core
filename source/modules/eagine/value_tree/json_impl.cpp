/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>

module eagine.core.value_tree;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.logging;

namespace eagine::valtree {
//------------------------------------------------------------------------------
static inline auto view(const rapidjson::GenericStringRef<char>& str) {
    return string_view{str.s, span_size(str.length)};
}
//------------------------------------------------------------------------------
template <typename E, typename A>
static inline auto view(rapidjson::GenericValue<E, A>& val) -> string_view {
    if(val.IsString()) {
        return {val.GetString(), span_size(val.GetStringLength())};
    }
    return {};
}
//------------------------------------------------------------------------------
static inline auto rapidjson_size(const span_size_t s) {
    return limit_cast<rapidjson::SizeType>(s);
}
//------------------------------------------------------------------------------
template <typename Encoding, typename Allocator, typename StackAlloc>
class rapidjson_value_node;

template <typename Encoding, typename Allocator, typename StackAlloc>
class rapidjson_document_compound;
//------------------------------------------------------------------------------
template <typename Encoding, typename Allocator, typename StackAlloc>
static auto rapidjson_make_value_node(
  rapidjson_document_compound<Encoding, Allocator, StackAlloc>& owner,
  rapidjson::GenericValue<Encoding, Allocator>& value,
  rapidjson::GenericValue<Encoding, Allocator>* name = nullptr)
  -> optional_reference<attribute_interface>;
//------------------------------------------------------------------------------
template <typename Encoding, typename Allocator, typename StackAlloc>
class rapidjson_value_node : public attribute_interface {
private:
    using _doc_t = rapidjson::GenericDocument<Encoding, Allocator, StackAlloc>;
    using _val_t = rapidjson::GenericValue<Encoding, Allocator>;
    using _node_t = rapidjson_value_node<Encoding, Allocator, StackAlloc>;
    using _comp_t =
      rapidjson_document_compound<Encoding, Allocator, StackAlloc>;

    _val_t* _rj_val{nullptr};
    _val_t* _rj_name{nullptr};

public:
    [[nodiscard]] rapidjson_value_node(
      _val_t& rj_val,
      _val_t* rj_name = nullptr) noexcept
      : _rj_val{&rj_val}
      , _rj_name{rj_name} {}

    constexpr auto operator==(const rapidjson_value_node& that) const noexcept {
        return _rj_val == that._rj_val;
    }

    auto type_id() const noexcept -> identifier final {
        return "rapidjson";
    }

    auto name() -> string_view {
        if(_rj_name) {
            const auto& name = extract(_rj_name);
            return {name.GetString(), span_size(name.GetStringLength())};
        }
        return {};
    }

    auto canonical_type() const -> value_type {
        if(_rj_val) {
            auto get_type = [&](const auto& val) {
                if(val.IsBool()) {
                    return value_type::bool_type;
                }
                if(val.IsInt()) {
                    return value_type::int32_type;
                }
                if(val.IsInt64()) {
                    return value_type::int64_type;
                }
                if(val.IsFloat() or val.IsDouble()) {
                    return value_type::float_type;
                }
                if(val.IsString()) {
                    return value_type::string_type;
                }
                return value_type::unknown;
            };

            const auto& val = extract(_rj_val);
            if(val.IsObject()) {
                return value_type::composite;
            }
            if(val.IsArray()) {
                const auto n = span_size(val.Size());
                if(n > 0) {
                    const auto common_type = get_type(val[0]);
                    for(const auto i : integer_range(n)) {
                        if(get_type(val[rapidjson_size(i)]) != common_type) {
                            return value_type::composite;
                        }
                    }
                    return common_type;
                }
                return value_type::composite;
            }
            return get_type(val);
        }
        return value_type::unknown;
    }

    auto nested_count() -> span_size_t {
        if(_rj_val) {
            const auto& val = extract(_rj_val);
            if(val.IsArray()) {
                return span_size(val.Size());
            }
            if(val.IsObject()) {
                return span_size(val.MemberCount());
            }
        }
        return 0;
    }

    auto nested(_comp_t& owner, const span_size_t index)
      -> optional_reference<attribute_interface> {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            if(val.IsArray()) {
                if(index < span_size(val.Size())) {
                    return rapidjson_make_value_node(
                      owner, val[rapidjson_size(index)]);
                }
            }
            if(val.IsObject()) {
                if(index < span_size(val.MemberCount())) {
                    auto pos = val.MemberBegin() + index;
                    return rapidjson_make_value_node(
                      owner, pos->value, &pos->name);
                }
            }
        }
        return {};
    }

    auto nested(_comp_t& owner, const string_view name)
      -> optional_reference<attribute_interface> {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            if(val.IsObject()) {
                auto found = val.FindMember(c_str(name));
                if(found != val.MemberEnd()) {
                    return rapidjson_make_value_node(
                      owner, found->value, &found->name);
                }
            }
            if(const auto opt_idx{from_string<span_size_t>(name)}) {
                return nested(owner, extract(opt_idx));
            }
        }
        return {};
    }

    auto find(
      _comp_t& owner,
      const basic_string_path& path,
      const span<const string_view> tags)
      -> optional_reference<attribute_interface> {
        _val_t* result = _rj_val;
        _val_t* name = nullptr;
        std::string temp_str;
        auto _cat =
          [&](string_view a, string_view b, string_view c) mutable -> auto& {
            return append_to(c, append_to(b, assign_to(a, temp_str)));
        };

        for(auto& entry : path) {
            if(result) {
                if(result->IsObject()) {
                    bool found = false;
                    auto member = result->MemberEnd();
                    for(auto tag : tags) {
                        member =
                          result->FindMember(_cat(entry, "@", tag).c_str());
                        if(member != result->MemberEnd()) {
                            result = &member->value;
                            name = &member->name;
                            found = true;
                            break;
                        }
                    }
                    if(not found) {
                        if(member == result->MemberEnd()) {
                            member = result->FindMember(c_str(entry));
                            if(member != result->MemberEnd()) {
                                result = &member->value;
                                name = &member->name;
                                found = true;
                            }
                        }
                    }
                    if(not found) {
                        result = nullptr;
                    }
                } else if(result->IsArray()) {
                    if(const auto opt_idx{from_string<span_size_t>(entry)}) {
                        const auto index{extract(opt_idx)};
                        if(index < span_size(result->Size())) {
                            result = &(*result)[rapidjson_size(index)];
                        } else {
                            result = nullptr;
                        }
                    } else {
                        result = nullptr;
                    }
                } else {
                    result = nullptr;
                }
            } else {
                break;
            }
        }

        if(result) {
            return rapidjson_make_value_node(owner, *result, name);
        }

        return {};
    }

    auto value_count() -> span_size_t {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            if(val.IsArray()) {
                return span_size(val.Size());
            }
            if(not val.IsNull()) {
                return 1;
            }
        }
        return 0;
    }

    auto convert(_val_t& val, std::string& dest) -> bool {
        if(val.IsString()) {
            dest.assign(val.GetString(), integer(val.GetStringLength()));
            return true;
        }
        return false;
    }

    template <typename T>
    auto convert_bool(_val_t& val, T& dest) -> bool {
        if(val.IsBool()) {
            if(assign_if_fits(val.GetBool(), dest)) {
                return true;
            }
        }
        if(val.IsInt()) {
            if(assign_if_fits(val.GetInt(), dest)) {
                return true;
            }
        }
        if(val.IsString()) {
            if(const auto converted{from_string<T>(view(val))}) {
                dest = extract(converted);
                return true;
            }
        }
        return false;
    }

    template <typename T>
    auto convert_tribool(_val_t& val, T& dest) -> bool {
        bool temp{};
        if(convert_bool(val, temp)) {
            dest = {temp, false};
            return true;
        }
        if(val.IsNull()) {
            dest = indeterminate;
            return true;
        }
        return false;
    }

    template <typename T>
    auto convert_int(_val_t& val, T& dest) -> bool {
        if(val.IsInt()) {
            if(assign_if_fits(val.GetInt(), dest)) {
                return true;
            }
        }
        if(val.IsInt64()) {
            if(assign_if_fits(val.GetInt64(), dest)) {
                return true;
            }
        }
        if(val.IsString()) {
            if(const auto converted{from_string<T>(view(val))}) {
                dest = extract(converted);
                return true;
            }
        }
        return false;
    }

    template <typename T>
    auto convert_uint(_val_t& val, T& dest) -> bool {
        if(val.IsUint()) {
            if(assign_if_fits(val.GetUint(), dest)) {
                return true;
            }
        }
        if(val.IsUint64()) {
            if(assign_if_fits(val.GetUint64(), dest)) {
                return true;
            }
        }
        if(val.IsString()) {
            if(const auto converted{from_string<T>(view(val))}) {
                dest = extract(converted);
                return true;
            }
        }
        return false;
    }

    template <typename T>
    auto convert_real(_val_t& val, T& dest) -> bool {
        if(val.IsFloat()) {
            if(assign_if_fits(val.GetFloat(), dest)) {
                return true;
            }
        }
        if(val.IsDouble()) {
            if(assign_if_fits(val.GetDouble(), dest)) {
                return true;
            }
        }
        if(val.IsInt()) {
            if(assign_if_fits(val.GetInt(), dest)) {
                return true;
            }
        }
        if(val.IsInt64()) {
            if(assign_if_fits(val.GetInt64(), dest)) {
                return true;
            }
        }
        if(val.IsString()) {
            if(const auto converted{from_string<T>(view(val))}) {
                dest = extract(converted);
                return true;
            }
        }
        return false;
    }

    template <typename R, typename P>
    auto convert_duration(_val_t& val, std::chrono::duration<R, P>& dest)
      -> bool {
        using _dur_t = std::chrono::duration<R, P>;
        if(val.IsFloat()) {
            if(assign_if_fits(val.GetFloat(), dest)) {
                return true;
            }
        }
        if(val.IsDouble()) {
            if(assign_if_fits(val.GetDouble(), dest)) {
                return true;
            }
        }
        if(val.IsInt()) {
            if(assign_if_fits(val.GetInt(), dest)) {
                return true;
            }
        }
        if(val.IsInt64()) {
            if(assign_if_fits(val.GetInt64(), dest)) {
                return true;
            }
        }
        if(val.IsString()) {
            if(const auto converted{from_string<_dur_t>(view(val))}) {
                dest = extract(converted);
                return true;
            }
        }
        return false;
    }

    auto convert(_val_t& val, bool& dest) {
        return convert_bool(val, dest);
    }

    auto convert(_val_t& val, tribool& dest) {
        return convert_tribool(val, dest);
    }

    auto convert(_val_t& val, byte& dest) {
        return convert_uint(val, dest);
    }

    auto convert(_val_t& val, std::int16_t& dest) {
        return convert_int(val, dest);
    }

    auto convert(_val_t& val, std::int32_t& dest) {
        return convert_int(val, dest);
    }

    auto convert(_val_t& val, std::int64_t& dest) {
        return convert_int(val, dest);
    }

    auto convert(_val_t& val, std::uint16_t& dest) {
        return convert_uint(val, dest);
    }

    auto convert(_val_t& val, std::uint32_t& dest) {
        return convert_uint(val, dest);
    }

    auto convert(_val_t& val, std::uint64_t& dest) {
        return convert_uint(val, dest);
    }

    auto convert(_val_t& val, float& dest) {
        return convert_real(val, dest);
    }

    auto convert(_val_t& val, std::chrono::duration<float>& dest) {
        return convert_duration(val, dest);
    }

    template <typename T>
    auto do_fetch_values(const span_size_t offset, span<T> dest)
      -> span_size_t {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            if(val.IsArray()) {
                const auto n = span_size(val.Size());
                span_size_t i = 0;
                while((i + offset < n) and (i < dest.size())) {
                    if(not convert(val[rapidjson_size(i + offset)], dest[i])) {
                        break;
                    }
                    ++i;
                }
                return i;
            } else {
                if(not dest.empty()) {
                    if(convert(val, dest.front())) {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    template <typename T>
    auto fetch_values(const span_size_t offset, span<T> dest) -> span_size_t {
        return do_fetch_values(offset, dest);
    }

    auto fetch_values(const span_size_t offset, span<char> dest)
      -> span_size_t {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            if(val.IsString()) {
                auto src{head(skip(view(val), offset), dest)};
                copy(src, dest);
                return src.size();
            }
        }
        return 0;
    }

    auto fetch_values(const span_size_t offset, span<byte> dest)
      -> span_size_t {
        if(_rj_val) {
            auto& val = extract(_rj_val);
            // blobs can also be decoded from base64 strings
            using memory::skip;
            if(val.IsString()) {
                const auto req_size =
                  base64_decoded_length(span_size(val.GetStringLength()));
                if(dest.size() < req_size) {
                    std::vector<byte> temp{};
                    if(const auto dec{base64_decode(view(val), temp)}) {
                        if(const auto src{
                             head(skip(cover(extract(dec)), offset), dest)}) {
                            copy(src, dest);
                            return src.size();
                        }
                    }
                    return 0;
                } else {
                    if(const auto src{head(
                         skip(base64_decode(view(val), dest), offset), dest)}) {
                        copy(src, dest);
                        return src.size();
                    }
                    return 0;
                }
            }
            return do_fetch_values(offset, dest);
        }
        return 0;
    }
};
//------------------------------------------------------------------------------
class rapidjson_chunk_stream {
public:
    rapidjson_chunk_stream(span<const memory::const_block> data) noexcept
      : _chunks{data} {}

    using Ch = rapidjson::UTF8<>::Ch;

    auto Peek() const noexcept -> Ch {
        if(_chunk < _chunks.size()) {
            return static_cast<Ch>(_chunks[_chunk][_index]);
        }
        return '\0';
    }

    auto Take() noexcept -> Ch {
        const Ch result{Peek()};
        if(_chunk < _chunks.size()) {
            if(++_index >= _chunks[_chunk].size()) {
                ++_chunk;
                _index = 0;
            }
        }
        return result;
    }

    auto Tell() const noexcept -> std::size_t {
        return _offset;
    }

    auto PutBegin() noexcept -> Ch* {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    void Put(Ch) noexcept {
        RAPIDJSON_ASSERT(false);
    }
    void Flush() noexcept {
        RAPIDJSON_ASSERT(false);
    }
    auto PutEnd(Ch*) noexcept -> std::size_t {
        RAPIDJSON_ASSERT(false);
        return 0;
    }

private:
    span<const memory::const_block> _chunks;
    std::size_t _offset{0U};
    span_size_t _chunk{0};
    span_size_t _index{0};
};
//------------------------------------------------------------------------------
template <typename Encoding, typename Allocator, typename StackAlloc>
class rapidjson_document_compound
  : public compound_with_refcounted_node<
      rapidjson_document_compound<Encoding, Allocator, StackAlloc>,
      rapidjson_value_node<Encoding, Allocator, StackAlloc>> {
private:
    using base = compound_with_refcounted_node<
      rapidjson_document_compound<Encoding, Allocator, StackAlloc>,
      rapidjson_value_node<Encoding, Allocator, StackAlloc>>;
    using base::_unwrap;

    using _doc_t = rapidjson::GenericDocument<Encoding, Allocator, StackAlloc>;
    using _val_t = rapidjson::GenericValue<Encoding, Allocator>;
    using _node_t = rapidjson_value_node<Encoding, Allocator, StackAlloc>;

    logger _log;
    _doc_t _rj_doc{};
    _node_t _root{};

public:
    [[nodiscard]] rapidjson_document_compound(
      _doc_t& rj_doc,
      const logger& parent)
      : _log{"JsnValTree", parent}
      , _rj_doc{std::move(rj_doc)}
      , _root{_rj_doc, nullptr} {}

    static auto do_make_shared(
      _doc_t& rj_doc,
      const rapidjson::ParseResult parse_ok,
      const logger& parent) -> std::shared_ptr<rapidjson_document_compound> {
        if(parse_ok) {
            return std::make_shared<rapidjson_document_compound>(
              rj_doc, parent);
        }
        const auto msg{rapidjson::GetParseError_En(parse_ok.Code())};
        parent.error("JSON parse error")
          .arg("message", msg)
          .arg("offset", parse_ok.Offset());
        return {};
    }

    [[nodiscard]] static auto make_shared(
      string_view json_str,
      const logger& parent) -> std::shared_ptr<rapidjson_document_compound> {
        _doc_t rj_doc;
        return do_make_shared(
          rj_doc,
          rj_doc.Parse(json_str.data(), rapidjson::SizeType(json_str.size())),
          parent);
    }

    [[nodiscard]] static auto make_shared(
      span<const memory::const_block> json_data,
      const logger& parent) -> std::shared_ptr<rapidjson_document_compound> {
        rapidjson_chunk_stream json_stream{json_data};
        _doc_t rj_doc;

        return do_make_shared(rj_doc, rj_doc.ParseStream(json_stream), parent);
    }

    auto type_id() const noexcept -> identifier final {
        return "rapidjson";
    }

    auto structure() -> optional_reference<attribute_interface> final {
        return &_root;
    }

    auto attribute_name(attribute_interface& attrib) -> string_view final {
        return _unwrap(attrib).name();
    }

    auto canonical_type(attribute_interface& attrib) -> value_type final {
        return _unwrap(attrib).canonical_type();
    }

    auto is_immutable(attribute_interface&) -> bool final {
        return true;
    }

    auto is_link(attribute_interface&) -> bool final {
        return false;
    }

    auto nested_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).nested_count();
    }

    auto nested(attribute_interface& attrib, span_size_t index)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, index);
    }

    auto nested(attribute_interface& attrib, string_view name)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, name);
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
      const span_size_t offset,
      span<T> dest) -> span_size_t {
        return _unwrap(attrib).fetch_values(offset, dest);
    }
};
//------------------------------------------------------------------------------
template <typename Encoding, typename Allocator, typename StackAlloc>
[[nodiscard]] static inline auto rapidjson_make_value_node(
  rapidjson_document_compound<Encoding, Allocator, StackAlloc>& owner,
  rapidjson::GenericValue<Encoding, Allocator>& value,
  rapidjson::GenericValue<Encoding, Allocator>* name)
  -> optional_reference<attribute_interface> {
    return owner.make_node(value, name);
}
//------------------------------------------------------------------------------
template <typename Document>
struct get_rapidjson_document_compound;

template <typename Document>
using get_rapidjson_document_compound_t =
  typename get_rapidjson_document_compound<Document>::type;

template <typename Encoding, typename Allocator, typename StackAlloc>
struct get_rapidjson_document_compound<
  rapidjson::GenericDocument<Encoding, Allocator, StackAlloc>> {
    using type = rapidjson_document_compound<Encoding, Allocator, StackAlloc>;
};

using default_rapidjson_document_compound =
  get_rapidjson_document_compound_t<rapidjson::Document>;
//------------------------------------------------------------------------------
[[nodiscard]] auto from_json_text(string_view json_text, const logger& parent)
  -> compound {
    return compound::make<default_rapidjson_document_compound>(
      json_text, parent);
}
//------------------------------------------------------------------------------
[[nodiscard]] auto from_json_data(
  span<const memory::const_block> json_data,
  const logger& parent) -> compound {
    return compound::make<default_rapidjson_document_compound>(
      json_data, parent);
}
//------------------------------------------------------------------------------
// streaming
//------------------------------------------------------------------------------
class rapidjson_progressive_stream {
public:
    using Ch = rapidjson::UTF8<>::Ch;

    auto Peek() const noexcept -> Ch {
        span_size_t offs{_offs};
        if(offs < _previous.size()) {
            return static_cast<Ch>(memory::const_block{_previous}[offs]);
        }
        offs -= _previous.size();
        if(offs < _current.size()) {
            return static_cast<Ch>(_current[offs]);
        }
        return '\0';
    }

    auto Take() noexcept -> Ch {
        const Ch result{Peek()};
        ++_offs;
        return result;
    }

    auto Tell() const noexcept -> std::size_t {
        return std_size(_done + _offs);
    }

    auto PutBegin() noexcept -> Ch* {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    void Put(Ch) noexcept {
        RAPIDJSON_ASSERT(false);
    }
    void Flush() noexcept {
        RAPIDJSON_ASSERT(false);
    }
    auto PutEnd(Ch*) noexcept -> std::size_t {
        RAPIDJSON_ASSERT(false);
        return 0;
    }

    auto available() const noexcept -> span_size_t {
        return _previous.size() + _current.size() - _offs;
    }

    void begin(memory::buffer_pool& buffers, span_size_t max_token_size) {
        _previous = buffers.get(max_token_size);
        _previous.clear();
        _current = {};
        _done = 0;
        _offs = 0;
    }

    void next(memory::const_block blk) {
        assert(_current.empty());
        _current = blk;
    }

    void advance() {
        const auto sz{_previous.size()};
        if(_offs >= sz) {
            _done += sz;
            _offs -= sz;
            _previous.clear();
            _current = skip(_current, _offs);
            _done += _offs;
            _offs = 0;
        }
        if(not _current.empty()) {
            append_to(_current, _previous);
            _current = {};
        }
    }

    auto get_unused() noexcept -> span<const memory::const_block> {
        const auto szp{_previous.size()};
        const auto szc{_current.size()};
        if(_offs < szp) {
            _unused[0] = skip(view(_previous), _offs);
            _unused[1] = _current;
            _offs = szp + szc;
            return view(_unused);
        }

        if(_offs < (szp + szc)) {
            _unused[0] = skip(_current, _offs - szp);
            _offs = szp + szc;
            return view_one(_unused[0]);
        }

        return {};
    }

    void finish(memory::buffer_pool& buffers) noexcept {
        buffers.eat(std::move(_previous));
    }

private:
    memory::buffer _previous;
    memory::const_block _current;
    std::array<memory::const_block, 2> _unused;
    span_size_t _done{0};
    span_size_t _offs{0};
};
//------------------------------------------------------------------------------
// TODO: actual progressive parsing of the individual chunks
class json_value_tree_stream_parser
  : public value_tree_stream_parser
  , public rapidjson::
      BaseReaderHandler<rapidjson::UTF8<>, json_value_tree_stream_parser> {
public:
    [[nodiscard]] json_value_tree_stream_parser(
      std::shared_ptr<value_tree_visitor> visitor,
      span_size_t max_token_size,
      memory::buffer_pool& buffers,
      const logger& parent) noexcept
      : _parent{parent}
      , _buffers{buffers}
      , _max_token_size{max_token_size}
      , _visitor{std::move(visitor)} {
        assert(_visitor);
    }

    auto begin() noexcept -> bool {
        _json_stream.begin(_buffers, _max_token_size);
        _json_reader.IterativeParseInit();
        _visitor->begin();
        _just_flushed = false;
        _in_unparsed = false;
        _old_val_typ = _cached_type::none;
        return true;
    }

    constexpr static auto _parse_flags() noexcept {
        return rapidjson::kParseDefaultFlags | rapidjson::kParseIterativeFlag |
               rapidjson::kParseStopWhenDoneFlag |
               rapidjson::kParseTrailingCommasFlag;
    }

    auto _do_parse_current() noexcept -> bool {
        return _json_reader.IterativeParseNext<_parse_flags()>(
                 _json_stream, *this) and
               not _json_reader.IterativeParseComplete();
    }

    auto parse_data(memory::const_block data) noexcept -> bool {
        if(_visitor->should_continue()) [[likely]] {
            if(_in_unparsed) {
                if(not data.empty()) {
                    _visitor->unparsed_data(view_one(data));
                    return true;
                }
            } else {
                _json_stream.next(data);
                bool parsed_something{false};
                while(_json_stream.available() >= _max_token_size) {
                    if(not _do_parse_current()) {
                        break;
                    }
                    parsed_something = true;
                }

                if(_json_reader.IterativeParseComplete()) {
                    if(const auto unparsed{_json_stream.get_unused()}) {
                        _visitor->unparsed_data(unparsed);
                        _in_unparsed = true;
                    } else {
                        return false;
                    }
                }

                if(parsed_something and not _just_flushed) {
                    _visitor->flush();
                    _just_flushed = true;
                }
                _json_stream.advance();
                return true;
            }
        }
        return false;
    }

    auto finish() noexcept -> bool {
        bool result = true;
        if(_in_unparsed) {
            assert(_json_stream.available() == 0);
        } else {
            while(_visitor->should_continue()) {
                if(not _do_parse_current()) {
                    break;
                }
            }
            if(_json_reader.IterativeParseComplete()) {
                if(const auto unparsed{_json_stream.get_unused()}) {
                    _visitor->unparsed_data(unparsed);
                }
            }
        }
        if(_json_reader.HasParseError()) [[unlikely]] {
            if(
              _json_reader.GetParseErrorCode() ==
              rapidjson::kParseErrorDocumentRootNotSingular) {
                _visitor->flush();
                result &= _visitor->finish();
            } else {
                _visitor->failed();
                result = false;
            }
        } else {
            _visitor->flush();
            result &= _visitor->finish();
        }
        _json_stream.finish(_buffers);
        return result;
    }

    enum class _cached_type { none, _int, _uint, _float, other };

    template <typename T>
    void _do_flush_previous(
      std::vector<T>& cache,
      _cached_type cur_val_typ,
      _cached_type new_val_typ) noexcept {
        const auto should_flush{
          ((_old_val_typ != new_val_typ) and not cache.empty()) or
          ((_old_val_typ == cur_val_typ) and (cache.size() >= 512))};
        if(should_flush) [[unlikely]] {
            _visitor->consume(memory::view(cache));
            cache.clear();
            _just_flushed = false;
        }
    }

    void _flush_previous(_cached_type new_val_typ) {
        _do_flush_previous(_float_values, _cached_type::_float, new_val_typ);
        _do_flush_previous(_uint_values, _cached_type::_uint, new_val_typ);
        _do_flush_previous(_int_values, _cached_type::_int, new_val_typ);
        _old_val_typ = new_val_typ;
    }

    template <typename T>
    void _do_push(T value, std::vector<T>& cache, _cached_type new_val_typ) {
        if(_list_stack.empty() or not _list_stack.top()) {
            _visitor->consume(view_one(value));
            _just_flushed = false;
        } else {
            _flush_previous(new_val_typ);
            cache.push_back(value);
        }
    }

    // rapidjson handler API
    auto Null() -> bool {
        _flush_previous(_cached_type::other);
        _visitor->consume(view_one(nothing));
        _just_flushed = false;
        return true;
    }
    auto Bool(bool value) -> bool {
        _flush_previous(_cached_type::other);
        _visitor->consume(view_one(value));
        _just_flushed = false;
        return true;
    }
    auto Int(int value) -> bool {
        return Int64(value);
    }
    auto Uint(unsigned value) -> bool {
        return Uint64(value);
    }
    auto Int64(std::int64_t value) -> bool {
        _do_push(value, _int_values, _cached_type::_int);
        return true;
    }
    auto Uint64(std::uint64_t value) -> bool {
        _do_push(value, _uint_values, _cached_type::_uint);
        return true;
    }
    auto Double(double value) -> bool {
        _do_push(value, _float_values, _cached_type::_float);
        return true;
    }
    auto String(const char* str, rapidjson::SizeType length, bool copy)
      -> bool {
        _flush_previous(_cached_type::other);
        const string_view value{str, span_size(length)};
        _visitor->consume(view_one(value));
        _just_flushed = false;
        return true;
    }
    auto StartObject() -> bool {
        _attr_stack.push({});
        _list_stack.push(false);
        _visitor->begin_struct();
        return true;
    }
    auto Key(const char* str, rapidjson::SizeType length, bool) -> bool {
        assert(not _attr_stack.empty());
        auto& attr_name = _attr_stack.top();
        if(not attr_name.empty()) {
            _visitor->finish_attribute(attr_name);
        }
        attr_name.assign(str, std_size(length));
        _visitor->begin_attribute(attr_name);
        return true;
    }
    auto EndObject(rapidjson::SizeType) -> bool {
        assert(not _attr_stack.empty());
        auto& attr_name = _attr_stack.top();
        if(not attr_name.empty()) {
            _visitor->finish_attribute(attr_name);
        }
        _attr_stack.pop();
        _list_stack.pop();
        if(_list_stack.empty()) {
            _in_unparsed = true;
        }
        _visitor->finish_struct();
        return true;
    }
    auto StartArray() -> bool {
        _list_stack.push(true);
        _visitor->begin_list();
        return true;
    }
    auto EndArray(rapidjson::SizeType) -> bool {
        _flush_previous(_cached_type::none);
        _visitor->finish_list();
        _list_stack.pop();
        if(_list_stack.empty()) {
            _in_unparsed = true;
        }
        return true;
    }

private:
    const logger& _parent;
    memory::buffer_pool& _buffers;
    const span_size_t _max_token_size;
    rapidjson_progressive_stream _json_stream;
    rapidjson::Reader _json_reader;
    std::shared_ptr<value_tree_visitor> _visitor;
    memory::buffer _json_block;
    std::stack<std::string> _attr_stack;
    std::stack<bool> _list_stack;
    std::vector<double> _float_values;
    std::vector<std::int64_t> _int_values;
    std::vector<std::uint64_t> _uint_values;
    _cached_type _old_val_typ{_cached_type::none};
    bool _just_flushed{false};
    bool _in_unparsed{false};
};
//------------------------------------------------------------------------------
[[nodiscard]] auto traverse_json_stream(
  std::shared_ptr<value_tree_visitor> visitor,
  span_size_t max_token_size,
  memory::buffer_pool& buffers,
  const logger& parent) -> value_tree_stream_input {
    return {std::make_unique<json_value_tree_stream_parser>(
      std::move(visitor), max_token_size, buffers, parent)};
}
//------------------------------------------------------------------------------
[[nodiscard]] auto traverse_json_stream(
  std::shared_ptr<object_builder> builder,
  memory::buffer_pool& buffers,
  const logger& parent) -> value_tree_stream_input {
    const auto max_token_size{builder->max_token_size()};
    return traverse_json_stream(
      make_building_value_tree_visitor(std::move(builder)),
      max_token_size,
      buffers,
      parent);
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
