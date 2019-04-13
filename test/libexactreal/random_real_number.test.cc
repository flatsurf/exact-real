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

#include <exact-real/arb.hpp>
#include <exact-real/arf.hpp>
#include <exact-real/real_number.hpp>

#include "arf.test.hpp"

using namespace exactreal;

TEST(RandomRealNumberTest, Equality) {
  auto rnd = RealNumber::random();
  EXPECT_EQ(*rnd, *rnd);
  EXPECT_NE(*rnd, *RealNumber::random());
}

TEST(RandomRealNumberTest, Double) {
  auto rnd = RealNumber::random();
  EXPECT_EQ(static_cast<double>(*rnd), static_cast<double>(*rnd));
  EXPECT_GE(static_cast<double>(*rnd), 0.);
  EXPECT_LT(static_cast<double>(*rnd), 1.);
}

TEST(RandomRealNumberTest, arf) { testArf(RealNumber::random()); }

TEST(RandomRealNumberTest, refine) {
  auto rnd = RealNumber::random();
  for (unsigned int prec = 1; prec <= 1024; prec *= 2) {
    Arb a = Arb::zero_pm_inf();

    rnd->refine(a, prec);
    EXPECT_EQ(arb_rel_accuracy_bits(a.arb_t()), prec);

    rnd->refine(a, prec / 2);
    EXPECT_EQ(arb_rel_accuracy_bits(a.arb_t()), prec);

    EXPECT_EQ(rnd->cmp(a), 0);
  }
}

TEST(RandomRealNumberTest, comparison) {
  auto rnd0 = RealNumber::random();
  auto rnd1 = RealNumber::random();
  EXPECT_NE(*rnd0, *rnd1);
  EXPECT_EQ(*rnd0, *rnd0);
  EXPECT_EQ(*rnd1, *rnd1);

  EXPECT_TRUE((*rnd0 < *rnd1) != (*rnd0 > *rnd1));
  if (*rnd0 > *rnd1) {
    return;
  }

  EXPECT_LT(*rnd0, *rnd1);
  EXPECT_GT(*rnd1, *rnd0);
}

struct RandomRealNumberFixture : benchmark::Fixture {
  std::unique_ptr<RealNumber> rnd = RealNumber::random();
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

#include "main.hpp"
