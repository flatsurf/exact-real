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
#include <e-antic/renfxx.h>
#include <gtest/gtest.h>

#include <exact-real/element.hpp>
#include <exact-real/integer_ring_traits.hpp>
#include <exact-real/module.hpp>
#include <exact-real/number_field_traits.hpp>
#include <exact-real/rational_field_traits.hpp>
#include <exact-real/real_number.hpp>

using namespace exactreal;
using std::make_shared;
using std::shared_ptr;
using std::vector;
using eantic::renf_class;
using eantic::renf_elem_class;

TEST(ElementZZ, Generators) {
	auto m = make_shared<Module<IntegerRingTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, 64);

  auto one = Element(m, 0);
  auto x = Element(m, 1);

  EXPECT_EQ(one, one);
  EXPECT_EQ(x, x);
  EXPECT_NE(one, x);
  EXPECT_GT(x, Element(m));
  EXPECT_LT(x, one);
}

TEST(ElementZZ, Additive) {
  auto m = make_shared<Module<IntegerRingTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, 64);

  Element<IntegerRingTraits> elements[]{Element(m, 0), Element(m, 1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_GT(x + elements[0], x);
    EXPECT_GT(x + elements[0], elements[0]);
    EXPECT_EQ(x - elements[i], Element(m));
  }
}

TEST(ElementZZ, PromotionFromTrivial) {
  auto m = make_shared<Module<IntegerRingTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, 64);

	auto gen = Element(m, 1);
	auto zero = Element(m);
	auto trivial = Element<IntegerRingTraits>();

	EXPECT_EQ(zero, trivial);
	EXPECT_NE(gen, trivial);

	EXPECT_EQ(gen + trivial, gen);
	EXPECT_EQ(zero + trivial, zero);
	EXPECT_EQ(0 * gen, trivial);
	EXPECT_GE(gen, trivial);
}

TEST(ElementZZ, Scalars) {
  auto m = make_shared<Module<IntegerRingTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, 64);

  Element<IntegerRingTraits> elements[]{Element(m, 0), Element(m, 1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, Element(m));
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, Element(m));
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, Element(m));
  }
}

TEST(ElementQQ, Scalars) {
  auto m = make_shared<Module<RationalFieldTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, 64);

  Element<RationalFieldTraits> elements[]{Element(m, 0), Element(m, 1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, Element(m));
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, Element(m));
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, Element(m));
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpq_class(2) * x);
    EXPECT_EQ(x - x, mpq_class(0) * x);
    EXPECT_EQ(mpq_class(-1) * x, -x);
    EXPECT_EQ(mpq_class(1) * x, x);
    EXPECT_EQ(mpq_class(0) * x, Element(m));
  }
}

TEST(ElementNF, Scalars) {
  renf_class K("a^2 - 2", "a", "1.41 +/- 0.1", 64);
  auto m = make_shared<Module<NumberFieldTraits>>(vector<std::shared_ptr<RealNumber>>{RealNumber::rational(1), RealNumber::random()}, K, 64);

  Element<NumberFieldTraits> elements[]{Element(m, 0), Element(m, 1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, Element(m));
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, Element(m));
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, Element(m));
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, renf_elem_class(K, 2) * x);
    EXPECT_EQ(x - x, renf_elem_class(K, 0) * x);
    EXPECT_EQ(renf_elem_class(K, -1) * x, -x);
    EXPECT_EQ(renf_elem_class(K, 1) * x, x);
    EXPECT_EQ(renf_elem_class(K, 0) * x, Element(m));
    EXPECT_EQ(-(renf_elem_class(K, "a") * x), renf_elem_class(K, "-a") * x);
    EXPECT_GT(renf_elem_class(K, "a") * x, x);
    EXPECT_GT(2 * x, renf_elem_class(K, "a") * x);

    EXPECT_EQ((x + x) / x, 2);
    EXPECT_EQ((x + x) / elements[!i], std::optional<renf_elem_class>{});
  }
}

#include "main.hpp"
