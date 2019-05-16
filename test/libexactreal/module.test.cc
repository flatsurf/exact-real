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

#include <exact-real/element.hpp>
#include <exact-real/integer_ring_traits.hpp>
#include <exact-real/module.hpp>
#include <exact-real/real_number.hpp>

using namespace exactreal;
using boost::lexical_cast;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

TEST(ModuleZZ, Create) {
  auto trivial = Module<IntegerRingTraits>::make();
  EXPECT_EQ(trivial->rank(), 0);
  EXPECT_EQ(lexical_cast<string>(*trivial), "ℤ-Module()");

  auto m = Module<IntegerRingTraits>::make({RealNumber::random(), RealNumber::random()});
  EXPECT_EQ(m->rank(), 2);
  EXPECT_EQ(lexical_cast<string>(*m), "ℤ-Module(ℝ(0.303644…), ℝ(0.120809…))");
}

TEST(ModuleZZ, Multiplication) {
  auto m = Module<IntegerRingTraits>::make({RealNumber::rational(1), RealNumber::random()});
  auto n = Module<IntegerRingTraits>::make({RealNumber::rational(1)});

  auto one = n->gen(0);
  auto rnd = m->gen(1);

  EXPECT_EQ((rnd * one).module()->rank(), 2);
  EXPECT_EQ((rnd * rnd).module()->rank(), 3);
  EXPECT_EQ((rnd * rnd * rnd).module()->rank(), 4);
}

#include "main.hpp"
