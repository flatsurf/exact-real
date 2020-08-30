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

namespace exactreal::test {

TEST_CASE("Random Real", "[real_number]") {
  auto rnd = RealNumber::random();

  SECTION("Equality") {
    REQUIRE(*rnd == *rnd);
    REQUIRE(*rnd != *RealNumber::random());
  }

  SECTION("Conversion to Double") {
    REQUIRE(static_cast<double>(*rnd) == static_cast<double>(*rnd));
    REQUIRE(static_cast<double>(*rnd) >= 0.);
    REQUIRE(static_cast<double>(*rnd) <= 1.);
  }

  SECTION("Conversion to Arf") {
    testArf(RealNumber::random());
  }

  SECTION("Refine") {
    for (unsigned int prec = 1; prec <= 1024; prec *= 2) {
      Arb a = Arb::zero_pm_inf();

      rnd->refine(a, prec);
      REQUIRE(arb_rel_accuracy_bits(a.arb_t()) == prec);

      rnd->refine(a, prec / 2);
      REQUIRE(arb_rel_accuracy_bits(a.arb_t()) == prec);

      REQUIRE(rnd->cmp(a) == 0);
    }
  }

  SECTION("Comparison") {
    auto rnd0 = RealNumber::random();
    auto rnd1 = RealNumber::random();
    REQUIRE(*rnd0 != *rnd1);
    REQUIRE(*rnd0 == *rnd0);
    REQUIRE(*rnd1 == *rnd1);

    REQUIRE((*rnd0 < *rnd1) != (*rnd0 > *rnd1));
    if (*rnd0 > *rnd1) {
      return;
    }

    REQUIRE(*rnd0 < *rnd1);
    REQUIRE(*rnd1 > *rnd0);
  }
}

}  // namespace exactreal::test
