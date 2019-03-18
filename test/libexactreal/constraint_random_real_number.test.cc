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

#include <exact-real/real_number.hpp>
#include <exact-real/arf.hpp>
#include <exact-real/arb.hpp>

#include "arf.test.hpp"

using namespace exactreal;
using std::numeric_limits;
using std::nextafter;
using std::unique_ptr;
using std::vector;
using std::swap;

TEST(ConstraintRandomRealNumberFromDoubleTest, Equality) {
	auto rnd = RealNumber::random(13.37);
	ASSERT_EQ(*rnd, *rnd);
	ASSERT_NE(*rnd, *RealNumber::random());
	ASSERT_NE(*rnd, *RealNumber::random(13.37));
}

TEST(ConstraintRandomRealNumberFromIntervalTest, Equality) {
	auto rnd = RealNumber::random(Arf(-13l), Arf(37l));
	ASSERT_EQ(*rnd, *rnd);
	ASSERT_NE(*rnd, *RealNumber::random());
	ASSERT_NE(*rnd, *RealNumber::random(Arf(-13l), Arf(37l)));
}

void testDoubleToDouble(double x, double direction) {
	for (size_t i = 0; i < 1024; i++) {
		auto rnd = RealNumber::random(x);
		ASSERT_EQ(static_cast<double>(*rnd), x) << "where the former approximates " << *rnd << "; they differ by " << (static_cast<double>(*rnd) - x);
		x = nextafter(x, direction);
	}
}

TEST(ConstraintRandomRealNumberFromDoubleTest, Double) {
	testDoubleToDouble(0., numeric_limits<double>::infinity());
	testDoubleToDouble(0., -numeric_limits<double>::infinity());
	testDoubleToDouble(13e37, numeric_limits<double>::infinity());
	testDoubleToDouble(13e37, -numeric_limits<double>::infinity());
	testDoubleToDouble(-1337, numeric_limits<double>::infinity());
	testDoubleToDouble(-1337, -numeric_limits<double>::infinity());
}

void testIntervalToDouble(double a, double b) {
	auto rnd = RealNumber::random(Arf(a), Arf(b));
	ASSERT_LT(static_cast<double>(*rnd), b);
	ASSERT_GT(static_cast<double>(*rnd), a);
}

TEST(ConstraintRandomRealNumberFromIntervalTest, Double) {
	testIntervalToDouble(-1l, 1l);
	testIntervalToDouble(0, 1e-37);
	testIntervalToDouble(-13e37, 13e37);
	testIntervalToDouble(0, 1e37);
	testIntervalToDouble(1e-37, 1e37);
}

TEST(ConstraintRandomRealNumberFromDoubleTest, arf) {
	testArf(RealNumber::random(0.));
	testArf(RealNumber::random(13.37));
	testArf(RealNumber::random(13e37));
	testArf(RealNumber::random(13e-37));
	testArf(RealNumber::random(1));
	testArf(RealNumber::random(-1));
}

TEST(ConstraintRandomRealNumberFromIntervalTest, arf) {
	testArf(RealNumber::random(Arf(-1), Arf(1)));
	testArf(RealNumber::random(Arf(-13e-37), Arf(13e37)));
	testArf(RealNumber::random(Arf(-13e-37), Arf(13e-37)));
	testArf(RealNumber::random(Arf(13e37), Arf(13e38)));
	testArf(RealNumber::random(Arf(-13e-37), Arf(0)));
	testArf(RealNumber::random(Arf(0), Arf(13e37)));
}

/*
TEST(RandomRealNumberTest, arf) {
	auto rnd = RealNumber::random();
	Arf prev = Arf();
	Arf approx = Arf();
	for (unsigned int prec = 1; prec <= 1024; prec*=2){
		prev = approx;
		approx = rnd->arf(prec);
		ASSERT_EQ(approx, rnd->arf(prec));
		ASSERT_GE(approx, Arf());
		ASSERT_LE(approx, Arf(1l));

		// arf_bits(approx.t) >= prec might not be true because trailing zeros in
		// the floating point representation are not stored explicitly:
		// ASSERT_GE(arf_bits(approx.t), prec);
		// Instead we check that at least the precision does not decrease:
		ASSERT_GE(arf_bits(approx.t), arf_bits(prev.t));

		Arf diff = (approx - rnd->arf(prec * 2)).abs();
		for (auto i = 0u; i < prec; i++){
			ASSERT_LE(diff, 1);
			diff *= 2;
		}
	}
}

TEST(RandomRealNumberTest, refine) {
	auto rnd = RealNumber::random();
	for (unsigned int prec = 1; prec <= 1024; prec*=2){
		Arb a = Arb::any();

		rnd->refine(a, prec);
		ASSERT_EQ(arb_rel_accuracy_bits(a.t), prec);

		rnd->refine(a, prec/2);
		ASSERT_EQ(arb_rel_accuracy_bits(a.t), prec);

		ASSERT_EQ(rnd->cmp(a), 0);
	}
}

TEST(RandomRealNumberTest, comparison) {
	auto rnd0 = RealNumber::random();
	auto rnd1 = RealNumber::random();
	ASSERT_NE(*rnd0, *rnd1);
	ASSERT_EQ(*rnd0, *rnd0);
	ASSERT_EQ(*rnd1, *rnd1);

	ASSERT_TRUE((*rnd0 < *rnd1) != (*rnd0 > *rnd1));
	if (*rnd0 > *rnd1) {
		return;
	}

	ASSERT_LT(*rnd0, *rnd1);
	ASSERT_GT(*rnd1, *rnd0);
}

struct RandomRealNumberFixture : benchmark::Fixture {
	std::unique_ptr<RealNumber> rnd = RealNumber::random();
};

BENCHMARK_F(RandomRealNumberFixture, Double)(benchmark::State& state){ 
	for (auto _ : state) {
		benchmark::DoNotOptimize(static_cast<double>(*rnd));
	}
}

BENCHMARK_DEFINE_F(RandomRealNumberFixture, arf)(benchmark::State& state){
	for (auto _ : state) {
		benchmark::DoNotOptimize(rnd->arf(static_cast<unsigned int>(state.range(0))));
	}
}

BENCHMARK_REGISTER_F(RandomRealNumberFixture, arf)->Range(16, 1<<16);
*/

#include "main.hpp"

