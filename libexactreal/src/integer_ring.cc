/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
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

#include "../exact-real/integer_ring.hpp"

#include "../exact-real/arb.hpp"

namespace exactreal {
IntegerRing::IntegerRing() {}
IntegerRing::IntegerRing(const mpz_class&) {}

IntegerRing IntegerRing::compositum(const IntegerRing&, const IntegerRing&) { return IntegerRing(); }

Arb IntegerRing::arb(const ElementClass& x, long) { return Arb(x); }

bool IntegerRing::unit(const ElementClass& x) { return x == 1 || x == -1; }

std::optional<mpq_class> IntegerRing::rational(const ElementClass& x) { return mpq_class(x); }

mpz_class IntegerRing::floor(const ElementClass& x) { return x; }
}  // namespace exactreal
