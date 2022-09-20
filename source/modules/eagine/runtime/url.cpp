/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:url;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.container;
export import <string>;
import <charconv>;
import <regex>;

namespace eagine {

/// @brief Value map class for storing URL query parts.
/// @see url
export struct url_query_args
  : flat_map<string_view, string_view, basic_view_less<string_view>> {

    /// @brief Returns the value of the argument with the specified name.
    /// @see arg_has_value
    /// @see arg_value_as
    auto arg_value(const string_view name) const noexcept
      -> valid_if_not_empty<string_view> {
        if(const auto pos{find(name)}; pos != end()) {
            return {std::get<1>(*pos)};
        }
        return {};
    }

    /// @brief Converts the value of the argument with the specified name to type T.
    /// @see arg_has_value
    /// @see arg_value
    template <typename T>
    auto arg_value_as(const string_view name, std::type_identity<T> = {})
      const noexcept -> std::optional<T> {
        if(const auto pos{find(name)}; pos != end()) {
            return from_string<T>(std::get<1>(*pos));
        }
        return {};
    }

    /// @brief Checks if the argument with the specified name has the specified value.
    /// @see arg_value
    /// @see arg_value_as
    template <typename T>
    auto arg_has_value(const string_view name, const T& value) const noexcept
      -> bool {
        if(const auto pos{find(name)}; pos != end()) {
            return string_has_value(std::get<1>(*pos), value);
        }
        return false;
    }
};

/// @brief Class parsing and providing access to parts of an URL.
/// @see url_query_args
export class url {
public:
    /// @brief Default constructor.
    url() noexcept = default;

    /// @brief Construction from a string.
    url(std::string url_str) noexcept
      : url{std::move(url_str), std::match_results<std::string::iterator>{}} {}

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
    auto is_valid() const noexcept {
        return _parsed;
    }

    /// @brief Indicates if the URL was parsed successfully.
    /// @see is_valid
    explicit operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Returns the whole URL string.
    auto str() const noexcept -> string_view {
        return {_url_str};
    }

    /// @brief Returns the scheme.
    /// @see has_scheme
    auto scheme() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_scheme)};
    }

    /// @brief Checks if the url scheme matches the given string.
    /// @see scheme
    auto has_scheme(const string_view str) const noexcept -> bool {
        return are_equal(str, _sw(_scheme));
    }

    /// @brief Returns the login name.
    auto login() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_login)};
    }

    /// @brief Returns the login password.
    auto password() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_passwd)};
    }

    /// @brief Returns the host name or IP address.
    /// @see domain
    auto host() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_host)};
    }

    /// @brief Returns the host name or IP address.
    /// @see host
    auto domain() const noexcept -> valid_if_not_empty<string_view> {
        return host();
    }

    /// @brief Returns the host name or IP address.
    /// @see host
    auto domain_identifier() const noexcept -> identifier {
        if(const auto str{_sw(_host)}) {
            if(identifier::can_be_encoded(str)) {
                return identifier{str};
            }
        }
        return {};
    }

    /// @brief Returns the port string.
    auto port_str() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_port)};
    }

    /// @brief Returns the port.
    auto port() const noexcept -> valid_if_not_zero<int> {
        int result{0};
        const auto p{_sw(_port)};
        [[maybe_unused]] const auto unused{
          std::from_chars(p.begin(), p.end(), result)};
        return {result};
    }

    /// @brief Returns the path string.
    auto path_str() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_path)};
    }

    /// @brief Returns the path.
    /// @see has_path
    /// @see path_identifier
    auto path() const noexcept -> basic_string_path {
        return {basic_string_path{_sw(_path), split_by, "/"}};
    }

    /// @brief Checks if the path matches the given string.
    /// @see path_str
    /// @see path
    auto has_path(const string_view str) const noexcept -> bool {
        return are_equal(str, _sw(_path));
    }

    /// @brief Returns the path as identifier if possible.
    /// @see path_str
    /// @see path_identifier
    auto path_identifier() const noexcept -> identifier {
        if(const string_view str{skip(_sw(_path), 1)}) {
            if(identifier::can_be_encoded(str)) {
                return identifier{str};
            }
        }
        return {};
    }

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
      -> valid_if_not_empty<string_view> {
        string_view result;
        for_each_delimited(_sw(_query), string_view{"+"}, [&](auto part) {
            auto [name, value] = split_by_first(part, string_view{"="});
            if(!result && are_equal(name, arg_name)) {
                result = value;
            }
        });
        return {result};
    }

    /// @brief Returns the fragment.
    auto fragment() const noexcept -> valid_if_not_empty<string_view> {
        return {_sw(_fragment)};
    }

private:
    url(std::string, std::match_results<std::string::iterator>) noexcept;

    using _range = std::pair<span_size_t, span_size_t>;

    auto _sw(_range r) const noexcept -> string_view {
        return string_view{
          head(skip(view(_url_str), std::get<0>(r)), std::get<1>(r))};
    }

    auto _parse_args() const noexcept -> url_query_args {
        url_query_args result;
        for_each_delimited(_sw(_query), string_view{"+"}, [&result](auto part) {
            auto [name, value] = split_by_first(part, string_view{"="});
            result[name] = value;
        });
        return result;
    }

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

} // namespace eagine

