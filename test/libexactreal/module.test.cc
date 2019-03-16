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

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <exact-real/module.hpp>
#include <exact-real/element.hpp>
#include <exact-real/integer_ring_traits.hpp>

using namespace exactreal;
using std::vector;
using std::make_shared;
using std::shared_ptr;

TEST(ModuleZZ, Create) {
	auto trivial = Module<IntegerRingTraits>({});
	EXPECT_EQ(trivial.rank(), 0);

	auto m = Module<IntegerRingTraits>({ RealNumber::random(), RealNumber::random() });
	EXPECT_EQ(m.rank(), 2);
}

#include "main.hpp"

