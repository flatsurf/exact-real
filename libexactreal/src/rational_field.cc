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

#include "../exact-real/rational_field.hpp"

#include "../exact-real/arb.hpp"

namespace exactreal {
RationalField::RationalField() {}
RationalField::RationalField(const mpq_class&) {}

RationalField RationalField::compositum(const RationalField&, const RationalField&) { return RationalField(); }

Arb RationalField::arb(const ElementClass& x, prec prec) { return Arb(x, prec); }

bool RationalField::unit(const ElementClass& x) { return x != 0; }

std::optional<mpq_class> RationalField::rational(const ElementClass& x) { return x; }

mpz_class RationalField::floor(const ElementClass& x) { return x.get_num() / x.get_den(); }
}  // namespace exactreal
