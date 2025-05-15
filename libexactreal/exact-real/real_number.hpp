/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2025 Julian Rüth
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

#ifndef LIBEXACTREAL_REAL_NUMBER_HPP
#define LIBEXACTREAL_REAL_NUMBER_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <memory>
#include <optional>

#include "forward.hpp"

namespace exactreal {

/// A (possibly transcendental) real number.
///
/// Real numbers can be generated with factory functions (see below) and then
/// be used as the generators of a \ref Module, i.e., a submodule of the real
/// numbers over some base ring.
///
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <exact-real/element.hpp>
///     #include <exact-real/integer_ring.hpp>
///     #include <exact-real/rational_field.hpp>
///     #include <exact-real/number_field.hpp>
///
///     auto M = exactreal::Module<exactreal::IntegerRing>::make({
///       exactreal::RealNumber::rational(1),
///       exactreal::RealNumber::random()});
///     *M
///     // -> ℤ-Module(1, ℝ(<...>))
///
class LIBEXACTREAL_API RealNumber
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : public std::enable_shared_from_this<RealNumber>,
           boost::totally_ordered<RealNumber>,
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
           boost::totally_ordered<RealNumber, mpq_class>
#endif
{
 public:
  virtual ~RealNumber();

  /// \name Factory Functions
  ///@{
  /// Return a random real in the range [0, 1]
  static std::shared_ptr<const RealNumber> random();

  /// Return a random real whose random digits are taken from a fixed \p seed.
  static std::shared_ptr<const RealNumber> random(Seed seed);

  /// Return a random real in the range [a, b].
  static std::shared_ptr<const RealNumber> random(const Arf& a, const Arf& b);

  /// Return a random real in the range [a, b] whose random digits are taken
  /// from a fixed \p seed.
  static std::shared_ptr<const RealNumber> random(const Arf& a, const Arf& b, Seed seed);

  /// Return a random real number, close to \p d.
  ///
  /// This is mostly useful to port code from doubles that are meant to
  /// resemble random reals.
  ///
  ///     const auto x = exactreal::RealNumber::random(1.);
  ///     *x
  ///     // -> ℝ(1=<...> + ℝ(<...>)p-64)
  ///
  static std::shared_ptr<const RealNumber> random(const double d);

  /// Return a random real number, close to \p d whose random digits are taken
  /// from a fixed \p seed.
  static std::shared_ptr<const RealNumber> random(const double d, Seed seed);

  /// Return a real number that is an exact rational.
  static std::shared_ptr<const RealNumber> rational(const mpq_class&);
  ///@}

  /// Return the closest double to this real; ties are rounded to even.
  ///
  ///     const auto x = exactreal::RealNumber::rational(1);
  ///     static_cast<double>(*x)
  ///     // -> 1
  ///
  explicit operator double() const;

  /// Return whether this real number is non-zero.
  ///
  ///     const auto x = exactreal::RealNumber::rational(0);
  ///     static_cast<bool>(*x)
  ///     // -> false
  ///
  explicit operator bool() const;

  /// Return this real number as a rational number if possible.
  ///
  ///     const auto x = exactreal::RealNumber::rational(1);
  ///     static_cast<std::optional<mpq_class>>(*x).value()
  ///     // -> 1
  ///
  ///     const auto y = exactreal::RealNumber::random();
  ///     static_cast<std::optional<mpq_class>>(*y).has_value()
  ///     // -> false
  ///
  explicit virtual operator std::optional<mpq_class>() const = 0;

  /// Return an approximation of this number as an \ref Arf float with \p prec
  /// bits of relative accuracy, i.e., if \f$ e = \frac{|x - ~x|}{|x|} \f$ is
  /// the relative error, then \f$ \log_2(1/e) \ge \mathrm{prec} \f$.
  ///
  /// For example, for a random real number, this method just returns the first
  /// prec binary digits after the first non-zero digit.
  ///
  ///     #include <exact-real/arf.hpp>
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     x->arf(3)
  ///     // -> 0.3125=5p-4
  ///
  ///     x->arf(64)
  ///     // -> 0.333333=12297829382473034411p-65
  ///
  virtual Arf arf(long prec) const = 0;

  /// Return an \ref Arb with \p prec bits of relative accuracy which contains
  /// this number, i.e., the returned value satisfies
  /// `arb_rel_accuracy_bits(x.arb_t()) == prec`.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     x->arb(3)
  ///     // -> [0.312500 +/- 0.0157]
  ///
  /// Note that the printed output here for the radius can be misleading since
  /// printing via `std::cout` (which is used here) normally only requests a
  /// small number of digits of precision from the underlying
  /// \rst{:c:func:`arb_get_str`.}
  ///
  ///     x->arb(64)
  ///     // -> [0.333333 +/- 3.34e-7]
  ///
  ///     std::cout << std::setprecision(32) << x->arb(64);
  ///     // -> [0.33333333333333333334236835143738 +/- 6.78e-21]
  ///
  Arb arb(long prec) const;

  /// Shrink the ball \p arb such that contains this number and has \p prec
  /// bits of relative accuracy, i.e., `arb_rel_accuracy_bits(arb.arb_t()) >=
  /// prec`.
  ///
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     exactreal::Arb a(std::pair{exactreal::Arf(0), exactreal::Arf(1)});
  ///     a
  ///     // -> [0.500000 +/- 0.501]
  ///
  ///     x->refine(a, 2);
  ///     a
  ///     // -> [0.375000 +/- 0.0313]
  ///
  ///     x->refine(a, 16);
  ///     a
  ///     // -> [0.333336 +/- 2.04e-6]
  ///
  void refine(Arb& arb, long prec) const;

  /// \name Comparison Operators
  /// There are operators `<`, `<=`, `==`, `!=`, `>`, `>=` with other real
  /// numbers, primitive integer types, GMP integers and rationals, and \ref
  /// Arf floating points numbers.
  ///
  /// Note that many of these operators are provided by boost operators and not
  /// listed explicitly below.
  ///
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     *x < 1
  ///     // -> true
  ///
  ///     *x < mpz_class{1}
  ///     // -> true
  ///
  ///     *x == mpq_class{1, 3}
  ///     // -> true
  ///
  ///     *x == x->arf(64)
  ///     // -> false
  ///
  ///@{
  bool operator<(const RealNumber&) const;
  bool operator==(const RealNumber&) const;

  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator<(Integer) const noexcept;
  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator>(Integer) const noexcept;
  template <typename Integer>
  std::enable_if_t<std::is_integral_v<Integer>, bool> operator==(Integer) const noexcept;

  bool operator<(const Arf&) const;
  bool operator>(const Arf&) const;
  bool operator==(const Arf&) const;

  bool operator<(const mpq_class&) const;
  bool operator>(const mpq_class&) const;
  bool operator==(const mpq_class&) const;

  bool operator<(const mpz_class&) const noexcept;
  bool operator>(const mpz_class&) const noexcept;
  bool operator==(const mpz_class&) const noexcept;

  /// Return where the \ref Arb interface contains this real (`0`) or the
  /// number is left (`-1`) or right (`1`) of that interval.
  ///
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     exactreal::Arb a(std::pair{exactreal::Arf(0), exactreal::Arf(1)});
  ///     x->cmp(a)
  ///     // -> 0
  ///
  int cmp(const Arb&) const;
  ///@}

  /// Return whether this real number, interpreted as a multivariate monomial
  /// is smaller than \p rhs; This interprets most real numbers as indeterminates
  /// and their products as products of these basic indeterminates. This is
  /// used internally for the operator/. The indeterminates, i.e., real numbers,
  /// are ordered consistently (by some internal identifiers.)
  bool deglex(const RealNumber& rhs) const;

  /// Return the product of this real number and \p rhs.
  ///
  ///     const auto x = exactreal::RealNumber::rational(mpq_class{1, 3});
  ///     const auto xx = *x * *x;
  ///     *xx
  ///     // -> 1/9
  ///
  ///     const auto y = exactreal::RealNumber::random();
  ///     const auto yy = *y * *y;
  ///     *yy
  ///     // -> ℝ(<...>)^2
  ///
  ///     const auto z = exactreal::RealNumber::random();
  ///     const auto yz = *y * *z;
  ///     *yz
  ///     // -> ℝ(<...>)*ℝ(<...>)
  ///
  virtual std::shared_ptr<const RealNumber> operator*(const RealNumber& rhs) const;

  /// Return the exact quotient of this real number divided by \p rhs.
  ///
  /// Returns an `std::nullopt` when the exact quotient cannot be represented.
  ///
  ///     const auto y = exactreal::RealNumber::random();
  ///     const auto z = (*y) * (*y);
  ///
  ///     auto quo = *z / *z;
  ///     *quo.value()
  ///     // -> 1
  ///
  ///     quo = *z / *y;
  ///     *quo.value()
  ///     // -> ℝ(<...>)
  ///
  ///     quo = *y / *z;
  ///     quo.has_value()
  ///     // -> false
  ///
  virtual std::optional<std::shared_ptr<const RealNumber>> operator/(const RealNumber& rhs) const;

  /// Write this real to the output stream.
  ///
  /// This is a helper function for the friend `operator<<` to allow
  /// implementations to override how printing works.
  virtual RealNumber const& operator>>(std::ostream&) const = 0;

  friend std::ostream& operator<<(std::ostream&, const RealNumber&) LIBEXACTREAL_API;

  static void load(IDecerealizer&, std::shared_ptr<const RealNumber>&);
  static void save(ICerealizer&, const std::shared_ptr<const RealNumber>&);
};

/// @cond INTERNAL
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
/// @endcond

}  // namespace exactreal

#endif
