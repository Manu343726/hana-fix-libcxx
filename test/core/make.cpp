/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/assert.hpp>
#include <boost/hana/core/make.hpp>
using namespace boost::hana;


struct udt {
    int value;
    explicit udt(int v) : value(v) { }
};

int main() {
    BOOST_HANA_CONSTEXPR_CHECK(make<udt>(1).value == 1);
}
