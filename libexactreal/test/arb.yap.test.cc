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

#include "../exact-real/yap/arb.hpp"
#include "arb.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

ArbTester tester;

TEST_CASE("Arb Arithmetic with Yap", "[arb][yap]") {
  tester.reset();

  SECTION("Arithmetic") {
    Arb x = tester.random(), y = tester.random(), z, z_;
    const prec prec = 64;

    z = (x + y)(prec);
    arb_add(z_.arb_t(), x.arb_t(), y.arb_t(), prec);
    REQUIRE(z.equal(z_));

    z = (x - y)(prec);
    arb_sub(z_.arb_t(), x.arb_t(), y.arb_t(), prec);
    REQUIRE(z.equal(z_));

    z = (x * y)(prec);
    arb_mul(z_.arb_t(), x.arb_t(), y.arb_t(), prec);
    REQUIRE(z.equal(z_));

    z = (x / y)(prec);
    arb_div(z_.arb_t(), x.arb_t(), y.arb_t(), prec);
    REQUIRE(z.equal(z_));

    z = -x;
    arb_neg(z_.arb_t(), x.arb_t());
    REQUIRE(z.equal(z_));
  }

  SECTION("Evaluate Expressions") {
    Arb x2(2), x3(3), x5(5), x7(7), x11(11);

    // Some random expressions generated with
    // def e():
    //     if random() > .5: return ['x2', 'x3', 'x5', 'x7', 'x11'][randrange(0, 5)]
    //     return "(%s %s %s)"%(e(), "+-*/"[randint(0,3)], e())

    Arb a;
    double d;

    a = ((((x7 / x5) + (x2 - x3)) / x3) + x2)(64);
    d = ((((7. / 5.) + (2. - 3.)) / 3.) + 2.);
    REQUIRE(static_cast<double>(a) == Approx(d));  // 2.13333

    a = (x2 - (x11 * (x2 / ((x2 / (x5 * ((x7 * x2) * x2))) / x2))))(64);
    d = 2. - (11. * (2. / ((2. / (5. * ((7. * 2.) * 2.))) / 2.)));
    REQUIRE(static_cast<double>(a) == Approx(d));  // -3078

    a = ((x3 + x11) / ((x5 - x2) + (((((x7 * x2) / (x11 / x5)) * x2) + (x2 - x11)) - (x5 - (x7 + x5)))))(64);
    d = ((3. + 11.) / ((5. - 2.) + (((((7. * 2.) / (11. / 5.)) * 2.) + (2. - 11.)) - (5. - (7. + 5.)))));
    REQUIRE(static_cast<double>(a) == Approx(d));  // 1.01987

    /*
    // adds one second to the compilation time [2019]
    a = (((x3 + x3) - x11) + (x5 / (((x5 * x3) - x3) / (x5 + ((x2 + (x5 + (((x7 / x11) * x5) - (x3 * (x7 * x3))))) +
    x5)))))(64); d = (((3. + 3.) - 11.) + (5. / (((5. * 3.) - 3.) / (5. + ((2. + (5. + (((7. / 11.) * 5.) - (3. * (7.
    * 3.))))) + 5.)))));
    REQUIRE(static_cast<double>(a) == Approx(d)); // -22.8409

    // adds one second to the compilation time [2019]
    a = (x5 / ((((((x2 / (x7 + ((x11 * x11) - (x7 / x11)))) - x7) * x7) - x11) / x5) + (x2 + (((x3 - (x11 * x11)) + x5) -
    (x3 * ((x11 - (x2 + x7)) - x3))))))(64); d = (5. / ((((((2. / (7. + ((11. * 11.) - (7. / 11.)))) - 7.) * 7.) - 11.)
    / 5.) + (2. + (((3. - (11. * 11.)) + 5.) - (3. * ((11. - (2. + 7.)) - 3.))))));
    REQUIRE(static_cast<double>(a) == Approx(d)); // -0.0416743

    // adds 10s to the compilation time [2019]
    a = (((x7 / (((((x3 + ((x3 / (x7 / ((x11 / (x5 + ((x11 + (x3 * (((x7 * x7) + x5) / x3))) - (x2 * x7)))) / (x3 * (x7 /
    x11))))) + x3)) - x5) / (x3 - (x3 / x2))) * x2) + ((x7 + x7) + ((x5 * ((((x2 * x3) - (x3 * (((x3 + x3) - x3) + x3))) *
    ((x11 - ((x11 / x3) / (x3 * x7))) + x3)) - (x7 * x5))) - x7)))) * (((x2 / ((x5 / x3) + ((x2 / (((x2 / (x2 * x7)) -
    (((x5 * (x3 / x3)) / x7) * ((x11 * (x2 - ((x3 / (x5 / x5)) * ((x5 * x3) - x5)))) / x3))) + ((((x11 / ((((x2 + x2) +
    (x2 / x7)) * x7) * x5)) / x11) + (((((x3 - (x7 * x11)) + x7) / (((x2 * (x5 * ((x11 * x5) + x3))) + x7) + ((x3 / x2) /
    x11))) / x11) / (x2 - x11))) * x3))) / x7))) / x7) + x7)) - ((x2 * x3) + x11))(64); d = (((7. / (((((3. + ((3. / (7. /
    ((11. / (5. + ((11. + (3. * (((7. * 7.) + 5.) / 3.))) - (2. * 7.)))) / (3. * (7. / 11.))))) + 3.)) - 5.) / (3. - (3.
    / 2.))) * 2.) + ((7. + 7.) + ((5. * ((((2. * 3.) - (3. * (((3. + 3.) - 3.) + 3.))) * ((11. - ((11. / 3.) / (3. * 7.)))
    + 3.)) - (7. * 5.))) - 7.)))) * (((2. / ((5. / 3.) + ((2. / (((2. / (2. * 7.)) - (((5. * (3. / 3.)) / 7.) * ((11. *
    (2. - ((3. / (5. / 5.)) * ((5. * 3.) - 5.)))) / 3.))) + ((((11. / ((((2. + 2.) + (2. / 7.)) * 7.) * 5.)) / 11.) +
    (((((3. - (7. * 11.)) + 7.) / (((2. * (5. * ((11. * 5.) + 3.))) + 7.) + ((3. / 2.) / 11.))) / 11.) / (2. - 11.)))
    * 3.))) / 7.))) / 7.) + 7.)) - ((2. * 3.) + 11.));
    REQUIRE(static_cast<double>(a) == Approx(d)); // -17.0504
    */
  }

  SECTION("Inplace Arithmetic") {
    Arb x = tester.random(), y, z;
    const prec prec = 64;

    REQUIRE(((z = Arb(1337)) += x(prec)).equal((Arb(1337) + x)(prec)));
    REQUIRE(((z = Arb(1337)) -= x(prec)).equal((Arb(1337) - x)(prec)));
    REQUIRE(((z = Arb(1337)) *= x(prec)).equal((Arb(1337) * x)(prec)));
    REQUIRE(((z = Arb(1337)) /= x(prec)).equal((Arb(1337) / x)(prec)));
  }
}

}  // namespace exactreal::test
