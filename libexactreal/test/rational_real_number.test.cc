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
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

TEST_CASE("Rational Real Number", "[real_number][rational]") {
  SECTION("Equality") {
    std::shared_ptr<const RealNumber> numbers[]{RealNumber::rational(0), RealNumber::rational(1),
                                                RealNumber::rational(mpq_class(1, 2)), RealNumber::rational(mpq_class(4, 3))};

    const size_t length = sizeof(numbers) / sizeof(numbers[0]);
    for (size_t i = 0; i < length; i++) {
      for (size_t j = 0; j < length; j++) {
        if (i == j) {
          REQUIRE(*numbers[i] == *numbers[j]);
        } else {
          REQUIRE(*numbers[i] != *numbers[j]);
        }
      }
    }
  }

  SECTION("Comparison") {
    std::shared_ptr<const RealNumber> numbers[]{RealNumber::rational(0), RealNumber::rational(mpq_class(1, 2)),
                                                RealNumber::rational(1), RealNumber::rational(mpq_class(4, 3))};

    const size_t length = sizeof(numbers) / sizeof(numbers[0]);
    for (size_t i = 0; i < length; i++) {
      for (size_t j = 0; j < length; j++) {
        if (i == j) {
          REQUIRE(*numbers[i] <= *numbers[j]);
          REQUIRE(*numbers[i] >= *numbers[j]);
        } else if (i < j) {
          REQUIRE(*numbers[i] < *numbers[j]);
        } else {
          REQUIRE(*numbers[i] > *numbers[j]);
        }
      }
    }
  }
}

}  // namespace exactreal::test
