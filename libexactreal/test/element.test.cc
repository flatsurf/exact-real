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

#include <e-antic/renfxx.h>

#include <boost/lexical_cast.hpp>
#include <unordered_set>

#include "../exact-real/arb.hpp"
#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"
#include "../exact-real/real_number.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

#include "module_generator.hpp"
#include "element_generator.hpp"

namespace exactreal::test {

TEMPLATE_TEST_CASE("Elements from Primitives", "[element]", IntegerRing, RationalField, NumberField) {
  using R = TestType;
  const auto M = Module<R>::make({RealNumber::rational(1), RealNumber::random()});
  const auto random = M->gen(1);

  auto one = Element(1);
  auto zero = Element(0);
  auto quot = Element(mpq_class(1, 2));

  // Check that arithmetic works with elements from different trivial modules
  REQUIRE(one + zero == one);
  REQUIRE(quot + zero == quot);
  REQUIRE(quot + one == one + quot);

  REQUIRE(random + zero == random);
  REQUIRE(random + one - one == random);
  REQUIRE(random + quot + quot == random + one);

  // Comparison between different parents in trivial cases
  REQUIRE(quot - quot == zero);
  REQUIRE(quot + quot == one);
  REQUIRE(random - random == zero);
  REQUIRE(random + one - random == one);
}

TEMPLATE_TEST_CASE("Element", "[element]", IntegerRing, RationalField, NumberField) {
  using R = TestType;

  const auto& M = GENERATE(take(4, modules<R>()));

  CAPTURE(M);

  const auto trivial = Element<IntegerRing>();

  SECTION("Relation Operators with Integer Types") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    REQUIRE((x <= static_cast<unsigned short>(0) || x >= static_cast<unsigned short>(0)));
    REQUIRE((x == static_cast<unsigned short>(0) || x != static_cast<unsigned short>(0)));
    if (x)
      REQUIRE((x < static_cast<unsigned short>(0) || x > static_cast<unsigned short>(0)));

    REQUIRE((x <= static_cast<short>(0) || x >= static_cast<short>(0)));
    REQUIRE((x == static_cast<short>(0) || x != static_cast<short>(0)));
    if (x)
      REQUIRE((x < static_cast<short>(0) || x > static_cast<short>(0)));

    REQUIRE((x <= 0u || x >= 0u));
    REQUIRE((x == 0u || x != 0u));
    if (x)
      REQUIRE((x < 0u || x > 0u));

    REQUIRE((x <= 0 || x >= 0));
    REQUIRE((x == 0 || x != 0));
    if (x)
      REQUIRE((x < 0 || x > 0));

    REQUIRE((x <= 0ul || x >= 0ul));
    REQUIRE((x == 0ul || x != 0ul));
    if (x)
      REQUIRE((x < 0ul || x > 0ul));

    REQUIRE((x <= 0l || x >= 0l));
    REQUIRE((x == 0l || x != 0l));
    if (x)
      REQUIRE((x < 0l || x > 0l));

    REQUIRE((x <= 0ull || x >= 0ull));
    REQUIRE((x == 0ull || x != 0ull));
    if (x)
      REQUIRE((x < 0ull || x > 0ull));

    REQUIRE((x <= 0ll || x >= 0ll));
    REQUIRE((x == 0ll || x != 0ll));
    if (x)
      REQUIRE((x < 0ll || x > 0ll));
  }

  SECTION("Relational Operators With Element") {
    const auto x = GENERATE_REF(elements<R>(M));
    const auto y = GENERATE_REF(elements<R>(M));

    CAPTURE(x, y);

    REQUIRE(x == x);
    REQUIRE(!(x != x));

    if (x != 0) {
      REQUIRE(x > 0);
      REQUIRE(x > M.zero());
    }

    if (x != 1) {
      REQUIRE(x < 1);
    }

    REQUIRE((x <= y || x >= y));

    if (x != y) {
      REQUIRE((x < y || x > y));
    }
  }

