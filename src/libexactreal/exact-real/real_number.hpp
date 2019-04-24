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

#ifndef LIBEXACTREAL_REAL_NUMBER_HPP
#define LIBEXACTREAL_REAL_NUMBER_HPP

#include <gmpxx.h>
#include <boost/operators.hpp>
#include <memory>

#include "exact-real/exact-real.hpp"

namespace exactreal {

struct Arb;
struct Arf;

struct RealNumber : boost::totally_ordered<RealNumber> {
  virtual ~RealNumber();
  explicit operator double() const;

  // Return an Arf float with prec bits of relative accuracy approximating this
  // number.
  virtual Arf arf(long prec) const = 0;

  // Return an Arb with prec bits of relative accuracy which contains this
  // number.
  Arb arb(long prec) const;

  // Ensure that arb contains this number and that the resulting arb number
  // has at least prec bits of relative accuracy.
  void refine(Arb& arb, long prec) const;

  Arb& iadd(Arb& self, long prec);
  bool operator<(const RealNumber&) const;
  virtual bool operator==(const RealNumber&) const = 0;

  int cmp(const Arb&) const;
  bool operator<(const Arb&) const;
  bool operator>(const Arb&) const;
  bool operator==(const Arb&) const;

  virtual RealNumber const& operator>>(std::ostream&) const = 0;
  friend std::ostream& operator<<(std::ostream&, const RealNumber&);

  // A random real in the range [0, 1]
  static std::unique_ptr<RealNumber> random();
  // A random real in the range [a, b]
  static std::unique_ptr<RealNumber> random(const Arf& a, const Arf& b);
  // A random real number, close to d; mostly useful to port code from doubles
  // that are meant to ressemble random reals
  static std::unique_ptr<RealNumber> random(const double d);
  static std::unique_ptr<RealNumber> rational(const mpq_class&);
  static std::unique_ptr<RealNumber> liouville(size_t base = 2);
  static std::unique_ptr<RealNumber> pi();
  static std::unique_ptr<RealNumber> e();
};

}  // namespace exactreal

#endif
