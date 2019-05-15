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
#include "exact-real/forward.hpp"

namespace exactreal {

class RealNumber : boost::totally_ordered<RealNumber>,
                   boost::totally_ordered<RealNumber, signed char>,
                   boost::totally_ordered<RealNumber, unsigned char>,
                   boost::totally_ordered<RealNumber, signed int>,
                   boost::totally_ordered<RealNumber, unsigned int>,
                   boost::totally_ordered<RealNumber, signed short int>,
                   boost::totally_ordered<RealNumber, unsigned short int>,
                   boost::totally_ordered<RealNumber, signed long int>,
                   boost::totally_ordered<RealNumber, unsigned long int>,
                   boost::totally_ordered<RealNumber, Arf>,
                   boost::totally_ordered<RealNumber, mpz_class>,
                   boost::totally_ordered<RealNumber, mpq_class> {
 public:
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

  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator<(Integer) const noexcept;
  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator>(Integer) const noexcept;
  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator==(Integer) const noexcept;

  bool operator<(const Arf&) const;
  bool operator>(const Arf&) const;
  virtual bool operator==(const Arf&) const = 0;

  bool operator<(const mpq_class&) const;
  bool operator>(const mpq_class&) const;
  virtual bool operator==(const mpq_class&) const = 0;

  bool operator<(const mpz_class&) const noexcept;
  bool operator>(const mpz_class&) const noexcept;
  bool operator==(const mpz_class&) const noexcept;

  // Return whether the Arb interval contains this real (0) or the number is left (-1) or right (1) of that interval.
  int cmp(const Arb&) const;

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

template <typename Integer>
std::enable_if_t<std::is_integral_v<Integer>, bool> RealNumber::operator<(Integer rhs) const noexcept {
  return this->operator<(Arf(rhs));
}

template <typename Integer>
std::enable_if_t<std::is_integral_v<Integer>, bool> RealNumber::operator>(Integer rhs) const noexcept {
  return this->operator>(Arf(rhs));
}

template <typename Integer>
std::enable_if_t<std::is_integral_v<Integer>, bool> RealNumber::operator==(Integer rhs) const noexcept {
  return this->operator==(Arf(rhs));
}

}  // namespace exactreal

#endif
