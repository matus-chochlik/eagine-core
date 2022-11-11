/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:curves;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :functions;
import <cmath>;
import <vector>;
import <utility>;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Helper class for bezier curve segment calculations.
/// @ingroup math
/// @tparam Type the interpolated type.
/// @tparam Parameter the curve interpolation parameter type.
/// @tparam N the curve order.
/// @see bezier_curves
template <typename Type, typename Parameter, int N>
struct bezier_t {
public:
    /// @brief Interpolate from control points in pack @p p at position @p t.
    template <typename... Points>
    constexpr auto operator()(const Parameter t, Points&&... p) const noexcept
        requires(sizeof...(Points) == N)
    {
        return _calc(N - 1, 0, t, std::forward<Points>(p)...);
    }

    /// @brief Interpolate from control points in span @p p at position @p t.
    template <typename P, typename S>
    auto operator()(const Parameter t, memory::basic_span<const Type, P, S> p)
      const noexcept {
        return _calc(N - 1, 0, t, p, std::make_index_sequence<N>());
    }

private:
    static constexpr auto _coef(
      const int m,
      const int i,
      const Parameter t) noexcept {
        using std::pow;
        return pow(t, Parameter(i)) * pow(1 - t, Parameter(m - i)) *
               Parameter(binomial(m, i));
    }

    static constexpr auto _calc(const int, const int, const Parameter) noexcept {
        return Type{0};
    }

    template <typename... P>
    static constexpr auto _calc(
      const int m,
      const int i,
      const Parameter t,
      const Type first,
      const P... rest) noexcept -> Type {
        return first * _coef(m, i, t) + _calc(m, i + 1, t, rest...);
    }

    template <typename P, typename S, std::size_t... I>
    static constexpr auto _calc(
      const int m,
      const int i,
      const Parameter t,
      const memory::basic_span<const Type, P, S> p,
      const std::index_sequence<I...>) noexcept -> Type {
        return _calc(m, i, t, p[I]...);
    }
};
//------------------------------------------------------------------------------
export template <typename Parameter, typename... CP>
constexpr auto bezier_point(const Parameter t, const CP... cps) noexcept
  -> std::common_type_t<CP...> {
    using bt = bezier_t<std::common_type_t<CP...>, Parameter, sizeof...(CP)>;
    return bt{}(t, cps...);
}
//------------------------------------------------------------------------------
/// @brief A sequence of Bezier curves, possibly connected at end points.
/// @ingroup math
/// @see CubicBezierLoop
///
/// This class stores the data for a sequence of connected Bezier curves
/// of a given @c Order. The begin of the i-th curve (segment) is equal
/// to the end of the (i-1)-th curve and the end of the i-th curve is
/// equal to the begin of the (i+1)-th curve. Between the begin and
/// end (control) points there is a fixed number (Order - 1) of curve control
/// points. The curves pass through the begin and end and are influenced
/// by the other control points.
export template <typename Type, typename Parameter, span_size_t Order>
class bezier_curves {
public:
    /// @brief Returns whether the curves made from points are connected.
    static auto are_connected(const std::vector<Type>& points) noexcept
      -> bool {
        return ((points.size() - 1) != Order) &&
               ((points.size() - 1) % Order) == 0;
    }

    /// @brief Returns true if the individual curves are connected.
    auto is_connected() const noexcept -> bool {
        return _connected;
    }

    static auto are_separated(const std::vector<Type>& points) noexcept
      -> bool {
        return (points.size() % (Order + 1)) == 0;
    }

    /// @brief Returns true if the individual curves are disconnected.
    auto is_separated() const noexcept -> bool {
        return !_connected;
    }

    /// @brief checks if the sequence of control points is OK for this curve type.
    static auto points_are_ok(const std::vector<Type>& points) noexcept
      -> bool {
        return !points.empty() &&
               (are_connected(points) || are_separated(points));
    }

    /// @brief Default constructor.
    bezier_curves() noexcept = default;

