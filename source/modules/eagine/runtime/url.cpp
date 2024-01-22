/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:url;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.container;
import :program_args;

namespace eagine {
export class url;
//------------------------------------------------------------------------------
/// @brief Value map class for storing URL query parts.
/// @see url
export struct url_query_args
  : flat_map<string_view, string_view, basic_view_less<string_view>> {

    /// @brief Returns the value of the argument with the specified name.
    /// @see arg_has_value
    /// @see arg_value_as
    /// @see decoded_arg_value
    /// @see has_arg
    auto arg_value(const string_view name) const noexcept
      -> optionally_valid<string_view>;

    /// @brief Returns the value of the URL-encoded argument with the specified name.
    /// @see arg_value
    /// @see arg_has_value
    /// @see arg_value_as
    /// @see arg_url
    auto decoded_arg_value(const string_view name) const noexcept
      -> optionally_valid<std::string>;

    /// @brief Returns the value of the specified argument as an URL
    /// @see arg_value
    /// @see arg_value_as
    auto arg_url(const string_view name) const noexcept -> url;

    /// @brief Converts the value of the argument with the specified name to type T.
    /// @see arg_has_value
    /// @see arg_value
    /// @see has_arg
    template <typename T>
    auto arg_value_as(const string_view name, std::type_identity<T> = {})
      const noexcept -> optionally_valid<T> {
        return arg_value(name)
          .and_then(from_string<T>(_1))
          .and_then(_1.cast_to<optionally_valid<T>>());
    }

    auto arg_value_as(
      const string_view name,
      std::type_identity<string_view> = {}) const noexcept
      -> optionally_valid<string_view> {
        return arg_value(name);
    }

    /// @brief Checks if argument convertible to type T exists.
    /// @see arg_has_value
    /// @see arg_value_as
    template <typename T = string_view>
    auto has_arg(const string_view name, std::type_identity<T> tid = {})
      const noexcept -> bool {
        return arg_value_as(name, tid).has_value();
    }

    /// @brief Checks if the argument with the specified name has the specified value.
    /// @see arg_value
    /// @see arg_value_as
    template <typename T>
    auto arg_has_value(const string_view name, const T& value) const noexcept
      -> bool {
        return arg_value(name).and_then(string_has_value(_1, value)).or_false();
    }
};
//------------------------------------------------------------------------------
/// @brief Class parsing and providing access to parts of an URL.
/// @see url_query_args
export class url {
public:
    /// @brief Default constructor.
    url() noexcept = default;

    /// @brief Construction from a string.
    url(std::string url_str) noexcept
      : url{std::move(url_str), std::match_results<std::string::iterator>{}} {}

    /// @brief Construction from an command-line argument.
    url(const program_arg& arg) noexcept
      : url{arg.get_string()} {}

    /// @brief Indicates if the given string is a valid url.
    static auto is_url(const string_view) noexcept -> bool;

    /// @brief Encodes the given string using the URI/URL encoding.
    static auto encode_component(const string_view) noexcept -> std::string;

    /// @brief Decode the given string using the URI/URL encoding.
    static auto decode_component(const string_view) noexcept
      -> optionally_valid<std::string>;

    /// @brief Compares two (possible URI/URL encoded) strings for equality.
    static auto components_are_equal(
      const string_view,
      const string_view) noexcept -> bool;

    /// @brief Comparison.
    auto operator<=>(const url& that) const noexcept {
        return _url_str.compare(that._url_str);
    }

    auto operator==(const url& that) const noexcept -> bool {
        return _url_str == that._url_str;
    }
    auto operator<(const url& that) const noexcept -> bool {
        return _url_str < that._url_str;
    }

    /// @brief Indicates if the URL was parsed successfully.
    auto has_value() const noexcept {
        return _parsed;
    }

    /// @brief Indicates if the URL was parsed successfully.
    /// @see has_value
    explicit operator bool() const noexcept {
        return has_value();
    }

    /// @brief Returns the whole URL string.
    auto str() const noexcept -> string_view {
        return {_url_str};
    }

