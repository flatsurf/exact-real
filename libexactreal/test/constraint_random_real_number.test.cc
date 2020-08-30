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

#include "../exact-real/arb.hpp"
#include "../exact-real/arf.hpp"
#include "../exact-real/real_number.hpp"
#include "arf.test.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

using std::nextafter;
using std::numeric_limits;
using std::swap;
using std::vector;

namespace exactreal::test {

TEST_CASE("Random Number in Interval", "[real_number][interval]") {
  SECTION("Equality") {
    auto rnd = RealNumber::random(Arf(-13l), Arf(37l));
    REQUIRE(*rnd == *rnd);
    REQUIRE(*rnd != *RealNumber::random());
    REQUIRE(*rnd != *RealNumber::random(Arf(-13l), Arf(37l)));
  }

  SECTION("Lies in Given Interval") {
    auto testIntervalToDouble = [](double a, double b) {
      auto rnd = RealNumber::random(Arf(a), Arf(b));
      REQUIRE(static_cast<double>(*rnd) <= b);
      REQUIRE(static_cast<double>(*rnd) >= a);
    };

    testIntervalToDouble(-1l, 1l);
    testIntervalToDouble(0, 1e-37);
    testIntervalToDouble(-13e37, 13e37);
    testIntervalToDouble(0, 1e37);
    testIntervalToDouble(1e-37, 1e37);
  }

  SECTION("Correct Arf Approximation") {
    testArf(RealNumber::random(Arf(-1), Arf(1)));
    testArf(RealNumber::random(Arf(-13e-37), Arf(13e37)));
    testArf(RealNumber::random(Arf(-13e-37), Arf(13e-37)));
    testArf(RealNumber::random(Arf(13e37), Arf(13e38)));
    testArf(RealNumber::random(Arf(-13e-37), Arf(0)));
    testArf(RealNumber::random(Arf(0), Arf(13e37)));
  }
}

TEST_CASE("Random Number from Double", "[real_number][double]") {
  SECTION("Equality") {
    auto rnd = RealNumber::random(13.37);
    REQUIRE(*rnd == *rnd);
    REQUIRE(*rnd != *RealNumber::random());
    REQUIRE(*rnd != *RealNumber::random(13.37));
  }

  SECTION("Can Recover Seed (requires IEEE compliance)") {
    auto testDoubleToDouble = [](double x, double direction) {
      for (size_t i = 0; i < 1024; i++) {
        auto rnd = RealNumber::random(x);
        CAPTURE(*rnd);
        CAPTURE(static_cast<double>(*rnd) - x);
        REQUIRE(static_cast<double>(*rnd) == x);
        x = nextafter(x, direction);
      }
    };

    testDoubleToDouble(0., numeric_limits<double>::infinity());
    testDoubleToDouble(0., -numeric_limits<double>::infinity());
    testDoubleToDouble(13e37, numeric_limits<double>::infinity());
    testDoubleToDouble(13e37, -numeric_limits<double>::infinity());
    testDoubleToDouble(-1337, numeric_limits<double>::infinity());
    testDoubleToDouble(-1337, -numeric_limits<double>::infinity());
  }

  SECTION("Correct Arf Approximation") {
    testArf(RealNumber::random(0.));
    testArf(RealNumber::random(13.37));
    testArf(RealNumber::random(13e37));
    testArf(RealNumber::random(13e-37));
    testArf(RealNumber::random(1));
    testArf(RealNumber::random(-1));
  }
}

}  // namespace exactreal::test
