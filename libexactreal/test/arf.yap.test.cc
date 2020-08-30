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

#include "../exact-real/yap/arf.hpp"
#include "arf.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

ArfTester tester;
const prec working_prec = 64;
const arf_rnd_t rnd_t = ARF_RND_NEAR;
const Arf::Round rnd = Arf::Round::NEAR;

TEST_CASE("Arf Arithmetic with Yap", "[arf][yap]") {
  tester.reset();
  Arf x = tester.random(), y = tester.random(), z, z_;

  z = (x + y)(working_prec, rnd);
  arf_add(z_.arf_t(), x.arf_t(), y.arf_t(), working_prec, rnd_t);
  REQUIRE(z == z_);

  z = (x - y)(working_prec, rnd);
  arf_sub(z_.arf_t(), x.arf_t(), y.arf_t(), working_prec, rnd_t);
  REQUIRE(z == z_);

  z = (x * y)(working_prec, rnd);
  arf_mul(z_.arf_t(), x.arf_t(), y.arf_t(), working_prec, rnd_t);
  REQUIRE(z == z_);

  z = (x / y)(working_prec, rnd);
  arf_div(z_.arf_t(), x.arf_t(), y.arf_t(), working_prec, rnd_t);
  REQUIRE(z == z_);

  z = -x;
  arf_neg(z_.arf_t(), x.arf_t());
  REQUIRE(z == z_);
}

TEST_CASE("Arf Expressions with Yap", "[arf][yap]") {
  Arf x2(2), x3(3), x5(5), x7(7), x11(11);

  // Some random expressions generated with
  // def e():
  //     if random() > .5: return ['x2', 'x3', 'x5', 'x7', 'x11'][randrange(0, 5)]
  //     return "(%s %s %s)"%(e(), "+-*/"[randint(0,3)], e())

  Arf a;
  double d;

  a = ((((x7 / x5) + (x2 - x3)) / x3) + x2)(working_prec, rnd);
  d = ((((7. / 5.) + (2. - 3.)) / 3.) + 2.);
  REQUIRE(static_cast<double>(a) == Approx(d));  // 2.13333

  a = (x2 - (x11 * (x2 / ((x2 / (x5 * ((x7 * x2) * x2))) / x2))))(working_prec, rnd);
  d = 2. - (11. * (2. / ((2. / (5. * ((7. * 2.) * 2.))) / 2.)));
  REQUIRE(static_cast<double>(a) == Approx(d));  // -3078

  a = ((x3 + x11) / ((x5 - x2) + (((((x7 * x2) / (x11 / x5)) * x2) + (x2 - x11)) - (x5 - (x7 + x5)))))(working_prec,
                                                                                                       rnd);
  d = ((3. + 11.) / ((5. - 2.) + (((((7. * 2.) / (11. / 5.)) * 2.) + (2. - 11.)) - (5. - (7. + 5.)))));
  REQUIRE(static_cast<double>(a) == Approx(d));  // 1.01987
}

TEST_CASE("Inplace Arithmetic with Yap", "[arf][yap]") {
  tester.reset();
  Arf x = tester.random(), y, z;

  REQUIRE(((z = Arf(1337)) += x(working_prec, rnd)) == ((Arf(1337) + x)(working_prec, rnd)));
  REQUIRE(((z = Arf(1337)) -= x(working_prec, rnd)) == ((Arf(1337) - x)(working_prec, rnd)));
  REQUIRE(((z = Arf(1337)) *= x(working_prec, rnd)) == ((Arf(1337) * x)(working_prec, rnd)));
  REQUIRE(((z = Arf(1337)) /= x(working_prec, rnd)) == ((Arf(1337) / x)(working_prec, rnd)));
}

}  // namespace exactreal::test
