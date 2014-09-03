/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/bool/logical.hpp>
#include <boost/hana/searchable/searchable.hpp>

#include <boost/hana/benchmark/measure.hpp>

<%= setup %>


int main() {
    auto searchable = <%= searchable %>;
    auto pred = [](auto&& x) { return boost::hana::false_; };

    boost::hana::benchmark::measure([=] {
        boost::hana::find(searchable, pred);
    });
}