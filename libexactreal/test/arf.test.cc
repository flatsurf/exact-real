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

#include <boost/lexical_cast.hpp>

#include "external/catch2/single_include/catch2/catch.hpp"

#include "../exact-real/arf.hpp"

using boost::lexical_cast;
using std::string;

namespace exactreal {
TEST(ArbTest, CreateDestroy) { delete new Arf(); }

TEST(ArbTest, Relations) {
  Arf x(-1), y(1);

  ASSERT_TRUE((x < y));
  ASSERT_TRUE((y > x));
  ASSERT_TRUE((x <= y));
  ASSERT_TRUE((y >= x));
  ASSERT_TRUE((x == x));
  ASSERT_TRUE((x != y));
  ASSERT_TRUE(!(y < x));
  ASSERT_TRUE(!(x > y));
  ASSERT_TRUE(!(y <= x));
  ASSERT_TRUE(!(x >= y));
  ASSERT_TRUE(!(x == y));
  ASSERT_TRUE(!(x != x));
}

TEST(ArbTest, Printing) {
  EXPECT_EQ(lexical_cast<string>(Arf()), "0");
  EXPECT_EQ(lexical_cast<string>(Arf(1337)), "1337");
  EXPECT_EQ(lexical_cast<string>(Arf(13.37)), "13.37=7526640877242941p-49");
}
}  // namespace exactreal

#include "main.hpp"
