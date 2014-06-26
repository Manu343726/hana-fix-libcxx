/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/foldable.hpp>

#include <boost/hana/detail/constexpr.hpp>
#include <boost/hana/detail/static_assert.hpp>
#include <boost/hana/integral.hpp>

#include <tuple>
#include "minimal_foldable.hpp"
using namespace boost::hana;


BOOST_HANA_CONSTEXPR_LAMBDA auto f = [](auto xs, auto x) {
    return std::tuple_cat(xs, std::make_tuple(x));
};

int main() {
    {
    constexpr auto s = std::make_tuple(1);
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s)) == s);
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, '2')) == f(s, '2'));
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, '2', 3.3)) == f(f(s, '2'), 3.3));
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, '2', 3.3, 4.4f)) == f(f(f(s, '2'), 3.3), 4.4f));
    }

    {
    constexpr auto s = std::make_tuple(int_<1>);
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s)) == s);
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, int_<2>)) == f(s, int_<2>));
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, int_<2>, int_<3>)) == f(f(s, int_<2>), int_<3>));
    BOOST_HANA_STATIC_ASSERT(foldl1(f, foldable(s, int_<2>, int_<3>, int_<4>)) == f(f(f(s, int_<2>), int_<3>), int_<4>));
    }
}