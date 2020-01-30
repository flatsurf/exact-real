/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyrhs (C) 2019 Julian Rüth
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

#include <ostream>

#include "../exact-real/cppyy.hpp"

using namespace exactreal;

namespace exactreal {
Arb binary(const Arb& lhs, const Arb& rhs, char op, prec prec) {
  if (op == '+') {
    return (lhs + rhs)(prec);
  } else if (op == '-') {
    return (lhs - rhs)(prec);
  } else if (op == '*') {
    return (lhs * rhs)(prec);
  } else if (op == '/') {
    return (lhs / rhs)(prec);
  } else {
    throw std::logic_error("invalid operator");
  }
}

Arf binary(const Arf& lhs, const Arf& rhs, char op, prec prec, Arf::Round round) {
  if (op == '+') {
    return (lhs + rhs)(prec, round);
  } else if (op == '-') {
    return (lhs - rhs)(prec, round);
  } else if (op == '*') {
    return (lhs * rhs)(prec, round);
  } else if (op == '/') {
    return (lhs / rhs)(prec, round);
  } else {
    throw std::logic_error("invalid operator");
  }
}

}  // namespace exactreal
