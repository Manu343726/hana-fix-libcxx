/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/pair.hpp>

#include <boost/hana/detail/static_assert.hpp>
using namespace boost::hana;


int main() {
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') == pair(1, '2'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(1, '0'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(0, '2'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(0, '0'));

    BOOST_HANA_STATIC_ASSERT(pair(1, '2') == pair(1.0, '2'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(1.0, '0'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(0.0, '2'));
    BOOST_HANA_STATIC_ASSERT(pair(1, '2') != pair(0.0, '0'));
}