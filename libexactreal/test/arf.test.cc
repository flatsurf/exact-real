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

namespace exactreal::test {

TEST_CASE("Create & Destroy Arf", "[arf]") { delete new Arf(); }

TEST_CASE("Initialization from Integer Types", "[arf]") {
  REQUIRE(Arf(static_cast<short>(1)) == Arf(1));
  REQUIRE(Arf(static_cast<unsigned short>(1)) == Arf(1));
  REQUIRE(Arf(1) == Arf(1));
  REQUIRE(Arf(1u) == Arf(1));
  REQUIRE(Arf(1l) == Arf(1));
  REQUIRE(Arf(1ul) == Arf(1));
  REQUIRE(Arf(1ll) == Arf(1));
  REQUIRE(Arf(1ull) == Arf(1));
  REQUIRE(Arf(mpz_class(1)) == Arf(1));

  REQUIRE((Arf() = static_cast<short>(1)) == Arf(1));
  REQUIRE((Arf() = static_cast<unsigned short>(1)) == Arf(1));
  REQUIRE((Arf() = 1) == Arf(1));
  REQUIRE((Arf() = 1u) == Arf(1));
  REQUIRE((Arf() = 1l) == Arf(1));
  REQUIRE((Arf() = 1ul) == Arf(1));
  REQUIRE((Arf() = 1ll) == Arf(1));
  REQUIRE((Arf() = 1ull) == Arf(1));
  REQUIRE((Arf() = mpz_class(1)) == Arf(1));
}

TEST_CASE("Relational Operators of Arf", "[arf]") {
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

  REQUIRE(static_cast<short>(1) < y);
  REQUIRE(y > static_cast<short>(1));
  REQUIRE(static_cast<short>(1) <= y);
  REQUIRE(y >= static_cast<short>(1));
  REQUIRE(static_cast<short>(1) == x);
  REQUIRE(x == static_cast<short>(1));
  REQUIRE(static_cast<short>(1) != y);
  REQUIRE(!(y < static_cast<short>(1)));
  REQUIRE(!(static_cast<short>(1) > y));
  REQUIRE(!(y <= static_cast<short>(1)));
  REQUIRE(!(static_cast<short>(1) >= y));
  REQUIRE(!(static_cast<short>(1) == y));
  REQUIRE(!(static_cast<short>(1) != x));
  REQUIRE(!(x != static_cast<short>(1)));

  REQUIRE(static_cast<unsigned short>(1) < y);
  REQUIRE(y > static_cast<unsigned short>(1));
  REQUIRE(static_cast<unsigned short>(1) <= y);
  REQUIRE(y >= static_cast<unsigned short>(1));
  REQUIRE(static_cast<unsigned short>(1) == x);
  REQUIRE(x == static_cast<unsigned short>(1));
  REQUIRE(static_cast<unsigned short>(1) != y);
  REQUIRE(!(y < static_cast<unsigned short>(1)));
  REQUIRE(!(static_cast<unsigned short>(1) > y));
  REQUIRE(!(y <= static_cast<unsigned short>(1)));
  REQUIRE(!(static_cast<unsigned short>(1) >= y));
  REQUIRE(!(static_cast<unsigned short>(1) == y));
  REQUIRE(!(static_cast<unsigned short>(1) != x));
  REQUIRE(!(x != static_cast<unsigned short>(1)));

  REQUIRE(1 < y);
  REQUIRE(y > 1);
  REQUIRE(1 <= y);
  REQUIRE(y >= 1);
  REQUIRE(1 == x);
  REQUIRE(x == 1);
  REQUIRE(1 != y);
  REQUIRE(!(y < 1));
  REQUIRE(!(1 > y));
  REQUIRE(!(y <= 1));
  REQUIRE(!(1 >= y));
  REQUIRE(!(1 == y));
  REQUIRE(!(1 != x));
  REQUIRE(!(x != 1));

  REQUIRE(1u < y);
  REQUIRE(y > 1u);
  REQUIRE(1u <= y);
  REQUIRE(y >= 1u);
  REQUIRE(1u == x);
  REQUIRE(x == 1u);
  REQUIRE(1u != y);
  REQUIRE(!(y < 1u));
  REQUIRE(!(1u > y));
  REQUIRE(!(y <= 1u));
  REQUIRE(!(1u >= y));
  REQUIRE(!(1u == y));
  REQUIRE(!(x != 1u));

  REQUIRE(1l < y);
  REQUIRE(y > 1l);
  REQUIRE(1l <= y);
  REQUIRE(y >= 1l);
  REQUIRE(1l == x);
  REQUIRE(x == 1l);
  REQUIRE(1l != y);
  REQUIRE(!(y < 1l));
  REQUIRE(!(1l > y));
  REQUIRE(!(y <= 1l));
  REQUIRE(!(1l >= y));
  REQUIRE(!(1l == y));
  REQUIRE(!(x != 1l));

  REQUIRE(1ul < y);
  REQUIRE(y > 1ul);
  REQUIRE(1ul <= y);
  REQUIRE(y >= 1ul);
  REQUIRE(1ul == x);
  REQUIRE(x == 1ul);
  REQUIRE(1ul != y);
  REQUIRE(!(y < 1ul));
  REQUIRE(!(1ul > y));
  REQUIRE(!(y <= 1ul));
  REQUIRE(!(1ul >= y));
  REQUIRE(!(1ul == y));
  REQUIRE(!(1ul != x));
  REQUIRE(!(x != 1ul));

  REQUIRE(1ll < y);
  REQUIRE(y > 1ll);
  REQUIRE(1ll <= y);
  REQUIRE(y >= 1ll);
  REQUIRE(1ll == x);
  REQUIRE(x == 1ll);
  REQUIRE(1ll != y);
  REQUIRE(!(y < 1ll));
  REQUIRE(!(1ll > y));
  REQUIRE(!(y <= 1ll));
  REQUIRE(!(1ll >= y));
  REQUIRE(!(1ll == y));
  REQUIRE(!(1ll != x));
  REQUIRE(!(x != 1ll));

  REQUIRE(1ull < y);
  REQUIRE(y > 1ull);
  REQUIRE(1ull <= y);
  REQUIRE(y >= 1ull);
  REQUIRE(1ull == x);
  REQUIRE(x == 1ull);
  REQUIRE(1ull != y);
  REQUIRE(!(y < 1ull));
  REQUIRE(!(1ull > y));
  REQUIRE(!(y <= 1ull));
  REQUIRE(!(1ull >= y));
  REQUIRE(!(1ull == y));
  REQUIRE(!(1ull != x));
  REQUIRE(!(x != 1ull));

  REQUIRE(mpz_class(1) < y);
  REQUIRE(y > mpz_class(1));
  REQUIRE(mpz_class(1) <= y);
  REQUIRE(y >= mpz_class(1));
  REQUIRE(mpz_class(1) == x);
  REQUIRE(x == mpz_class(1));
  REQUIRE(mpz_class(1) != y);
  REQUIRE(!(y < mpz_class(1)));
  REQUIRE(!(mpz_class(1) > y));
  REQUIRE(!(y <= mpz_class(1)));
  REQUIRE(!(mpz_class(1) >= y));
  REQUIRE(!(mpz_class(1) == y));
  REQUIRE(!(mpz_class(1) != x));
  REQUIRE(!(x != mpz_class(1)));
}

TEST_CASE("Printing of Arf", "[arf]") {
  REQUIRE(boost::lexical_cast<std::string>(Arf()) == "0");
  REQUIRE(boost::lexical_cast<std::string>(Arf(1337)) == "1337");
  REQUIRE(boost::lexical_cast<std::string>(Arf(13.37)) == "13.37=7526640877242941p-49");
}

TEST_CASE("Floor & Ceil", "[arf]") {
  REQUIRE(Arf(.4).floor() == 0);
  REQUIRE(Arf(.4).ceil() == 1);
  REQUIRE(Arf(.6).floor() == 0);
  REQUIRE(Arf(.6).ceil() == 1);
}

TEST_CASE("Hashing", "[arf]") {
  REQUIRE(std::hash<Arf>{}(Arf{}) == std::hash<Arf>{}(Arf{}));
}

}  // namespace exactreal::test
