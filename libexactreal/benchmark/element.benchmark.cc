/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2020 Julian Rüth
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

#include <memory>
#include <algorithm>

#include <benchmark/benchmark.h>

#include <e-antic/renfxx.h>

#include "../exact-real/module.hpp"
#include "../exact-real/element.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/rational_field.hpp"
#include "../exact-real/number_field.hpp"

namespace exactreal::test {

template <typename Ring>
class ElementBenchmark : public benchmark::Fixture {
  typename Ring::ElementClass coefficient() {
    return rand() % 1024;
  }

  Element<Ring> monomial(std::vector<long> degrees, const std::vector<std::shared_ptr<const RealNumber>>& gens) {
    Element<Ring> x = Module<Ring>::make({RealNumber::rational(1)})->gen(0); 

    for (int i = 0; i < degrees.size(); i++)
      while(degrees[i]--)
        x *= *gens[i];

    return x;
  }

  Element<Ring> element(std::vector<long> degrees, const std::vector<std::shared_ptr<const RealNumber>>& gens) {
    const auto module = Module<Ring>::make({RealNumber::rational(1)});

    auto x = coefficient() * monomial(degrees, gens);

    if (!std::any_of(begin(degrees), end(degrees), [](const long d) { return d; }))
      return x;

    while (true) {
      const size_t i = rand() % degrees.size();
      if (degrees[i]) {
        degrees[i]--;
        return x + element(degrees, gens);
      }
    }
  }

  auto elements(benchmark::State& state) {
    const long vars = state.range(0);

    std::vector<std::shared_ptr<const RealNumber>> gens;
    gens.push_back(RealNumber::rational(1));
    for (int i = 0; i < vars; i++)
      gens.push_back(RealNumber::random());

    std::vector<long> lhs_degrees, rhs_degrees;
    for (int i = 0; i < vars; i++) {
      lhs_degrees.push_back(state.range(1 + i));
      rhs_degrees.push_back(state.range(1 + vars + i));
    }

    return std::tuple{ element(lhs_degrees, gens), element(rhs_degrees, gens) };
  }

 public:

  void truediv(benchmark::State& state) {
    const auto [lhs, rhs] = elements(state);

    const auto dividend = lhs * rhs;
    const auto divisor = rhs;

    for (auto _ : state) {
      const auto quotient = dividend.truediv(divisor);
      assert(quotient);
    }
  }

  static void BenchmarkedDegrees(benchmark::internal::Benchmark* b) {
    // elements in zero variables, i.e., base ring elements
    b->Args({0,});
    // elements in one variable, x^1 + … (lower degree summands,) and x^1 + …
    b->Args({1, 1, 1});
    // elements in two variables, x^1 + …, and y^1 + …
    b->Args({2, 1, 0, 0, 1}),
    // x^3*y^4 + …, and x^1*y^2 + …
    b->Args({2, 3, 4, 1, 2});
  }
};

BENCHMARK_TEMPLATE_DEFINE_F(ElementBenchmark, truediv_Z, IntegerRing)(benchmark::State& state) { truediv(state); }
BENCHMARK_REGISTER_F(ElementBenchmark, truediv_Z)->Apply(ElementBenchmark<IntegerRing>::BenchmarkedDegrees);

BENCHMARK_TEMPLATE_DEFINE_F(ElementBenchmark, truediv_Q, RationalField)(benchmark::State& state) { truediv(state); }
BENCHMARK_REGISTER_F(ElementBenchmark, truediv_Q)->Apply(ElementBenchmark<RationalField>::BenchmarkedDegrees);

BENCHMARK_TEMPLATE_DEFINE_F(ElementBenchmark, truediv_K, NumberField)(benchmark::State& state) { truediv(state); }
BENCHMARK_REGISTER_F(ElementBenchmark, truediv_K)->Apply(ElementBenchmark<NumberField>::BenchmarkedDegrees);


}  // namespace exactreal::test

