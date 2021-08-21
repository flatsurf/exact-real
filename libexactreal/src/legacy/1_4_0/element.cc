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

#include <e-antic/renf_elem_class.hpp>

#include "element.hpp"

namespace exactreal {

template <typename Ring>
class LIBEXACTREAL_API Element {
 public:
  template <typename C>
  std::vector<C> coefficients() const LIBEXACTREAL_API;
};

template <typename Ring>
template <typename C>
std::vector<C> Element<Ring>::coefficients() const {
  return Element_coefficients_1_4_0<Ring, C>(*this);
}

template std::vector<mpz_class> Element<IntegerRing>::coefficients<mpz_class>() const;
template std::vector<mpq_class> Element<RationalField>::coefficients<mpq_class>() const;
template std::vector<eantic::renf_elem_class> Element<NumberField>::coefficients<eantic::renf_elem_class>() const;
template std::vector<mpq_class> Element<NumberField>::coefficients<mpq_class>() const;

}