    /// @brief Creates the bezier curves from the control @c points.
    /// @pre points_are_ok(points)
    ///
    /// The number of points must be ((C * Order) + 1) or (C * (Order + 1))
    /// where @em C is the number of curves (segments) in the sequence.
    /// If both of the above are true then the curves are considered
    /// to be connected.
    bezier_curves(std::vector<Type> points)
      : _points{std::move(points)}
      , _connected{are_connected(_points)} {
        assert(points_are_ok(_points));
    }

    /// @brief Creates the bezier curves from the control @c points.
    /// @pre points_are_ok(points)
    /// @pre are_connected(points) == connected
    ///
    /// The number of points must be ((C * Order) + 1) and connected
    /// or (C * (Order + 1)) and not(connected),
    /// where @em C is the number of curves (segments) in the sequence.
    bezier_curves(std::vector<Type> points, const bool connected)
      : _points{std::move(points)}
      , _connected{connected} {
        assert(points_are_ok(_points));
        assert(are_connected(_points) == _connected);
    }

    /// @brief Creates the bezier curves from the control @c points.
    /// @pre points_are_ok(points)
    ///
    /// The number of points must be ((C * Order) + 1) or (C * (Order + 1))
    /// where @em C is the number of curves (segments) in the sequence.
    /// If both of the above are true then the curves are considered
    /// to be connected.
    template <typename P, typename S>
    bezier_curves(memory::basic_span<const Type, P, S> points)
      : _points{points.begin(), points.end()}
      , _connected{are_connected(_points)} {
        assert(points_are_ok(_points));
    }

    /// @brief Creates the bezier curves from the control @c points.
    /// @pre points_are_ok(points)
    /// @pre are_connected(points) == connected
    ///
    /// The number of points must be ((C * Order) + 1) or (C * (Order + 1))
    /// where @em C is the number of curves (segments) in the sequence.
    /// If both of the above are true then the curves are considered
    /// to be connected.
    template <typename P, typename S>
    bezier_curves(
      memory::basic_span<const Type, P, S> points,
      const bool connected)
      : _points{points.begin(), points.end()}
      , _connected{connected} {
        assert(points_are_ok(_points));
        assert(are_connected(_points) == _connected);
    }

    auto segment_step() const noexcept -> span_size_t {
        assert(points_are_ok(_points));
        return _connected ? Order : Order + 1;
    }

    /// @brief Returns the count of individual curves in the sequence.
    auto segment_count() const noexcept -> span_size_t {
        assert(points_are_ok(_points));

        return _connected ? span_size(_points.size() - 1) / Order
                          : span_size(_points.size()) / (Order + 1);
    }

    /// @brief Returns the contol points of the curve.
    auto control_points() const noexcept -> const std::vector<Type>& {
        return _points;
    }

    /// @brief Clears the control points.
    auto clear() noexcept -> auto& {
        _points.clear();
        return *this;
    }

    /// @brief Wraps the parameter value to [0.0, 1.0].
    static auto wrap(Parameter t) noexcept -> Parameter {
        const Parameter zero{0};
        const Parameter one{1};
        if(t < zero) {
            t += std::floor(std::fabs(t)) + one;
        } else if(t > one) {
            t -= std::floor(t);
        }
        assert(t >= zero && t <= one);
        return t;
    }

    /// @brief Gets the point on the curve at position t (must be in (0.0, 1.0)).
    auto position01(Parameter t) const noexcept -> Type {
        const Parameter zero{0};
        const Parameter one{1};

        if(t >= one) {
            t = zero;
        }
        assert(t >= zero && t < one);

        const auto toffs = t * Parameter(segment_count());
        const auto t_sub = toffs - std::floor(toffs);
        const auto poffs = span_size_t(toffs) * segment_step();
        assert(poffs < span_size(_points.size()) - Order);

        return _bezier(t_sub, skip(view(_points), poffs));
    }

    /// @brief Gets the point on the curve at position t wrapped to [0.0, 1.0].
    auto position(const Parameter t) const noexcept -> Type {
        return position01(wrap(t));
    }

