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
#include "exact-real/cereal.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arf.hpp"

using std::max;
using std::ostream;

namespace exactreal {
RealNumber::~RealNumber() {}

RealNumber::operator double() const { return static_cast<double>(arf(54)); }

RealNumber::operator bool() const {
  auto q = static_cast<std::optional<mpq_class>>(*this);
  return !q || *q;
}

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

  assert(this->cmp(arb) == 0);
  assert(!*this || arb_rel_accuracy_bits(arb.arb_t()) >= prec);
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

bool RealNumber::operator==(const RealNumber& rhs) const {
  // Since all real numbers are created by a UniqueFactory, it suffices to compare them for identity.
  return this == &rhs;
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

bool RealNumber::operator==(const Arf& arf) const {
  auto rat = static_cast<std::optional<mpq_class>>(*this);
  if (!rat.has_value())
    return false;

  Arf num = (arf * Arf(rat->get_den(), 0))(ARF_PREC_EXACT, Arf::Round::NEAR);
  return num == Arf(rat->get_num(), 0);
}

bool RealNumber::operator==(const mpq_class& rat) const {
  return rat == static_cast<std::optional<mpq_class>>(*this);
}

ostream& operator<<(ostream& os, const RealNumber& self) {
  self >> os;
  return os;
}

const static std::optional<unsigned int> noSeed = 0;
const static std::type_info& RATIONAL = typeid(*RealNumber::rational(0));
const static std::type_info& RANDOM = typeid(*RealNumber::random(noSeed));
const static std::type_info& CONSTRAINED = typeid(*RealNumber::random(Arf(1), Arf(2), noSeed));
const static std::type_info& PRODUCT = typeid(*((*RealNumber::random(noSeed)) * (*RealNumber::random(noSeed))));

void save_rational(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);
void save_random(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);
void save_constrained(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);
void save_product(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);

void RealNumberCereal::save(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self) {
  if (typeid(*self) == RATIONAL) {
    archive(cereal::make_nvp("kind", std::string("rational")));
    save_rational(archive, self);
  } else if (typeid(*self) == RANDOM) {
    archive(cereal::make_nvp("kind", std::string("random")));
    save_random(archive, self);
  } else if (typeid(*self) == CONSTRAINED) {
    archive(cereal::make_nvp("kind", std::string("constrained")));
    save_constrained(archive, self);
  } else if (typeid(*self) == PRODUCT) {
    archive(cereal::make_nvp("kind", std::string("product")));
    save_product(archive, self);
  } else {
    throw std::logic_error("not implemented -- RealNumberCereal::save");
  }
}

void load_rational(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);
void load_random(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);
void load_constrained(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);
void load_product(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);

void RealNumberCereal::load(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self) {
  std::string kind;
  archive(cereal::make_nvp("kind", kind));
  if (kind == "rational") {
    load_rational(archive, self);
  } else if (kind == "random") {
    load_random(archive, self);
  } else if (kind == "constrained") {
    load_constrained(archive, self);
  } else if (kind == "product") {
    load_product(archive, self);
  } else {
    throw std::logic_error("not implemented - RealNumberCereal::load()");
  }
}

}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "exact-real/detail/smart_ostream.hpp"

template std::ostream& exactreal::operator<<(std::ostream&, const std::shared_ptr<const RealNumber>&);
