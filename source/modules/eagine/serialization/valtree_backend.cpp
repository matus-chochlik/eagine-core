/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:valtree_backend;

import eagine.core.concepts;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.value_tree;
import eagine.core.valid_if;
import :result;
import :interface;
import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Value-tree-based implementation of deserializer backend.
/// @ingroup serialization
export class valtree_deserializer_backend : public deserializer_backend {

public:
    using error_code = deserialization_error_code;
    using result = deserialization_errors;

    valtree_deserializer_backend(valtree::compound source) noexcept
      : _source{std::move(source)} {}

    valtree_deserializer_backend(
      valtree::compound source,
      valtree::attribute root) noexcept
      : _source{std::move(source)}
      , _root{std::move(root)} {}

    static constexpr const identifier_value id_value{"ValTree"};

    auto source() noexcept -> deserializer_data_source* final {
        return nullptr;
    }

    auto type_id() noexcept -> identifier final {
        return "ValTree";
    }

    auto enum_as_string() noexcept -> bool final {
        return true;
    }

    auto begin() noexcept -> result final {
        _errors.clear();
        if(_attribs.empty()) {
            if(_root) {
                if(_source.type_id() == _root.type_id()) {
                    _attribs.push(_root);
                } else {
                    _errors.set(error_code::data_source_error);
                }
            } else {
                if(not _source) [[unlikely]] {
                    _errors.set(error_code::data_source_error);
                }
                if(auto root{_source.structure()}) {
                    _attribs.emplace(std::move(root));
                } else {
                    _errors.set(error_code::missing_member);
                }
            }
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    template <typename T>
    auto do_read(span<T> values, span_size_t& done) noexcept -> result {
        if(not _attribs.empty()) {
            done = _source.fetch_values(_attribs.top(), 0, values).size();
            if(done < values.size()) {
                _errors.set(error_code::not_enough_data);
            }
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    template <typename T, typename I>
    auto do_convert(
      span<T> values,
      span_size_t& done,
      std::type_identity<I>) noexcept -> result {
        std::vector<I> temp;
        temp.resize(values.std_size());
        do_read(cover(temp), done);
        if(done == values.size()) {
            span_size_t i{0};
            for(const auto& val : temp) {
                if(const auto conv{convert_if_fits<T>(val)}) {
                    values[i++] = extract(conv);
                } else {
                    _errors.set(error_code::invalid_format);
                    break;
                }
            }
        }
        return _errors;
    }

    auto do_convert(
      span<identifier> values,
      span_size_t& done,
      std::type_identity<std::string>) noexcept -> result {
        std::vector<std::string> temp;
        temp.resize(values.std_size());
        do_read(cover(temp), done);
        if(done == values.size()) {
            span_size_t i{0};
            for(const auto& str : temp) {
                if(identifier::can_be_encoded(view(str))) {
                    values[i++] = identifier(view(str));
                } else {
                    _errors.set(error_code::invalid_format);
                    break;
                }
            }
        }
        return _errors;
    }

    auto do_convert(
      span<decl_name_storage> values,
      span_size_t& done,
      std::type_identity<std::string>) noexcept -> result {
        std::vector<std::string> temp;
        temp.resize(values.std_size());
        do_read(cover(temp), done);
        if(done == values.size()) {
            span_size_t i{0};
            for(const auto& str : temp) {
                if(values[i].assign(view(str))) {
                    ++i;
                } else {
                    _errors.set(error_code::invalid_format);
                    break;
                }
            }
        }
        return _errors;
    }

    auto read(span<bool> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<char> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<std::int8_t> v, span_size_t& d) noexcept -> result final {
        return do_convert(v, d, std::type_identity<short>{});
    }

    auto read(span<short> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<int> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<long> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<long long> v, span_size_t& d) noexcept -> result final {
        return do_convert(v, d, std::type_identity<long>{});
    }

    auto read(span<std::uint8_t> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<unsigned short> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<unsigned> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<unsigned long> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<unsigned long long> v, span_size_t& d) noexcept
      -> result final {
        return do_convert(v, d, std::type_identity<unsigned long>{});
    }

    auto read(span<float> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto read(span<double> v, span_size_t& d) noexcept -> result final {
        return do_convert(v, d, std::type_identity<float>{});
    }

    auto read(span<identifier> v, span_size_t& d) noexcept -> result final {
        return do_convert(v, d, std::type_identity<std::string>{});
    }

    auto read(span<decl_name_storage> v, span_size_t& d) noexcept
      -> result final {
        return do_convert(v, d, std::type_identity<std::string>{});
    }

    auto read(span<std::string> v, span_size_t& d) noexcept -> result final {
        return do_read(v, d);
    }

    auto begin_struct(span_size_t& count) noexcept -> result final {
        if(not _attribs.empty()) {
            count = _source.nested_count(_attribs.top());
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    auto begin_member(const string_view name) noexcept -> result final {
        if(not _attribs.empty()) {
            if(auto nested{_source.nested(_attribs.top(), name)}) {
                _attribs.emplace(std::move(nested));
            } else {
                _errors.set(error_code::missing_member);
            }
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    auto finish_member(const string_view) noexcept -> result final {
        if(not _attribs.empty()) {
            _attribs.pop();
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    auto finish_struct() noexcept -> result final {
        return _errors;
    }

    auto begin_list(span_size_t& count) noexcept -> result final {
        if(not _attribs.empty()) {
            count = _source.nested_count(_attribs.top());
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

    auto begin_element(const span_size_t) noexcept -> result final {
        return _errors;
    }

    auto finish_element(const span_size_t) noexcept -> result final {
        return _errors;
    }

    auto finish_list() noexcept -> result final {
        return _errors;
    }

    auto finish() noexcept -> result final {
        if(not _attribs.empty()) {
            _attribs.pop();
        } else {
            _errors.set(error_code::backend_error);
        }
        return _errors;
    }

private:
    result _errors{};
    valtree::compound _source;
    valtree::attribute _root;
    std::stack<valtree::attribute> _attribs;
};
//------------------------------------------------------------------------------
} // namespace eagine

