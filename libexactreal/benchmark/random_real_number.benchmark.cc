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

#include <benchmark/benchmark.h>

#include <memory>

#include "../exact-real/arf.hpp"
#include "../exact-real/real_number.hpp"

namespace exactreal::test {

struct RandomRealNumberFixture : benchmark::Fixture {
  std::shared_ptr<const RealNumber> rnd = RealNumber::random();
};

BENCHMARK_F(RandomRealNumberFixture, Double)
(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(static_cast<double>(*rnd));
  }
}

BENCHMARK_DEFINE_F(RandomRealNumberFixture, arf)
(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(rnd->arf(static_cast<unsigned int>(state.range(0))));
  }
}

BENCHMARK_REGISTER_F(RandomRealNumberFixture, arf)->Range(16, 1 << 16);

}  // namespace exactreal::test
