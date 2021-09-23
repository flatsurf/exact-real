/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2021 Julian Rüth
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

#include <vector>
#include <e-antic/renf_elem_class.hpp>

#include "../../../exact-real/integer_ring.hpp"
#include "../../../exact-real/rational_field.hpp"
#include "../../../exact-real/number_field.hpp"
#include "../../../exact-real/external/spimpl/spimpl.h"
#include "../../external/catch2/single_include/catch2/catch.hpp"

namespace exactreal {

template <typename Ring>
class Element {
 public:
  Element();
  Element(const typename Ring::ElementClass&);

  template <typename C>
  std::vector<C> coefficients() const;

  template <typename T, typename = typename Ring::template multiplication_t<T>>
  Element& operator*=(const T& c);

  template <typename T, typename = typename Ring::template division_t<T>, typename=void>
  Element& operator/=(const T& c);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

}

namespace exactreal::test {

TEST_CASE("1.4.0 ABI of Element<IntegerRing>", "[element][legacy]") {
  auto x = Element<IntegerRing>(2);
  x *= mpz_class(2);
  REQUIRE(x.coefficients<mpz_class>() == std::vector<mpz_class>{4});
}

TEST_CASE("1.4.0 ABI of Element<RationalField>", "[element][legacy]") {
  auto x = Element<RationalField>(2);
  x *= mpz_class(2);
  x *= mpq_class(2);
  x /= mpz_class(2);
  x /= mpq_class(2);
  REQUIRE(x.coefficients<mpq_class>() == std::vector<mpq_class>{2});
}

TEST_CASE("1.4.0 ABI of Element<NumberField>", "[element][legacy]") {
  auto x = Element<NumberField>(eantic::renf_elem_class(2));
  x *= mpz_class(2);
  x *= mpq_class(2);
  x /= mpz_class(2);
  x /= mpq_class(2);
  REQUIRE(x.coefficients<mpq_class>() == std::vector<mpq_class>{2});
  REQUIRE(x.coefficients<eantic::renf_elem_class>() == std::vector<eantic::renf_elem_class>{2});
}

}
