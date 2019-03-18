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

#include <cassert>

#include "exact-real/arb.hpp"
#include "exact-real/arf.hpp"
#include "exact-real/real_number.hpp"

using std::max;
using std::ostream;

namespace exactreal {
RealNumber::~RealNumber() {}

RealNumber::operator double() const { return static_cast<double>(arf(54)); }

int RealNumber::cmp(const Arb& arb) const {
  auto interval = arb.arf();
  auto a = interval.first, b = interval.second;

  auto approx = arf(max(arf_bits(a.t), arf_bits(b.t)) + 1);
  if (approx < a) {
    return -1;
  } else if (approx > b) {
    return 1;
  } else {
    return 0;
  }
}

void RealNumber::refine(Arb& arb, long prec) const {
  // Since we refine, arb must contain this real number.
  assert(this->operator==(arb));

  if (arb_rel_accuracy_bits(arb.t) >= prec) {
    return;
  }

  Arf midpoint = arf(prec);
  arb_set_arf(arb.t, midpoint.t);
  arb_add_error_2exp_si(arb.t,
                        (fmpz_get_si(&midpoint.t[0].exp) - 1) - (prec + 1));
}

Arb RealNumber::arb(long prec) const {
  Arb ret = Arb::any();
  refine(ret, prec);
  return ret;
}

bool RealNumber::operator<(const RealNumber& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  Arb self = Arb::any();
  Arb other = Arb::any();
  for (long prec = 2;; prec *= 2) {
    refine(self, prec);
    rhs.refine(other, prec);
    auto cmp = self.cmp(other);
    if (cmp.has_value()) {
      if (cmp.value() < 0) {
        return true;
      }
      if (cmp.value() > 0) {
        return false;
      }
    }
  }
}

bool RealNumber::operator==(const Arb& rhs) const { return cmp(rhs) == 0; }

ostream& operator<<(ostream& out, const RealNumber& self) {
  self >> out;
  return out;
}
}  // namespace exactreal
