/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#include "../exact-real/arb.hpp"
#include "../exact-real/arf.hpp"
#include "../exact-real/real_number.hpp"
#include "arf.test.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

TEST_CASE("Product of Real Numbers", "[real_number][product]") {
  const auto a = GENERATE(RealNumber::rational(1), RealNumber::rational(mpq_class{2, 3}), RealNumber::random(.424554), RealNumber::random(13.37), RealNumber::random(.012345), RealNumber::random(1.00112233), RealNumber::random(-1.00112233));
  const auto b = GENERATE(RealNumber::rational(1), RealNumber::rational(mpq_class{-3, 2}), RealNumber::random(.910621), RealNumber::random(1.020304), RealNumber::random(0.5));

  CAPTURE(*a);
  CAPTURE(*b);

  auto a2 = *a * (*a);
  auto b2 = *b * (*b);

  testArf(a);
  testArf(b);
  testArf(a2);
  testArf(b2);

  if ((static_cast<std::optional<mpq_class>>(*a) && *a != 1 && !static_cast<std::optional<mpq_class>>(*b))
   || (static_cast<std::optional<mpq_class>>(*b) && *b != 1 && !static_cast<std::optional<mpq_class>>(*a))) {
    // Multiplication with rationals != 1 is not implemented.
    REQUIRE_THROWS(*a * (*b));
    return;
  }

  auto ab = *a * (*b);
  auto a2b2 = *ab * (*ab);

  REQUIRE(*ab == *(*b * (*a)));

  if (*a != *b && *a != 1)
    REQUIRE(*ab != *a2);

  if (*a != 1)
    REQUIRE(*a != *a2);

  REQUIRE(*a2b2 == *(*a2 * (*b2)));

  if (*ab != 1)
    REQUIRE(*a2b2 != *ab);

  testArf(ab);
  testArf(a2b2);
}

TEST_CASE("DegLex Order of Real Numbers", "[real_number][product][deglex]") {
  std::shared_ptr<const RealNumber> gens[]{RealNumber::rational(1), RealNumber::random(), RealNumber::random()};

  auto& one = *gens[0];
  auto& x = *gens[1] < *gens[2] ? *gens[1] : *gens[2];
  auto& y = *gens[1] < *gens[2] ? *gens[2] : *gens[1];

  std::shared_ptr<const RealNumber> products[] = {x * x, x * y, y * y};

  auto& xx = *products[0];
  auto& xy = *products[1];
  auto& yy = *products[2];

  SECTION("Rationals correspond to Constant Polynomials") {
    REQUIRE(!one.deglex(one));
    REQUIRE(!one.deglex(*RealNumber::rational(2)));
    REQUIRE(one.deglex(x));
    REQUIRE(one.deglex(xy));
  }

  SECTION("Products are Ordered by Total Degree") {
    REQUIRE(x.deglex(xx));
    REQUIRE(y.deglex(xx));
    REQUIRE(xx.deglex(*(x * xx)));
    REQUIRE(xx.deglex(*(x * xy)));
    REQUIRE(xx.deglex(*(x * yy)));
  }

  SECTION("Indeterminates are Ordered by Value") {
    REQUIRE(x.deglex(y));
  }

  SECTION("Products of the same Total Degree are Ordered Lexicographically") {
    REQUIRE(x.deglex(y));
    REQUIRE(xx.deglex(xy));
    REQUIRE(xx.deglex(yy));
    REQUIRE(xy.deglex(yy));
  }
}

}  // namespace exactreal::test
