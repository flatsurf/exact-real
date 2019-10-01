/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include <exact-real/arb.hpp>
#include <exact-real/arf.hpp>
#include <exact-real/real_number.hpp>

#include "arf.test.hpp"

namespace exactreal {
TEST(ProductRealNumberTest, Equality) {
  for (auto da : {.424554, 13.37, .012345, 1.00112233}) {
    for (auto db : {.910621, 1.020304, 0.5}) {
      auto a = RealNumber::random(da);
      auto b = RealNumber::random(db);
      auto a2 = *a * (*a);
      auto b2 = *b * (*b);
      auto ab = *a * (*b);
      auto a2b2 = *ab * (*ab);

      ASSERT_EQ(*ab, *(*b * (*a)));
      ASSERT_NE(*ab, *a2);
      ASSERT_NE(*a, *a2);
      ASSERT_EQ(*a2b2, *(*a2 * (*b2)));
      ASSERT_NE(*a2b2, *ab);
    }
  }
}

TEST(ProductRealNumberTest, arf) {
  for (auto da : {.424554, 13.37, .012345, 1.00112233}) {
    for (auto db : {.910621, 1.020304, 0.5}) {
      auto a = RealNumber::random(da);
      auto b = RealNumber::random(db);
      auto a2 = *a * (*a);
      auto b2 = *b * (*b);
      auto ab = *a * (*b);
      auto a2b2 = *ab * (*ab);

      testArf(a);
      testArf(b);
      testArf(a2);
      testArf(b2);
      testArf(ab);
      testArf(a2b2);
    }
  }
}
}  // namespace exactreal

#include "main.hpp"
