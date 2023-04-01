/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.utility;
//------------------------------------------------------------------------------
// 1
//------------------------------------------------------------------------------
template <typename T1, typename T2>
class protected_member_cls_1 : eagine::protected_member<std::pair<T1, T2>> {
private:
    using selector = std::type_identity<std::pair<T1, T2>>;

public:
    protected_member_cls_1(T1 v1, T2 v2) {
        this->get_the_member() = std::make_pair(v1, v2);
    }

    T1 get_v1() {
        return eagine::get_member<selector>(*this).first;
    }

    T2 get_v2() const {
        return eagine::get_member<selector>(*this).second;
    }
};
//------------------------------------------------------------------------------
template <typename T1, typename T2>
void protected_member_1_T1T2(eagitest::case_& test) {
    const T1 v1 = test.random().get_any<T1>();
    const T2 v2 = test.random().get_any<T2>();

    protected_member_cls_1<T1, T2> x(v1, v2);

    test.check_equal(x.get_v1(), v1, "v1");
    test.check_equal(x.get_v2(), v2, "v2");
}
//------------------------------------------------------------------------------
void protected_member_1(unsigned, auto& s) {
    eagitest::case_ test{s, 1, "1"};
    protected_member_1_T1T2<int, float>(test);
    protected_member_1_T1T2<char, double>(test);
}
//------------------------------------------------------------------------------
// 2
//------------------------------------------------------------------------------
template <typename T1, typename T2, typename T3>
class protected_member_cls_2
  : eagine::protected_member<T1, eagine::selector<1>>
  , eagine::protected_member<T2, eagine::selector<2>>
  , eagine::protected_member<T3, eagine::selector<3>> {
public:
    protected_member_cls_2(T1 v1, T2 v2, T3 v3) {
        using namespace eagine;
        get_member<selector<1>>(*this) = v1;
        get_member<selector<2>>(*this) = v2;
        get_member<selector<3>>(*this) = v3;
    }

    T1 get_v1() {
        using namespace eagine;
        return get_member<selector<1>>(*this);
    }

    T2 get_v2() const {
        using namespace eagine;
        return get_member<selector<2>>(*this);
    }

    T3 get_v3() const {
        using namespace eagine;
        return get_member<selector<3>>(*this);
    }
};
//------------------------------------------------------------------------------
template <typename T1, typename T2, typename T3>
void protected_member_2_T1T2T3(eagitest::case_& test) {
    const T1 v1 = test.random().get_any<T1>();
    const T2 v2 = test.random().get_any<T2>();
    const T3 v3 = test.random().get_any<T3>();

    protected_member_cls_2<T1, T2, T3> x(v1, v2, v3);

    test.check_equal(x.get_v1(), v1, "v1");
    test.check_equal(x.get_v2(), v2, "v2");
    test.check_equal(x.get_v3(), v3, "v3");
}
//------------------------------------------------------------------------------
void protected_member_2(unsigned, auto& s) {
    eagitest::case_ test{s, 2, "2"};

    protected_member_2_T1T2T3<short, float, bool>(test);
    protected_member_2_T1T2T3<char, double, char>(test);
    protected_member_2_T1T2T3<int, int, int>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "protected member", 2};
    test.repeat(1000, protected_member_1);
    test.repeat(1000, protected_member_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
