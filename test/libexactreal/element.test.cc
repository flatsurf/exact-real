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
#include <exact-real/rational_field_traits.hpp>
#include <exact-real/number_field_traits.hpp>

using namespace exactreal;
using std::vector;
using std::make_shared;
using std::shared_ptr;

TEST(ElementZZ, Generators) {
	auto m = Module<IntegerRingTraits>({ RealNumber::rational(1), RealNumber::random() });
	
	auto one = Element(m, 0);
	auto x = Element(m, 1);

	EXPECT_EQ(one, one);
	EXPECT_EQ(x, x);
	EXPECT_NE(one, x);
	EXPECT_GT(x, m.zero());
	EXPECT_LT(x, one);
}

TEST(ElementZZ, Additive) {
	auto m = Module<IntegerRingTraits>({ RealNumber::rational(1), RealNumber::random() });
	
	Element<IntegerRingTraits> elements[] { Element(m, 0), Element(m, 1) };

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_GT(x + elements[0], x);
		EXPECT_GT(x + elements[0], elements[0]);
		EXPECT_EQ(x - elements[i], m.zero());
	}
}

TEST(ElementZZ, Scalars) {
	auto m = Module<IntegerRingTraits>({ RealNumber::rational(1), RealNumber::random() });
	
	Element<IntegerRingTraits> elements[] { Element(m, 0), Element(m, 1) };

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, 2*x);
		EXPECT_EQ(x - x, 0*x);
		EXPECT_EQ(-1*x, -x);
		EXPECT_EQ(1*x, x);
		EXPECT_EQ(0*x, m.zero());
	}

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, mpz_class(2)*x);
		EXPECT_GT(mpz_class(2)*x, x);
		EXPECT_EQ(x - x, mpz_class(0)*x);
		EXPECT_EQ(mpz_class(-1)*x, -x);
		EXPECT_LT(mpz_class(-1)*x, x);
		EXPECT_LT(mpz_class(-1)*x, m.zero());
		EXPECT_EQ(mpz_class(1)*x, x);
		EXPECT_EQ(mpz_class(0)*x, m.zero());
	}
}

TEST(ElementQQ, Scalars) {
	auto m = Module<RationalFieldTraits>({ RealNumber::rational(1), RealNumber::random() });
	
	Element<RationalFieldTraits> elements[] { Element(m, 0), Element(m, 1) };

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, 2*x);
		EXPECT_EQ(x - x, 0*x);
		EXPECT_EQ(-1*x, -x);
		EXPECT_EQ(1*x, x);
		EXPECT_EQ(0*x, m.zero());
	}

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, mpz_class(2)*x);
		EXPECT_GT(mpz_class(2)*x, x);
		EXPECT_EQ(x - x, mpz_class(0)*x);
		EXPECT_EQ(mpz_class(-1)*x, -x);
		EXPECT_LT(mpz_class(-1)*x, x);
		EXPECT_LT(mpz_class(-1)*x, m.zero());
		EXPECT_EQ(mpz_class(1)*x, x);
		EXPECT_EQ(mpz_class(0)*x, m.zero());
	}

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, mpq_class(2)*x);
		EXPECT_EQ(x - x, mpq_class(0)*x);
		EXPECT_EQ(mpq_class(-1)*x, -x);
		EXPECT_EQ(mpq_class(1)*x, x);
		EXPECT_EQ(mpq_class(0)*x, m.zero());
	}
}

TEST(ElementNF, Scalars) {
  renf_class K("a^2 - 2", "a", "1.41 +/- 0.1", 64);
	auto m = Module<NumberFieldTraits>({ RealNumber::rational(1), RealNumber::random() }, K);
	
	Element<NumberFieldTraits> elements[] { Element(m, 0), Element(m, 1) };

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, 2*x);
		EXPECT_EQ(x - x, 0*x);
		EXPECT_EQ(-1*x, -x);
		EXPECT_EQ(1*x, x);
		EXPECT_EQ(0*x, m.zero());
	}

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, mpz_class(2)*x);
		EXPECT_GT(mpz_class(2)*x, x);
		EXPECT_EQ(x - x, mpz_class(0)*x);
		EXPECT_EQ(mpz_class(-1)*x, -x);
		EXPECT_LT(mpz_class(-1)*x, x);
		EXPECT_LT(mpz_class(-1)*x, m.zero());
		EXPECT_EQ(mpz_class(1)*x, x);
		EXPECT_EQ(mpz_class(0)*x, m.zero());
	}

	for (size_t i = 0; i<sizeof(elements)/sizeof(elements[0]); i++){
		auto x = elements[i];
		EXPECT_EQ(x + x, renf_elem_class(K, 2)*x);
		EXPECT_EQ(x - x, renf_elem_class(K, 0)*x);
		EXPECT_EQ(renf_elem_class(K, -1)*x, -x);
		EXPECT_EQ(renf_elem_class(K, 1)*x, x);
		EXPECT_EQ(renf_elem_class(K, 0)*x, m.zero());
		EXPECT_EQ(-(renf_elem_class(K, "a")*x), renf_elem_class(K, "-a")*x);
		EXPECT_GT(renf_elem_class(K, "a")*x, x);
		EXPECT_GT(2*x, renf_elem_class(K, "a")*x);
	}
}

#include "main.hpp"
