/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/ext/std/ratio.hpp>

#include <boost/hana/detail/assert.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

#include <test/auto/base.hpp>
#include <test/cnumeric.hpp>

// instances
#include <test/auto/comparable.hpp>
#include <test/auto/group.hpp>
#include <test/auto/integral_domain.hpp>
#include <test/auto/monoid.hpp>
#include <test/auto/orderable.hpp>
#include <test/auto/ring.hpp>

#include <ratio>
using namespace boost::hana;


namespace boost { namespace hana { namespace test {
    template <>
    auto instances<ext::std::Ratio> = tuple(
        type<Comparable>,
        type<Group>,
        type<IntegralDomain>,
        type<Monoid>,
        type<Orderable>,
        type<Ring>
    );

    template <>
    auto objects<ext::std::Ratio> = tuple(
        ::std::ratio<0>{},
        ::std::ratio<1, 3>{},
        ::std::ratio<1, 2>{},
        ::std::ratio<2, 6>{},
        ::std::ratio<3, 1>{},
        ::std::ratio<7, 8>{},
        ::std::ratio<3, 5>{},
        ::std::ratio<2, 1>{}
    );
}}}


int main() {
    test::check_datatype<ext::std::Ratio>();

    // Conversions
    {
        // IntegralConstant -> Ratio
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                to<ext::std::Ratio>(test::cnumeric<int, 0>),
                std::ratio<0>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                to<ext::std::Ratio>(test::cnumeric<int, 1>),
                std::ratio<1>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                to<ext::std::Ratio>(test::cnumeric<int, 3>),
                std::ratio<3>{}
            ));
        }
    }

    // Comparable
    {
        // equal
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                std::ratio<0>{},
                std::ratio<0>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                std::ratio<3, 5>{},
                std::ratio<6, 10>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(not_(equal(
                std::ratio<4, 5>{},
                std::ratio<6, 10>{}
            )));
        }
    }

    // Orderable
    {
        // less
        {
            BOOST_HANA_CONSTANT_ASSERT(less(
                std::ratio<1>{},
                std::ratio<3>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(less(
                std::ratio<4, 10>{},
                std::ratio<3, 5>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(not_(less(
                std::ratio<3, 5>{},
                std::ratio<4, 10>{}
            )));
        }
    }

    // Monoid
    {
        // plus
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                plus(std::ratio<3, 4>{}, std::ratio<5, 10>{}),
                std::ratio<3*10 + 5*4, 4*10>{}
            ));
        }

        // zero
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                zero<ext::std::Ratio>,
                std::ratio<0, 1>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                zero<ext::std::Ratio>,
                std::ratio<0, 2>{}
            ));
        }
    }

    // Group
    {
        // minus
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                minus(std::ratio<3, 4>{}, std::ratio<5, 10>{}),
                std::ratio<3*10 - 5*4, 4*10>{}
            ));
        }
    }

    // Ring
    {
        // mult
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                mult(std::ratio<3, 4>{}, std::ratio<5, 10>{}),
                std::ratio<3*5, 4*10>{}
            ));
        }

        // one
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                one<ext::std::Ratio>,
                std::ratio<1, 1>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                one<ext::std::Ratio>,
                std::ratio<2, 2>{}
            ));
        }
    }

    // IntegralDomain
    {
        // quot
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                quot(std::ratio<6>{}, std::ratio<4>{}),
                std::ratio<6, 4>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                quot(std::ratio<3, 4>{}, std::ratio<5, 10>{}),
                std::ratio<3*10, 4*5>{}
            ));
        }

        // mod
        {
            BOOST_HANA_CONSTANT_ASSERT(equal(
                mod(std::ratio<6>{}, std::ratio<4>{}),
                std::ratio<0>{}
            ));

            BOOST_HANA_CONSTANT_ASSERT(equal(
                mod(std::ratio<3, 4>{}, std::ratio<5, 10>{}),
                std::ratio<0>{}
            ));
        }
    }
}