    /// @brief Makes a sequence of points on the curve (n points per segment).
    void approximate(
      std::vector<Type>& dest,
      const valid_if_positive<span_size_t>& n) const noexcept {
        const auto sstep = segment_step();
        const auto s = segment_count();

        dest.resize(integer(s * extract(n) + 1));

        auto p = dest.begin();
        const Parameter t_step = Parameter(1) / Parameter(extract(n));

        for(const auto i : integer_range(s)) {
            const auto poffs = i * sstep;
            auto t_sub = Parameter(0);
            for([[maybe_unused]] const auto j : integer_range(extract(n))) {
                assert(p != dest.end());
                *p = Type(_bezier(t_sub, skip(view(_points), poffs)));
                ++p;

                t_sub += t_step;
            }
        }
        assert(p != dest.end());
        *p = _points.back();
        ++p;
        assert(p == dest.end());
    }

    /// @brief Returns a sequence of points on the curve (n points per segment).
    auto approximate(const valid_if_positive<span_size_t> n) const
      -> std::vector<Type> {
        std::vector<Type> result;
        approximate(result, n);
        return result;
    }

    /// @brief Returns a derivative of this curve
    auto derivative() const noexcept
      -> bezier_curves<Type, Parameter, Order - 1> {
        const auto sstep = segment_step();
        const auto s = segment_count();

        std::vector<Type> new_points(integer(s * Order));
        auto p = new_points.begin();

        for(const auto i : integer_range(s)) {
            for(const auto j : integer_range(Order)) {
                const auto k = i * sstep + j;
                assert(p != new_points.end());
                *p = (_points[integer(k + 1)] - _points[integer(k)]) * Order;
                ++p;
            }
        }
        assert(p == new_points.end());

        return {std::move(new_points), false};
    }

private:
    static_assert(Order > 0);

    std::vector<Type> _points;
    bezier_t<Type, Parameter, Order + 1> _bezier{};
    bool _connected{false};
};

/// @brief Alias for specialization of bezier_curves for cubic curves.
/// @ingroup math
export template <typename Type, typename Parameter>
using cubic_bezier_curves = bezier_curves<Type, Parameter, 3>;
//------------------------------------------------------------------------------
/// @brief A closed smooth cubic Bezier spline passing through all input points.
/// @ingroup math
///
/// This class constructs a closed sequence of Bezier curves that are smooth
/// at the curve connection points. The control points between the begin
/// and end points of each segment are calculated automatically to make
/// the transition between the individual segments smooth.
export template <typename Type, typename Parameter>
class cubic_bezier_loop : public cubic_bezier_curves<Type, Parameter> {
public:
    cubic_bezier_loop() noexcept = default;

    /// @brief Creates a loop passing through the sequence of the input points.
    template <typename P, typename S>
    cubic_bezier_loop(
      const memory::basic_span<const Type, P, S> points,
      const Parameter r)
      : bezier_curves<Type, Parameter, 3>(_make_cpoints(points, r)) {}

    /// @brief Creates a loop passing through the sequence of the input points.
    template <typename P, typename S>
    cubic_bezier_loop(const memory::basic_span<const Type, P, S> points)
      : cubic_bezier_loop(points, Parameter(1) / Parameter(3)) {}

private:
    template <typename P, typename S>
    static auto _make_cpoints(
      const memory::basic_span<const Type, P, S> points,
      const Parameter r) -> std::vector<Type> {
        span_size_t i = 0, n = points.size();
        assert(n != 0);

        std::vector<Type> result(integer(n * 3 + 1));
        auto ir = result.begin();

        while(i != n) {
            const auto a = (n + i - 1) % n;
            const auto b = i;
            const auto c = (i + 1) % n;
            const auto d = (i + 2) % n;

            assert(ir != result.end());
            *ir = points[b];
            ++ir;
            assert(ir != result.end());
            *ir = Type(points[b] + (points[c] - points[a]) * r);
            ++ir;
            assert(ir != result.end());
            *ir = Type(points[c] + (points[b] - points[d]) * r);
            ++ir;
            ++i;
        }
        assert(ir != result.end());
        *ir = points[0];
        ++ir;
        assert(ir == result.end());

        return result;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::math
