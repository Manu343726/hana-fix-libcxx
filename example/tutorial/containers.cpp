/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana.hpp>

#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
using namespace boost::hana;
using namespace boost::hana::literals;
using namespace std::literals;


int main() {

{

//! [make<Tuple>]
auto xs = make<Tuple>(1, 2.2, 'a', "bcde"s);
//! [make<Tuple>]

}{

//! [make<Range>]
constexpr auto r = make<Range>(int_<3>, int_<10>);
static_assert(r == make_range(int_<3>, int_<10>), "");
//! [make<Range>]

}{

//! [lifetime]
std::string hello = "Hello";
std::vector<char> world = {'W', 'o', 'r', 'l', 'd'};

// hello is copied, world is moved-in
auto xs = make_tuple(hello, std::move(world));

// s is a reference to the copy of hello inside xs.
// It becomes a dangling reference as soon as xs is destroyed.
std::string& s = xs[0_c];
//! [lifetime]
(void)s;

}{

//! [reference_wrapper]
std::vector<int> ints = { /* huge vector of ints */ };
std::vector<std::string> strings = { /* huge vector of strings */ };

auto map = make_map(
  make_pair(type<int>, std::ref(ints)),
  make_pair(type<std::string>, std::ref(strings))
);

auto& v = map[type<int>].get();
BOOST_HANA_RUNTIME_CHECK(&v == &ints);
//! [reference_wrapper]

}{

//! [types]
auto xs = make_tuple(1, '2', "345");
auto ints = make_range(int_<0>, int_<100>);
// what can we say about the types of xs and ints?
//! [types]
(void)xs;
(void)ints;

}

}


namespace overloading {
//! [overloading]
template <typename T>
void f(std::vector<T> xs) {
  // ...
}

template <typename Xs, typename = std::enable_if_t<is_a<Tuple, Xs>()>>
void f(Xs xs) {
  // ...
}
//! [overloading]
}