    /// @brief Returns the scheme.
    /// @see has_scheme
    auto scheme() const noexcept -> valid_if_not_empty<string_view> {
        return _swov(_scheme);
    }

    /// @brief Checks if the url scheme matches the given string.
    /// @see scheme
    auto has_scheme(const string_view str) const noexcept -> bool {
        return are_equal(str, _sw(_scheme));
    }

    /// @brief Returns the login name.
    auto login() const noexcept -> optionally_valid<std::string>;

    /// @brief Returns the login password.
    auto password() const noexcept -> optionally_valid<std::string>;

    /// @brief Returns the host name or IP address.
    /// @see domain
    auto host() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_host)};
    }

    /// @brief Returns the host name or IP address.
    /// @see host
    /// @see has_domain
    auto domain() const noexcept -> valid_if_not_empty<string_view> {
        return host();
    }

    /// @brief Checks if the domain matches the given string.
    /// @see domain
    /// @see host
    auto has_domain(const string_view str) const noexcept -> bool;

    /// @brief Returns the host name or IP address.
    /// @see host
    /// @see has_domain
    auto domain_identifier() const noexcept -> identifier;

    /// @brief Returns the port string.
    auto port_str() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_port)};
    }

    /// @brief Returns the port.
    auto port() const noexcept -> valid_if_not_zero<int> {
        return {from_string<int>(_sw(_port)).value_or(0)};
    }

    /// @brief Returns the path string.
    auto path_str() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_path)};
    }

    /// @brief Returns the path dirname.
    /// @see path_basename
    auto path_dirname() const noexcept -> valid_if_not_empty<string_view>;

    /// @brief Returns the path basename.
    /// @see path_dirname
    /// @see has_path_prefix
    /// @see has_path_suffix
    auto path_basename() const noexcept -> valid_if_not_empty<string_view>;

    /// @brief Indicates if path ends with the specified prefix
    /// @see path_basename
    /// @see has_path_suffix
    /// @see path_str
    auto has_path_prefix(const string_view prefix) const noexcept -> bool;

    /// @brief Indicates if path ends with the specified suffix
    /// @see path_basename
    /// @see has_path_prefix
    /// @see path_str
    auto has_path_suffix(const string_view suffix) const noexcept -> bool;

    /// @brief Returns the path.
    /// @see has_path
    /// @see path_identifier
    auto path() const noexcept -> basic_string_path;

    /// @brief Checks if the path matches the given string.
    /// @see path_str
    /// @see path
    auto has_path(const string_view str) const noexcept -> bool;

    /// @brief Returns the path as identifier if possible.
    /// @see path_str
    /// @see path_identifier
    auto path_identifier() const noexcept -> identifier;

    /// @brief Returns the query string.
    auto query_str() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_query)};
    }

    /// @brief Returns the query.
    /// @see argument
    auto query() const noexcept -> const url_query_args& {
        return _query_args;
    }

    /// @brief Returns the value of the specified query argument.
    /// @see query
    auto argument(const string_view arg_name) const noexcept
      -> valid_if_not_empty<string_view>;

    /// @brief Returns the fragment.
    auto fragment() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_fragment)};
    }

    /// @brief Releases the internally allocated URL string.
    /// @post not has_value()
    auto release_string() noexcept -> std::string {
        _parsed = false;
        return {std::move(_url_str)};
    }

private:
    url(std::string, std::match_results<std::string::iterator>) noexcept;

    using _range = std::pair<span_size_t, span_size_t>;

    auto _sw(_range r) const noexcept -> string_view;
    auto _swov(_range r) const noexcept -> valid_if_not_empty<string_view>;

    auto _parse_args() const noexcept -> url_query_args;

    void _cover(
      _range& part,
      const std::match_results<std::string::iterator>& match,
      const std::size_t index) const noexcept;

    static auto _get_regex() noexcept -> const std::regex&;

    std::string _url_str;

    _range _scheme;
    _range _login;
    _range _passwd;
    _range _host;
    _range _port;
    _range _path;
    _range _query;
    _range _fragment;
    url_query_args _query_args;
    bool _parsed{false};
};
//------------------------------------------------------------------------------
} // namespace eagine

