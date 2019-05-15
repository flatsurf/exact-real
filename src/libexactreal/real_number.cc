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
  auto interval = static_cast<std::pair<Arf, Arf>>(arb);
  auto a = interval.first, b = interval.second;

  auto approx = arf(max(arf_bits(a.arf_t()), arf_bits(b.arf_t())) + 1);
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
  assert(this->cmp(arb) == 0);

  if (arb_rel_accuracy_bits(arb.arb_t()) >= prec) {
    return;
  }

  Arf midpoint = arf(prec);
  arb_set_arf(arb.arb_t(), midpoint.arf_t());
  arb_add_error_2exp_si(arb.arb_t(), (fmpz_get_si(&midpoint.arf_t()[0].exp) - 1) - (prec + 1));
}

Arb RealNumber::arb(long prec) const {
  Arb ret = Arb::zero_pm_inf();
  refine(ret, prec);
  return ret;
}

bool RealNumber::operator<(const RealNumber& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  Arb self = Arb::zero_pm_inf();
  Arb other = Arb::zero_pm_inf();
  for (long prec = 2;; prec *= 2) {
    refine(self, prec);
    rhs.refine(other, prec);
    auto lt = self < other;
    if (lt.has_value()) {
      return *lt;
    }
  }
}

bool RealNumber::operator<(const Arf& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  Arb self = Arb::zero_pm_inf();
  Arb other = Arb(rhs);
  for (long prec = 2;; prec *= 2) {
    refine(self, prec);
    auto lt = self < other;
    if (lt.has_value()) {
      return *lt;
    }
  }
}

bool RealNumber::operator>(const Arf& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  return !this->operator<(rhs);
}

ostream& operator<<(ostream& out, const RealNumber& self) {
  self >> out;
  return out;
}
}  // namespace exactreal
