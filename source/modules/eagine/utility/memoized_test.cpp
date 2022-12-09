/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
import <map>;
//------------------------------------------------------------------------------
unsigned long fib_calc(unsigned long n) {
    unsigned long m[3] = {1, 1, 2};
    if(n < 3) {
        return m[n];
    }
    short i = 0;
    while(true) {
        m[i % 3] = m[(i + 1) % 3] + m[(i + 2) % 3];
        if(n <= 3) {
            break;
        }
        ++i;
        --n;
    }
    return m[i % 3];
}
//------------------------------------------------------------------------------
void memoized_fibonacci(auto& s) {
    eagitest::case_ test{s, 1, "fibonacci"};
    eagitest::track trck{test, 0, 5};

    using N = unsigned long;

    eagine::memoized<N(N)> fib_memo([](N _n, eagine::memoized<N(N)>& m) -> N {
        if(_n < 2) {
            return 1;
        }
        return m(_n, [](N n, auto& f) { return f(n - 2) + f(n - 1); });
    });

    for(int j = 0; j < 20; ++j) {
        for(unsigned long i = 0; i < 90; ++i) {
            test.check_equal(fib_calc(i), fib_memo(i), "A");
            trck.passed_part(1);
        }
        if(j % 3 == 1) {
            fib_memo.clear();
            trck.passed_part(2);
        }
    }

    unsigned long f[90];
    for(unsigned long i = 0; i < 90; ++i) {
        f[i] = fib_memo(i);
    }

    for(unsigned long i = 0; i < 90; ++i) {
        test.check_equal(f[i], fib_memo(i), "B");
        trck.passed_part(3);
    }

    for(unsigned long i = 0; i < 90; ++i) {
        fib_memo.reset(i);
        trck.passed_part(4);
    }

    for(unsigned long i = 0; i < 90; ++i) {
        test.check_equal(f[i], fib_memo(i), "C");
        trck.passed_part(5);
    }
}
//------------------------------------------------------------------------------
unsigned long fact_calc(unsigned n) {
    unsigned long r = n;
    while(n-- > 1) {
        r *= n;
    }
    return r;
}
//------------------------------------------------------------------------------
void memoized_factorial(auto& s) {
    eagitest::case_ test{s, 2, "factorial"};
    eagitest::track trck{test, 0, 2};

    using M = unsigned;
    using N = unsigned long;

    using namespace eagine;

    memoized<N(M)> fact_memo([](M _n, eagine::memoized<N(M)>& m) -> N {
        if(_n < 2) {
            return _n;
        }
        return m(_n, [](M n, auto& f) { return n * f(n - 1); });
    });
    for(int j = 0; j < 20; ++j) {
        for(unsigned i = 0; i < 22; ++i) {
            test.check_equal(fact_calc(i), fact_memo(i), "A");
            trck.passed_part(1);
        }
        if(j % 7 == 1) {
            fact_memo.clear();
            trck.passed_part(2);
        }
    }
}
//------------------------------------------------------------------------------
unsigned long exp_calc(unsigned x, unsigned e) {
    unsigned long r = 1;
    while(e-- > 0) {
        r *= x;
    }
    return r;
}
//------------------------------------------------------------------------------
void memoized_exponential(auto& s) {
    eagitest::case_ test{s, 3, "exponential"};
    eagitest::track trck{test, 0, 2};
    auto& rg = s.random();

    using M = unsigned;
    using N = unsigned long;

    eagine::memoized<N(M, M)> exp_memo(
      [](M _x, M _e, eagine::memoized<N(M, M)>& m) -> N {
          if(_e == 0) {
              return 1;
          }
          auto l = [](M x, M e, auto& f) {
              return x * f(x, e - 1);
          };
          return m(_x, _e, l);
      });

    for(int j = 0; j < 10; ++j) {
        for(unsigned i = 0; i < 64; ++i) {
            test.check_equal(exp_calc(2, i), exp_memo(2, i), "A");
            trck.passed_part(1);
        }
    }

    for(int j = 0; j < 1000; ++j) {
        const auto x = rg.get_between(0U, 20U);
        const auto e = rg.get_between(0U, 20U - x);
        test.check_equal(exp_calc(x, e), exp_memo(x, e), "B");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "memoized", 3};
    test.once(memoized_fibonacci);
    test.once(memoized_factorial);
    test.once(memoized_exponential);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
