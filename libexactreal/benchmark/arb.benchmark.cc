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

#include "../exact-real/yap/arb.hpp"
#include "../test/arb.hpp"

namespace exactreal::test {

struct ArbBenchmark : public benchmark::Fixture {
  void SetUp(const benchmark::State& state) override { SetUp(const_cast<benchmark::State&>(state)); }

  void SetUp(benchmark::State&) override { tester.reset(); }

  Arb random(benchmark::State& state) { return tester.random(state.range(0), state.range(1)); }

  static void BenchmarkedSizes(benchmark::internal::Benchmark* b) {
    b->Args({53, 10});
    b->Args({65536, 1024});
  }

  ArbTester tester;
};

BENCHMARK_DEFINE_F(ArbBenchmark, Create)
(benchmark::State& state) {
  for (auto _ : state) {
    // interestingly, this is not optimized away by GCC -O3
    Arb x;
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Create);

BENCHMARK_DEFINE_F(ArbBenchmark, CreateMove)
(benchmark::State& state) {
  flint::frandxx rand;
  Arb x = random(state);
  Arb z = x;
  bool skip = false;

  for (auto _ : state) {
    if (!skip) {
      Arb y(std::move(x));
      x = std::move(y);
    }
    // only run every other loop, so that CreateMove() (which is two
    // operations, Arb(&&) then operator=(&&)) can be easily compared in the
    // output.
    skip = !skip;
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, CreateMove)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, CreateCopy)
(benchmark::State& state) {
  Arb x = random(state);
  for (auto _ : state) {
    Arb y(x);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, CreateCopy)->Apply(ArbBenchmark::BenchmarkedSizes);

// For comparison, assignments with the C API
BENCHMARK_DEFINE_F(ArbBenchmark, Assign_C)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  arb_t x_, y_;
  arb_init(x_);
  arb_init(y_);
  arb_set(y_, y.arb_t());

  for (auto _ : state) {
    arb_set(x_, y_);
  }

  arb_clear(y_);
  arb_clear(x_);
}
BENCHMARK_REGISTER_F(ArbBenchmark, Assign_C)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, Assign)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  for (auto _ : state) {
    x = y;
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Assign)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, AssignMove)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  Arb z = y;

  for (auto _ : state) {
    x = std::move(y);
    y = std::move(x);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, AssignMove)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, Addition_Inplace_Yap)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  for (auto _ : state) {
    x = y;
    x += y(64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Addition_Inplace_Yap)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, Addition_Yap)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  for (auto _ : state) {
    x = y;
    x = (x + y)(64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Addition_Yap)->Apply(ArbBenchmark::BenchmarkedSizes);

// For comparison, arithmetic with the C API
BENCHMARK_DEFINE_F(ArbBenchmark, Addition_C)
(benchmark::State& state) {
  Arb x = random(state), y = random(state);

  for (auto _ : state) {
    x = y;
    arb_add(x.arb_t(), x.arb_t(), y.arb_t(), 64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Addition_C)->Apply(ArbBenchmark::BenchmarkedSizes);

BENCHMARK_DEFINE_F(ArbBenchmark, Arithmetic_Yap)
(benchmark::State& state) {
  Arb x = random(state), y = random(state), z = random(state);

  for (auto _ : state) {
    x = y;
    x += (y * z + x)(64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Arithmetic_Yap)->Apply(ArbBenchmark::BenchmarkedSizes);

// For comparison, the same, naively with the C API
BENCHMARK_DEFINE_F(ArbBenchmark, Arithmetic_C)
(benchmark::State& state) {
  Arb x = random(state), y = random(state), z = random(state);

  for (auto _ : state) {
    x = y;
    Arb lhs;
    arb_mul(lhs.arb_t(), y.arb_t(), z.arb_t(), 64);
    Arb value;
    arb_add(value.arb_t(), lhs.arb_t(), x.arb_t(), 64);
    arb_add(x.arb_t(), x.arb_t(), value.arb_t(), 64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Arithmetic_C)->Apply(ArbBenchmark::BenchmarkedSizes);

// For comparison, the same, optimized with the C API
BENCHMARK_DEFINE_F(ArbBenchmark, Arithmetic_C_optimized)
(benchmark::State& state) {
  Arb x = random(state), y = random(state), z = random(state);

  for (auto _ : state) {
    x = y;
    arb_add(x.arb_t(), x.arb_t(), x.arb_t(), 64);
    arb_addmul(x.arb_t(), y.arb_t(), z.arb_t(), 64);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark, Arithmetic_C_optimized)->Apply(ArbBenchmark::BenchmarkedSizes);

}  // namespace exactreal::test
