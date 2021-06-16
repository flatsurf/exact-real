/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  exact-real is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#include <e-antic/renfxx.h>

#include <boost/lexical_cast.hpp>
#include <cereal/archives/json.hpp>
#include <e-antic/cereal.hpp>

#include "../exact-real/cereal.hpp"
#include "../exact-real/real_number.hpp"
#include "arb.hpp"
#include "arf.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

using cereal::JSONInputArchive;
using cereal::JSONOutputArchive;

namespace exactreal::test {

template <typename T>
struct is_shared_ptr : std::false_type {};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template <typename T>
struct is_vector : std::false_type {};
template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};
template <typename T>
std::string toString(const T& x) {
  if constexpr (is_shared_ptr<T>::value) {
    if (x)
      return boost::lexical_cast<std::string>(&*x) + "=&" + boost::lexical_cast<std::string>(*x);
    else
      return "null";
  } else if constexpr (is_vector<T>::value) {
    return "[…]";
  } else {
    return boost::lexical_cast<std::string>(x);
  }
}

template <typename T>
T test_serialization(const T& x) {
  std::stringstream s;

  {
    JSONOutputArchive archive(s);
    archive(cereal::make_nvp("test", x));
  }

  T y;
  {
    JSONInputArchive archive(s);
    archive(cereal::make_nvp("test", y));
  }

  if constexpr (std::is_same_v<T, Arb>) {
    if (x.equal(y)) return y;
  } else {
    if (x == y) return y;
  }
  if constexpr (is_shared_ptr<T>::value) {
    if (!x && !y) return y;
    if (x && y && *x == *y) return y;
  }
  throw std::runtime_error("deserialization failed to reconstruct element, the original value " + toString(x) + " had serialized to " + s.str() + " which deserialized to " + toString(y));
}

TEST_CASE("Serialization of Arb", "[cereal][arb]") {
  ArbTester arbs;
  for (int i = 0; i < 1024; i++) {
    // Valgrind might report: Source and destination overlap in memcpy_chk.
    // This can be safely ignored: cereal's double conversion is calling
    // std::memmove which seems to intercepted incorrectly by valgrind.
    // (memmove can handle overlapping addresses.)
    test_serialization(arbs.random());
  }
}

TEST_CASE("Serialization of Arf", "[cereal][arf]") {
  ArfTester arfs;
  for (int i = 0; i < 1024; i++) {
    // Valgrind might report: Source and destination overlap in memcpy_chk.
    // This can be safely ignored: cereal's double conversion is calling
    // std::memmove which seems to intercepted incorrectly by valgrind.
    // (memmove can handle overlapping addresses.)
    test_serialization(arfs.random());
  }
}

TEST_CASE("Serialization of RealNumber", "[cereal][real_number]") {
  auto rnd = RealNumber::random();
  test_serialization(rnd);

  rnd = (*rnd) * (*rnd);
  test_serialization(rnd);

  rnd = RealNumber::rational(mpq_class(13, 37));
  test_serialization(rnd);

  rnd = RealNumber::random(13.37);
  test_serialization(rnd);
}

TEMPLATE_TEST_CASE("Serialization of Module", "[cereal][module]", (IntegerRing), (RationalField), (NumberField)) {
  auto trivial = Module<TestType>::make({});
  test_serialization(trivial);

  auto m = Module<TestType>::make({RealNumber::random(), RealNumber::random()});
  test_serialization(m);
}

TEMPLATE_TEST_CASE("Serialization of Element", "[cereal][element]", (IntegerRing), (RationalField), (NumberField)) {
  SECTION("Elements in a trivial module") {
    auto m = Module<TestType>::make({});

    test_serialization(m->zero());
    test_serialization(std::vector{m->zero(), m->zero()});
  }

  SECTION("Elements in a non-trivial module") {
    auto m = Module<TestType>::make({RealNumber::rational(1), RealNumber::random()});

    test_serialization(m->gen(1));
    test_serialization(m->zero());
  }
}

}  // namespace exactreal::test
