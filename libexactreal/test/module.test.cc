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

#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"

using boost::lexical_cast;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

namespace exactreal {
TEST(ModuleZZ, Create) {
  auto trivial = Module<IntegerRing>::make({});
  EXPECT_EQ(trivial->rank(), 0);
  EXPECT_EQ(lexical_cast<string>(*trivial), "ℤ-Module()");

  auto m = Module<IntegerRing>::make({RealNumber::random(), RealNumber::random()});
  EXPECT_EQ(m->rank(), 2);
  EXPECT_EQ(lexical_cast<string>(*m), "ℤ-Module(ℝ(0.120809…), ℝ(0.178808…))");

  auto n = Module<IntegerRing>::make(m->basis());
  EXPECT_EQ(m, n);
}

TEST(ModuleZZ, Multiplication) {
  auto m = Module<IntegerRing>::make({RealNumber::random(), RealNumber::rational(1)});
  auto n = Module<IntegerRing>::make({RealNumber::rational(1)});

  auto one = n->gen(0);
  auto rnd = m->gen(0);

  EXPECT_EQ((rnd * one).module()->rank(), 2);
  // This might not hold, as the order of the generators in the two modules might be different
  // EXPECT_EQ((rnd * one).module(), rnd.module());
  EXPECT_EQ((rnd * one).module(), (one * rnd).module());
  EXPECT_EQ((rnd * rnd).module()->rank(), 3);
  EXPECT_EQ((rnd * rnd).module(), (rnd * rnd).module());
  EXPECT_EQ((rnd * rnd * rnd).module()->rank(), 4);
  EXPECT_NE((rnd * rnd * rnd).module(), (rnd * rnd).module());
}
}  // namespace exactreal

#include "main.hpp"
