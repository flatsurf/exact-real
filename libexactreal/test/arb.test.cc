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
#include <boost/lexical_cast.hpp>

#include <exact-real/arb.hpp>

using boost::lexical_cast;
using std::string;

namespace exactreal {
TEST(ArbTest, CreateDestroy) { delete new Arb(); }

TEST(ArbTest, Relations) {
  Arb x(-1), y(1);

  ASSERT_TRUE((x < y) && *(x < y));
  ASSERT_TRUE((y > x) && *(y > x));
  ASSERT_TRUE((x <= y) && *(x <= y));
  ASSERT_TRUE((y >= x) && *(y >= x));
  ASSERT_TRUE((x == x) && *(x == x));
  ASSERT_TRUE((x != y) && *(x != y));
  ASSERT_TRUE((y > x) && !*(y < x));
  ASSERT_TRUE((x < y) && !*(x > y));
  ASSERT_TRUE((y >= x) && !*(y <= x));
  ASSERT_TRUE((x <= y) && !*(x >= y));
}

TEST(ArbTest, UnaryMinus) {
  Arb x(1);

  ASSERT_TRUE(x == -(-x) && *(x == -(-x)));
  ASSERT_TRUE(x == -x && !*(x == -x));
}

TEST(ArbTest, IsExact) {
  ASSERT_TRUE(Arb(mpq_class(1, 2), 2).is_exact());
  ASSERT_FALSE(Arb(mpq_class(1, 3), 2).is_exact());
}

TEST(ArbTest, Printing) {
  EXPECT_EQ(lexical_cast<string>(Arb()), "0");
  EXPECT_EQ(lexical_cast<string>(Arb(1337)), "1337.00");
  EXPECT_EQ(lexical_cast<string>(Arb(string(1337, '1'), 1024)), "[1.11111e+1336 +/- 1.12e+1330]");
  EXPECT_EQ(lexical_cast<string>(Arb("." + string(1337, '0') + "1", 1024)), "[1.00000e-1338 +/- 3e-1348]");
  ASSERT_EQ(lexical_cast<string>(Arb(mpq_class(1, 2), 1)), "0.500000");
  ASSERT_EQ(lexical_cast<string>(Arb(mpq_class(1, 3), 64)), "[0.333333 +/- 3.34e-7]");
}
}  // namespace exactreal

#include "main.hpp"
