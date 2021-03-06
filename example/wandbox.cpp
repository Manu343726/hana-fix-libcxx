#include <boost/hana.hpp>

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
using namespace boost::hana;
using namespace boost::hana::literals;
using namespace std::literals;


//////////////////////////////////////////////////////////////////////////////
// Welcome to Hana!
//
// You can play around and press 'Run' at the bottom of this file to compile
// and run this code.
//
// To get you started, here's a small JSON generator written with Hana
// (this is explained in the tutorial if you're interested):
//////////////////////////////////////////////////////////////////////////////

// 1. Define some utilities
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


// 2. Define how to print user-defined types
template <typename T>
  std::enable_if_t<models<Struct, T>(),
std::string> to_json(T const& x) {
  auto json = transform(keys(x), [&](auto name) {
    auto const& member = at_key(x, name);
    return quote(to<char const*>(name)) + " : " + to_json(member);
  });

  return "{" + join(std::move(json), ", ") + "}";
}

// 3. Define how to print Sequences
template <typename Xs>
  std::enable_if_t<models<Sequence, Xs>(),
std::string> to_json(Xs const& xs) {
  auto json = transform(xs, [](auto const& x) {
    return to_json(x);
  });

  return "[" + join(std::move(json), ", ") + "]";
}


// 4. Create your own types and make them compatible with Hana.
//    This can be done intrusively or non-intrusively.
struct Car {
  BOOST_HANA_DEFINE_STRUCT(Car,
    (std::string, brand),
    (std::string, model)
  );
};

int main() {
  // 5. Generate beautiful JSON without hassle. Enjoy!
  auto cars = make_tuple(
    Car{"BMW", "Z3"},
    Car{"Audi", "A4"},
    Car{"Ferrari", "F40"},
    Car{"Lamborghini", "Diablo"}
    // ...
  );

  std::cout << to_json(cars) << std::endl;
}
