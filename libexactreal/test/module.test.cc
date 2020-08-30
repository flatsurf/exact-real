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

#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

using boost::lexical_cast;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

namespace exactreal::test {

TEST_CASE("Module over ZZ", "[module][integer_ring]") {
  SECTION("Rank") {
    auto trivial = Module<IntegerRing>::make({});
    REQUIRE(trivial->rank() == 0);
    REQUIRE(lexical_cast<string>(*trivial) == "ℤ-Module()");

    auto m = Module<IntegerRing>::make({RealNumber::random(), RealNumber::random()});
    REQUIRE(m->rank() == 2);
    REQUIRE(lexical_cast<string>(*m) == "ℤ-Module(ℝ(0.120809…), ℝ(0.303644…))");

    auto n = Module<IntegerRing>::make(m->basis());
    REQUIRE(m == n);
  }

  SECTION("Multiplication") {
    auto m = Module<IntegerRing>::make({RealNumber::random(), RealNumber::rational(1)});
    auto n = Module<IntegerRing>::make({RealNumber::rational(1)});

    auto one = n->gen(0);
    auto rnd = m->gen(0);

    REQUIRE((rnd * one).module()->rank() == 2);
    // This might not hold, as the order of the generators in the two modules might be different
    // REQUIRE((rnd * one).module() == rnd.module());
    REQUIRE((rnd * one).module() == (one * rnd).module());
    REQUIRE((rnd * rnd).module()->rank() == 3);
    REQUIRE((rnd * rnd).module() == (rnd * rnd).module());
    REQUIRE((rnd * rnd * rnd).module()->rank() == 4);
    REQUIRE((rnd * rnd * rnd).module() != (rnd * rnd).module());
  }
}

}  // namespace exactreal::test
