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

#include <boost/lexical_cast.hpp>

#include "../exact-real/arf.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

using boost::lexical_cast;
using std::string;

namespace exactreal::test {

TEST_CASE("Create & Destroy Arf", "[arf]") { delete new Arf(); }

TEST_CASE("Relations of Arf", "[arf]") {
  Arf x(-1), y(1);

  REQUIRE(x < y);
  REQUIRE(y > x);
  REQUIRE(x <= y);
  REQUIRE(y >= x);
  REQUIRE(x == x);
  REQUIRE(x != y);
  REQUIRE(!(y < x));
  REQUIRE(!(x > y));
  REQUIRE(!(y <= x));
  REQUIRE(!(x >= y));
  REQUIRE(!(x == y));
  REQUIRE(!(x != x));
}

TEST_CASE("Printing of Arf", "[arf]") {
  REQUIRE(lexical_cast<string>(Arf()) == "0");
  REQUIRE(lexical_cast<string>(Arf(1337)) == "1337");
  REQUIRE(lexical_cast<string>(Arf(13.37)) == "13.37=7526640877242941p-49");
}

TEST_CASE("Floor & Ceil", "[arf]") {
  REQUIRE(Arf(.4).floor() == 0);
  REQUIRE(Arf(.4).ceil() == 1);
  REQUIRE(Arf(.6).floor() == 0);
  REQUIRE(Arf(.6).ceil() == 1);
}

}  // namespace exactreal::test
