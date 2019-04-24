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

#include <gmpxx.h>

#include "exact-real/real_number.hpp"
#include "exact-real/yap/arf.hpp"

using namespace exactreal;
using std::make_unique;
using std::ostream;
using std::unique_ptr;

namespace {

// An exact rational number
struct RationalRealNumber final : RealNumber {
  explicit RationalRealNumber(const mpq_class& value) : value(value) {}

  virtual Arf arf(long prec) const override {
    return (Arf(value.get_num(), 0) / Arf(value.get_den(), 0))(prec, Arf::Round::NEAR);
  }

  bool operator==(const RealNumber& rhs) const override {
    if (typeid(rhs) == typeid(*this)) {
      return this->value == static_cast<const RationalRealNumber*>(&rhs)->value;
    } else {
      return false;
    }
  }

  RealNumber const& operator>>(ostream& out) const override {
    out << value;
    return *this;
  }

 private:
  mpq_class value;
};
}  // namespace

namespace exactreal {
unique_ptr<RealNumber> RealNumber::rational(const mpq_class& value) { return make_unique<RationalRealNumber>(value); }
}  // namespace exactreal
