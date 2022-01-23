/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#include <boost/lexical_cast.hpp>

#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/rational_field.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"

#include "module_generator.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

namespace exactreal::test {

TEMPLATE_TEST_CASE("Module", "[module]", IntegerRing, RationalField, NumberField) {
  using R = TestType;

  SECTION("Uniqueness") {
    REQUIRE(&*Module<R>::make({}) == &*Module<R>::make({}));
    REQUIRE(&*Module<R>::make({RealNumber::random()}) != &*Module<R>::make({RealNumber::random()}));
  }

  SECTION("Rank") {
    auto trivial = Module<R>::make({});
    REQUIRE(trivial->rank() == 0);
    REQUIRE(boost::lexical_cast<std::string>(*trivial) == boost::lexical_cast<std::string>(*trivial));

    auto m = Module<R>::make({RealNumber::random(), RealNumber::random()});
    REQUIRE(m->rank() == 2);
    REQUIRE(boost::lexical_cast<std::string>(*m) == boost::lexical_cast<std::string>(*m));

    auto n = Module<R>::make(m->basis());
    REQUIRE(m == n);
  }

  SECTION("Multiplication of Generators") {
    const auto m = Module<R>::make({RealNumber::random(), RealNumber::rational(1)});
    const auto n = Module<R>::make({RealNumber::rational(1)});

    const auto one = n->gen(0);
    const auto rnd = m->gen(0);

    REQUIRE((rnd * one).module()->rank() == 2);
    // This might not hold, as the order of the generators in the two modules might be different
    // REQUIRE((rnd * one).module() == rnd.module());
    REQUIRE((rnd * one).module() == (one * rnd).module());
    REQUIRE((rnd * rnd).module()->rank() == 3);
    REQUIRE((rnd * rnd).module() == (rnd * rnd).module());
    REQUIRE((rnd * rnd * rnd).module()->rank() == 4);
    REQUIRE((rnd * rnd * rnd).module() != (rnd * rnd).module());
  }

  SECTION("Construction of 1 Element") {
    REQUIRE(Module<R>::make({RealNumber::rational(1), RealNumber::random()})->one() == 1);
    REQUIRE(Module<R>::make({RealNumber::rational(mpq_class{1, 2}), RealNumber::random()})->one() == 1);

    if constexpr (std::is_same_v<R, IntegerRing>) {
      REQUIRE_THROWS(Module<R>::make({RealNumber::rational(2), RealNumber::random()})->one());
    } else {
      REQUIRE(Module<R>::make({RealNumber::rational(2), RealNumber::random()})->one() == 1);
  }
  }

  SECTION("Span of Modules") {
    const auto& m = GENERATE(take(4, modules<R>()));
    const auto& n = GENERATE(take(4, modules<R>()));

    const auto span = Module<R>::span(m.shared_from_this(), n.shared_from_this());

    REQUIRE(span->rank() >= m.rank());
    REQUIRE(span->rank() >= n.rank());
  }

  if constexpr (std::is_same_v<R, NumberField>) {
    SECTION("Trivial Composita") {
      const auto& m = GENERATE(take(4, modules<R>()));
      const auto& n = Module<R>::make({RealNumber::random()});

      const auto span = Module<R>::span(m.shared_from_this(), n);

      REQUIRE(span->rank() == m.rank() + n->rank());
    }
  }
}

}  // namespace exactreal::test
