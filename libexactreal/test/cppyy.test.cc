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

#include "../exact-real/cppyy.hpp"
#include "../exact-real/yap/arb.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

TEST_CASE("Test cppyy's C++ interface to Arb", "[arb][cppyy]") {
  Arb x(1);
  auto y = x + x;
  Arb z = exactreal::cppyy::eval(std::move(y), 10);
  REQUIRE(z.equal(Arb(2)));
}

}  // namespace exactreal::test
