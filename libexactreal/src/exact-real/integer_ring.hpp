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

#ifndef LIBEXACTREAL_INTEGER_RING_HPP
#define LIBEXACTREAL_INTEGER_RING_HPP

#include <gmpxx.h>
#include <boost/operators.hpp>

#include "exact-real/exact-real.hpp"
#include "exact-real/forward.hpp"

namespace exactreal {

struct IntegerRing : boost::equality_comparable<IntegerRing> {
  IntegerRing();
  IntegerRing(const mpz_class&);

  static IntegerRing compositum(const IntegerRing& lhs, const IntegerRing& rhs);

  typedef mpz_class ElementClass;
  static constexpr bool isField = false;
  static Arb arb(const ElementClass& x, long prec);
  bool operator==(const IntegerRing&) const { return true; }
};

}  // namespace exactreal

#endif
