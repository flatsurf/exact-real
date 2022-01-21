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
  auto& x = *gens[1];
  auto& y = *gens[2];

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

  SECTION("Products of the same Total Degree are Ordered Lexicographically") {
    // This test assumes that x < y (using knowledge about the internal ordering of elements.)
    REQUIRE(x.deglex(y));
    REQUIRE(xx.deglex(xy));
    REQUIRE(xx.deglex(yy));
    REQUIRE(xy.deglex(yy));
  }
}

TEST_CASE("Division of Real Numbers", "[real_number][truediv]") {
  const auto one = RealNumber::rational(1);
  const auto x = RealNumber::random();
  const auto y = RealNumber::random();

  const auto x2 = *x**x;
  const auto xy = *x**y;
  const auto y2 = *y**y;
  const auto x3 = *x**x2;
  const auto x2y = *x2**y;
  const auto xy2 = *x**y2;
  const auto y3 = *y**y2;

  const auto CHECK_NO_DIVISION = [](const auto& divident, const auto& divisor) {
    CAPTURE(*divident, *divisor);

    const auto quotient = *divident / *divisor;

    REQUIRE(!quotient);
  };

  const auto CHECK_DIVISION = [](const auto& divident, const auto& divisor, const auto& expected) {
    CAPTURE(*divident, *divisor, *expected);

    const auto quotient = *divident / *divisor;

    REQUIRE(quotient);

    CAPTURE(**quotient);

    REQUIRE(*quotient == expected);
  };

  CAPTURE(one);
  CHECK_DIVISION(one, one, one);
  CHECK_NO_DIVISION(one, x);
  CHECK_NO_DIVISION(one, y);
  CHECK_NO_DIVISION(one, x2);
  CHECK_NO_DIVISION(one, xy);
  CHECK_NO_DIVISION(one, y2);
  CHECK_NO_DIVISION(one, x3);
  CHECK_NO_DIVISION(one, x2y);
  CHECK_NO_DIVISION(one, xy2);
  CHECK_NO_DIVISION(one, y3);

  CAPTURE(x);
  CHECK_DIVISION(x, one, x);
  CHECK_DIVISION(x, x, one);
  CHECK_NO_DIVISION(x, y);
  CHECK_NO_DIVISION(x, x2);
  CHECK_NO_DIVISION(x, xy);
  CHECK_NO_DIVISION(x, y2);
  CHECK_NO_DIVISION(x, x3);
  CHECK_NO_DIVISION(x, x2y);
  CHECK_NO_DIVISION(x, xy2);
  CHECK_NO_DIVISION(x, y3);

  CAPTURE(y);
  CHECK_DIVISION(y, one, y);
  CHECK_NO_DIVISION(y, x);
  CHECK_DIVISION(y, y, one);
  CHECK_NO_DIVISION(y, x2);
  CHECK_NO_DIVISION(y, xy);
  CHECK_NO_DIVISION(y, y2);
  CHECK_NO_DIVISION(y, x3);
  CHECK_NO_DIVISION(y, x2y);
  CHECK_NO_DIVISION(y, xy2);
  CHECK_NO_DIVISION(y, y3);

  CAPTURE(x2);
  CHECK_DIVISION(x2, one, x2);
  CHECK_DIVISION(x2, x, x);
  CHECK_NO_DIVISION(x2, y);
  CHECK_DIVISION(x2, x2, one);
  CHECK_NO_DIVISION(x2, xy);
  CHECK_NO_DIVISION(x2, y2);
  CHECK_NO_DIVISION(x2, x3);
  CHECK_NO_DIVISION(x2, x2y);
  CHECK_NO_DIVISION(x2, xy2);
  CHECK_NO_DIVISION(x2, y3);

  CAPTURE(xy);
  CHECK_DIVISION(xy, one, xy);
  CHECK_DIVISION(xy, x, y);
  CHECK_DIVISION(xy, y, x);
  CHECK_NO_DIVISION(xy, x2);
  CHECK_DIVISION(xy, xy, one);
  CHECK_NO_DIVISION(xy, y2);
  CHECK_NO_DIVISION(xy, x3);
  CHECK_NO_DIVISION(xy, x2y);
  CHECK_NO_DIVISION(xy, xy2);
  CHECK_NO_DIVISION(xy, y3);

  CAPTURE(y2);
  CHECK_DIVISION(y2, one, y2);
  CHECK_NO_DIVISION(y2, x);
  CHECK_DIVISION(y2, y, y);
  CHECK_NO_DIVISION(y2, x2);
  CHECK_NO_DIVISION(y2, xy);
  CHECK_DIVISION(y2, y2, one);
  CHECK_NO_DIVISION(y2, x3);
  CHECK_NO_DIVISION(y2, x2y);
  CHECK_NO_DIVISION(y2, xy2);
  CHECK_NO_DIVISION(y2, y3);

  CAPTURE(x3);
  CHECK_DIVISION(x3, one, x3);
  CHECK_DIVISION(x3, x, x2);
  CHECK_NO_DIVISION(x3, y);
  CHECK_DIVISION(x3, x2, x);
  CHECK_NO_DIVISION(x3, xy);
  CHECK_NO_DIVISION(x3, y2);
  CHECK_DIVISION(x3, x3, one);
  CHECK_NO_DIVISION(x3, x2y);
  CHECK_NO_DIVISION(x3, xy2);
  CHECK_NO_DIVISION(x3, y3);

  CAPTURE(x2y);
  CHECK_DIVISION(x2y, one, x2y);
  CHECK_DIVISION(x2y, x, xy);
  CHECK_DIVISION(x2y, y, x2);
  CHECK_DIVISION(x2y, x2, y);
  CHECK_DIVISION(x2y, xy, x);
  CHECK_NO_DIVISION(x2y, y2);
  CHECK_NO_DIVISION(x2y, x3);
  CHECK_DIVISION(x2y, x2y, one);
  CHECK_NO_DIVISION(x2y, xy2);
  CHECK_NO_DIVISION(x2y, y3);

  CAPTURE(xy2);
  CHECK_DIVISION(xy2, one, xy2);
  CHECK_DIVISION(xy2, x, y2);
  CHECK_DIVISION(xy2, y, xy);
  CHECK_NO_DIVISION(xy2, x2);
  CHECK_DIVISION(xy2, xy, y);
  CHECK_DIVISION(xy2, y2, x);
  CHECK_NO_DIVISION(xy2, x3);
  CHECK_NO_DIVISION(xy2, x2y);
  CHECK_DIVISION(xy2, xy2, one);
  CHECK_NO_DIVISION(xy2, y3);

  CAPTURE(y3);
  CHECK_DIVISION(y3, one, y3);
  CHECK_NO_DIVISION(y3, x);
  CHECK_DIVISION(y3, y, y2);
  CHECK_NO_DIVISION(y3, x2);
  CHECK_NO_DIVISION(y3, xy);
  CHECK_DIVISION(y3, y2, y);
  CHECK_NO_DIVISION(y3, x3);
  CHECK_NO_DIVISION(y3, x2y);
  CHECK_NO_DIVISION(y3, xy2);
  CHECK_DIVISION(y3, y3, one);
}

}  // namespace exactreal::test