  SECTION("Relational Operators With Real Numbers") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    for (int i = 0; i < M.rank(); i++) {
      const auto& g = *M.basis()[i];

      REQUIRE((x <= g || x >= g));

      if (x == M.gen(i)) {
        REQUIRE(x == g);
        REQUIRE((x <= g && x >= g));
      } else {
        REQUIRE(x != g);
        REQUIRE((x < g || x > g));
      }
    }
  }

  SECTION("Coefficients") {
    const auto x = GENERATE_REF(elements<R>(M));

    REQUIRE(*x.module() == M);

    CAPTURE(x);

    for (size_t g = 0; g < M.rank(); g++) {
      for (size_t i = 0; i < M.rank(); i++) {
        CAPTURE(g, i);

        if (x == M.gen(g)) {
          if (i == g)
            REQUIRE(x[i] == 1);
          else
            REQUIRE(x[i] != 1);
        }
      }
    }

    REQUIRE(Element<R>(M.shared_from_this(), x.coefficients()) == x);
  }

  SECTION("Arithmetic with int Scalars") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    REQUIRE(x + x == 2 * x);
    REQUIRE(x - x == 0 * x);
    REQUIRE(-1 * x == -x);
    REQUIRE(1 * x == x);
    REQUIRE(0 * x == M.zero());
    REQUIRE(trivial * x == trivial);
  }

  SECTION("Arithmetic with mpz Scalars") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    if (x != 0) {
      REQUIRE(mpz_class(2) * x > x);
      REQUIRE(mpz_class(-1) * x < x);
      REQUIRE(mpz_class(-1) * x < M.zero());
    }

    REQUIRE(x + x == mpz_class(2) * x);
    REQUIRE(x - x == mpz_class(0) * x);
    REQUIRE(mpz_class(-1) * x == -x);

    if (M.rank()) {
      REQUIRE(mpz_class(-1) * x < M.one());
    }

    REQUIRE(mpz_class(1) * x == x);
    REQUIRE(mpz_class(0) * x == M.zero());
    REQUIRE(x / mpz_class(1) == x);
  }

  if constexpr (!std::is_same_v<R, IntegerRing>) {
    SECTION("Arithmetic with mpq Scalars") {
      const auto x = GENERATE_REF(elements<R>(M));

      CAPTURE(x);

      REQUIRE(x + x == mpq_class(2) * x);
      REQUIRE(x - x == mpq_class(0) * x);
      REQUIRE(mpq_class(-1) * x == -x);
      REQUIRE(mpq_class(1) * x == x);
      REQUIRE(mpq_class(0) * x == M.zero());
      REQUIRE(x / mpq_class(1) == x);
      REQUIRE(x / mpq_class(1, 1) == x);
      REQUIRE(x / mpq_class(1, 2) == 2 * x);
    }
  }

  if constexpr (std::is_same_v<R, NumberField>) {
    SECTION("Arithmetic with renf_elem_class Scalars") {
      const auto x = GENERATE_REF(elements<R>(M));

      CAPTURE(x);

      const auto K = M.ring().parameters;

      REQUIRE(x + x == eantic::renf_elem_class(*K, 2) * x);
      REQUIRE(x - x == eantic::renf_elem_class(*K, 0) * x);
      REQUIRE(eantic::renf_elem_class(*K, -1) * x == -x);
      REQUIRE(eantic::renf_elem_class(*K, 1) * x == x);
      REQUIRE(eantic::renf_elem_class(*K, 0) * x == M.zero());
      REQUIRE(-(eantic::renf_elem_class(*K, "a") * x) == eantic::renf_elem_class(*K, "-a") * x);
      REQUIRE(mpq_class(1, 2) * x == x / 2);

      if (x > 0) {
        REQUIRE(2 * x > eantic::renf_elem_class(*K, "a") * x);
        REQUIRE(eantic::renf_elem_class(*K, "a") * x > x);
      }
    }
  }

  SECTION("Division") {
    const auto x = GENERATE_REF(elements<R>(M));
    const auto y = GENERATE_REF(elements<R>(M));

    CAPTURE(x, y);

    if (y) {
      if (x == y) {
        REQUIRE((x + x).truediv(y) == 2);
        REQUIRE((x + x).floordiv(y) == 2);
      } else {
        REQUIRE((x + x).truediv(y) != 2);
      }
    }
  }

  SECTION("Additive Structure") {
    const auto x = GENERATE_REF(elements<R>(M));
    const auto y = GENERATE_REF(elements<R>(M));

    CAPTURE(x, y);

    if (y)
      REQUIRE(x + y > x);

    REQUIRE(x - x == 0);
  }

  SECTION("Promotion from Trivial Elements") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    REQUIRE(M.zero() == trivial);

    if (x)
      REQUIRE(x != trivial);

    REQUIRE(x + trivial == x);
    REQUIRE(0 * x == trivial);
    REQUIRE(x >= trivial);
  }

  SECTION("Promotion from Submodule") {
    auto N = Module<R>::make({RealNumber::rational(1)});

    if (M.rank() > 1) {
      REQUIRE(N->gen(0) == M.gen(0));
      REQUIRE(N->gen(0) != M.gen(1));
      REQUIRE(&*(N->gen(0) + M.gen(0)).module() == &M);
    }
  }

  SECTION("Failing Pushout with Incompatible Generators") {
    auto N = Module<R>::make({RealNumber::rational(2)});

    if (M.rank()) {
      REQUIRE_THROWS(N->gen(0) + M.gen(0));
    } else {
      REQUIRE_NOTHROW(N->gen(0) + M.zero());
    }
  }

  SECTION("Explicit bool Cast") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    if (x == M.zero()) {
      REQUIRE(!x);
    } else {
      REQUIRE(x);
    }
  }

  SECTION("Printing") {
    const auto x = GENERATE_REF(elements<R>(M));
    const auto y = GENERATE_REF(elements<R>(M));

    CAPTURE(x, y);

    if (x == y)
      REQUIRE(boost::lexical_cast<std::string>(x) == boost::lexical_cast<std::string>(y));
    else
      REQUIRE(boost::lexical_cast<std::string>(x) != boost::lexical_cast<std::string>(y));
  }

  SECTION("Multiplication") {
    const auto x = GENERATE_REF(elements<R>(M));
    const auto y = GENERATE_REF(elements<R>(M));

    CAPTURE(x, y);

    REQUIRE(x * y == y * x);

    if (M.rank()) {
      REQUIRE(M.one() * x == x);
      REQUIRE(x * M.one() == x);
    }

    if (x != 1 && x != 0) {
      REQUIRE(x * x != x);
    }
  }

  SECTION("Simplification") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    if (x != 0 && x != 1)
      REQUIRE((x * x - x * x).module()->rank() > M.rank());

    REQUIRE((x * x - x * x).simplify().module()->rank() == 0);
  }

  SECTION("Division") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    if (x != 0) {
      REQUIRE((3 * x).truediv(x) == 3);

      if constexpr (std::is_same_v<R, exactreal::IntegerRing>) {
        REQUIRE(!((3 * x).truediv(2 * x)).has_value());
        REQUIRE((3 * x).floordiv(2 * x) == 1);
      }
    }

    if (M.rank()) {
      REQUIRE(*x.truediv(M.one()) == x);

      if (x != 1)
        REQUIRE(*((x + M.one()) * (x - M.one())).truediv(x - M.one()) == x + M.one());
    }

    if (x) {
      REQUIRE(*x.truediv(x) == 1);
      REQUIRE(*(x * x).truediv(x) == x);
    }
  }

  SECTION("Floor & Ceil") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    const auto floor = x.floor();
    const auto ceil = x.ceil();

    if (x == 1 || x == 0)
      REQUIRE(floor == ceil);
    else if (x == 0)
      REQUIRE(floor == ceil - 1);
  }

  SECTION("Unit") {
    const auto x = GENERATE_REF(elements<R>(M));

    CAPTURE(x);

    if (x == 1)
      REQUIRE(x.unit());
    else
      REQUIRE(!x.unit());
  }

  SECTION("Hashing") {
    {
      const auto set = std::unordered_set{M.zero(), M.zero()};
      REQUIRE(set.size() == 1);
    }

    if (M.rank()) {
      const auto set = std::unordered_set{M.zero(), M.one()};
      REQUIRE(set.size() == 2);
    }
  }

  if constexpr (!std::is_same_v<R, exactreal::IntegerRing>) {
    SECTION("Cast to Rational") {
      if (M.rank()) {
        REQUIRE(static_cast<std::optional<mpq_class>>(M.one()) == mpq_class(1, 1));
        REQUIRE(static_cast<std::optional<mpq_class>>(M.one() / 2) == mpq_class(1, 2));
      }
    }
  }

  SECTION("Cast to Integer") {
    if (M.rank()) {
      REQUIRE(static_cast<std::optional<mpz_class>>(M.one()) == 1);
      if constexpr (!std::is_same_v<R, exactreal::IntegerRing>) {
        REQUIRE(static_cast<std::optional<mpz_class>>(M.one() / 2) == std::nullopt);
      }
    }
  }
}

}  // namespace exactreal::test
