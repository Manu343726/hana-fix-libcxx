/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana.hpp>

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
using namespace boost::hana;
using namespace std::literals;


//! [utilities]
template <typename Xs>
std::string join(Xs&& xs, std::string sep) {
  return fold(intersperse(std::forward<Xs>(xs), sep), "", _ + _);
}

std::string quote(std::string s) { return "\"" + s + "\""; }

template <typename T>
auto to_json(T const& x) -> decltype(std::to_string(x)) {
  return std::to_string(x);
}

std::string to_json(char c) { return quote({c}); }
std::string to_json(std::string s) { return quote(s); }
//! [utilities]

//! [Struct]
template <typename T>
  std::enable_if_t<models<Struct, T>(),
std::string> to_json(T const& x) {
  auto json = transform(keys(x), [&](auto name) {
    auto const& member = at_key(x, name);
    return quote(to<char const*>(name)) + " : " + to_json(member);
  });

  return "{" + join(std::move(json), ", ") + "}";
}
//! [Struct]

//! [Sequence]
template <typename Xs>
  std::enable_if_t<models<Sequence, Xs>(),
std::string> to_json(Xs const& xs) {
  auto json = transform(xs, [](auto const& x) {
    return to_json(x);
  });

  return "[" + join(std::move(json), ", ") + "]";
}
//! [Sequence]


int main() {
//! [usage]
struct Car {
  BOOST_HANA_DEFINE_STRUCT(Car,
    (std::string, brand),
    (std::string, model)
  );
};

struct Person {
  BOOST_HANA_DEFINE_STRUCT(Person,
    (std::string, name),
    (std::string, last_name),
    (int, age)
  );
};

Car bmw{"BMW", "Z3"}, audi{"Audi", "A4"};
Person john{"John", "Doe", 30};

auto tuple = make_tuple(john, audi, bmw);
std::cout << to_json(tuple) << std::endl;
//! [usage]
}
