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

// Include this file once at the end of your .test.cc file to get a main that
// runs unit tests and benchmarks.

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

	int result = RUN_ALL_TESTS();

	if (result) {
		return result;
	}

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();

  return result;
}
