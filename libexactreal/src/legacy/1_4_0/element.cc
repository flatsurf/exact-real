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

#include "element.hpp"

#include <e-antic/renf_elem_class.hpp>

#include "../../../exact-real/external/spimpl/spimpl.h"
#include "../../../exact-real/integer_ring.hpp"
#include "../../../exact-real/number_field.hpp"
#include "../../../exact-real/rational_field.hpp"

namespace exactreal {

template <typename Ring>
class LIBEXACTREAL_API Element {
 public:
  template <typename C>
  std::vector<C> coefficients() const;

  template <typename T, typename = typename Ring::template multiplication_t<T>>
  Element& operator*=(const T& c);

  template <typename T, typename = typename Ring::template division_t<T>, typename = void>
  Element& operator/=(const T& c);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

template <typename Ring>
template <typename C>
std::vector<C> Element<Ring>::coefficients() const {
  return Element_coefficients_1_4_0<Ring, C>(*this);
}

template <typename Ring>
template <typename T, typename>
Element<Ring>& Element<Ring>::operator*=(const T& c) {
  return Element_operator_mul_1_4_0<Ring, T>(*this, c);
}

template <typename Ring>
template <typename T, typename, typename>
Element<Ring>& Element<Ring>::operator/=(const T& c) {
  return Element_operator_div_1_4_0<Ring, T>(*this, c);
}

template std::vector<mpq_class> Element<IntegerRing>::coefficients<mpq_class>() const;
template std::vector<mpz_class> Element<IntegerRing>::coefficients<mpz_class>() const;
template std::vector<mpq_class> Element<RationalField>::coefficients<mpq_class>() const;
template std::vector<eantic::renf_elem_class> Element<NumberField>::coefficients<eantic::renf_elem_class>() const;
template std::vector<mpq_class> Element<NumberField>::coefficients<mpq_class>() const;

template Element<IntegerRing>& Element<IntegerRing>::operator*=(const mpz_class&);
template Element<RationalField>& Element<RationalField>::operator*=(const mpz_class&);
template Element<RationalField>& Element<RationalField>::operator*=(const mpq_class&);
template Element<NumberField>& Element<NumberField>::operator*=(const mpz_class&);
template Element<NumberField>& Element<NumberField>::operator*=(const mpq_class&);

template Element<RationalField>& Element<RationalField>::operator/=(const mpz_class&);
template Element<RationalField>& Element<RationalField>::operator/=(const mpq_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const mpz_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const mpq_class&);

}  // namespace exactreal
