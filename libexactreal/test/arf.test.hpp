/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#ifndef LIBEXACTREAL_TEST_ARF_HPP
#define LIBEXACTREAL_TEST_ARF_HPP

#include <limits>
#include <memory>

#include "../exact-real/arb.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/yap/arf.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

inline Arf absoluteError(const Arf& expected, const Arf& actual) {
  return Arf((expected - actual)(ARF_PREC_EXACT, Arf::Round::NEAR)).abs();
}

inline Arf relativeError(const Arf& expected, const Arf& actual) {
  if (expected == 0) {
    throw std::logic_error("relative error not defined for exact zero");
  }

  return (absoluteError(expected, actual) / expected.abs())(64, Arf::Round::NEAR);
}

inline long absoluteAccuracy(const Arf& expected, const Arf& actual) {
  return Arf((1 / absoluteError(expected, actual))(64, Arf::Round::NEAR)).logb();
}

inline long relativeAccuracy(const Arf& expected, const Arf& actual) {
  const auto relError = relativeError(expected, actual);
  if (relError == 0)
    return std::numeric_limits<long>::max();

  return Arf((1 / relError)(64, Arf::Round::NEAR)).logb();
}

inline void testArf(std::shared_ptr<const RealNumber> x) {
  if (!*x) {
    return;
  }

  // As it simplifies debugging quite a bit, we expect digits that we did not ask for to be 0.
  CHECK(abs(x->arf(0).mantissa()) == 1);

  const long PREC = 4096;

  Arf best = x->arf(PREC);

  // Check that things are sane for some random precisions
  for (auto prec : {1, 2, 3, 4, 7, 12, 32, 44, 64, 77, 114, 269, 281, 358, 411, 418,
                    476, 517, 614, 636, 684, 698, 756, 768, 769, 771, 810, 881, 925, 955, 1007, 1041,
                    1091, 1114, 1145, 1345, 1379, 1507, 1540, 1561, 1637, 1713, 1715, 1723, 1734, 1762, 1778, 1875,
                    1909, 1914, 1920, 1926, 1945, 2005, 2021, 2062, 2097, 2112, 2128, 2137, 2169, 2193, 2223, 2456,
                    2465, 2472, 2482, 2505, 2584, 2660, 2674, 2695, 2700, 2706, 2712, 2719, 2741, 2764, 2785, 2931,
                    2986, 3005, 3035, 3086, 3116, 3134, 3160, 3189, 3222, 3313, 3357, 3383, 3412, 3511, 3527, 3545,
                    3658, 3750, 3763, 3763, 3793, 3814, 3830, 3861, 3869, 3895, 3927, 3971, 4057}) {
    Arf approx = x->arf(prec);

    REQUIRE(approx == x->arf(prec));

    // As we are assuming that digits that we did not ask for are 0, approx can
    // have at most prec + 1 digits (as trailing zeros are not counted.)
    REQUIRE(arf_bits(approx.arf_t()) <= prec + 1);

    REQUIRE(!arf_is_zero(approx.arf_t()));

    CAPTURE(*x);
    CAPTURE(prec);
    CAPTURE(approx);
    CAPTURE(absoluteError(best, approx));
    CAPTURE(relativeError(best, approx));

    // Check that approx actually has prec bits of precision.
    REQUIRE(relativeAccuracy(best, approx) >= prec);
  }
}

}  // namespace exactreal::test

// Force creation of code for easier debugging in gdb; so we can call operator*.
template class std::shared_ptr<const exactreal::RealNumber>;

#endif
