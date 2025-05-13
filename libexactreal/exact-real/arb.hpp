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

#ifndef LIBEXACTREAL_ARB_HPP
#define LIBEXACTREAL_ARB_HPP

#include <flint/flint.h>

#if __FLINT_RELEASE < 30000
#include <arb.h>
#else
#include <flint/arb.h>
#endif

#include <gmpxx.h>

#include <boost/yap/algorithm_fwd.hpp>

#include <e-antic/renfxx_fwd.hpp>
#include <memory>
#include <optional>

#include "arf.hpp"

namespace exactreal {
/// After some discussion with the Arb author, it seems that 64 and 128 are good
/// default precisions for which Arb should be particularly fast. We use this
/// only when we need some random precision to at which to start an algorithm.
/// For callers of the library, this should not have any effect other than
/// changing the performance of certain calls slightly.
inline constexpr const prec ARB_PRECISION_FAST = 64;

/// A wrapper for \rst{:c:type:`arb_t`} elements so we get C++ style memory
/// management.
///
/// We use some Yap magic to get nice operators (which is tricky
/// otherwise because we cannot pass the additional prec and rnd parameters to
/// operators in C++.)
///
/// If you don't like that magic and only want memory management, just create
/// elements
///
/// ```
/// 1+1
/// // -> 2
/// ```
///
///     #include <exact-real/arb.hpp>
///     exactreal::Arb x, y;
///
/// and then use the Arb functions directly:
///
///     arb_add(x.arb_t(), x.arb_t(), y.arb_t(), 64);
///
/// Using yap this can be rewritten as any of the following:
///
///     #include <exact-real/yap/arb.hpp>
///
///     x += y(64);
///     x = (x + y)(64);
///
/// Note that the latter might use an additional temporary Arb. See the
/// yap/arb.hpp header for more details.
///
/// Note that methods here are usually named as their counterparts in
/// \rst{:ref:`arb.h <arb>`} with the leading arb_ removed.
///
/// \note We do not really recommend that you use the yap interface. We are
/// considering to remove it because it is not terribly useful in practice and
/// annoying to maintain.
///
/// \note This class has nothing to do with exact-real but should be
/// implemented by FLINT itself. Eventually we would like to migrate these
/// classes into a proper C++ wrapper that provides C++ memory management for
/// FLINT.
class LIBEXACTREAL_API Arb
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : yap::Terminal<Arb, yap::ArbExpr>
#endif
{
 public:
  /// \name Arb(…)
  ///@{
  /// Create an exact zero element.
  ///
  ///     exactreal::Arb x;
  ///     std::cout << x;
  ///     // -> 0
  ///
  Arb() noexcept;

  /// Create a copy of `x`.
  ///
  ///     exactreal::Arb x{1337};
  ///     exactreal::Arb y{x};
  ///     (x == y) == true
  ///     // -> true
  ///
  Arb(const Arb&) noexcept;

  /// Create a new element from `x`.
  ///
  ///     exactreal::Arb x{1337};
  ///     exactreal::Arb y{std::move(x)};
  ///     std::cout << y;
  ///     // -> 1337.00
  ///
  Arb(Arb&&) noexcept;

  /// Create an exact element equal to this integer.
  ///
  ///     #include <gmpxx.h>
  ///
  ///     exactreal::Arb x{mpz_class{1337}};
  ///     std::cout << x;
  ///     // -> 1337.00
  ///
  explicit Arb(const mpz_class&) noexcept;

  /// Create an element containing this rational.
  /// Typically, the result won't be exact, in particular not if the rational
  /// cannot be represented exactly in base 2.
  ///
  ///     exactreal::Arb x{mpq_class{1, 2}};
  ///     std::cout << x;
  ///     // -> 0.500000
  ///
  ///     exactreal::Arb y{mpq_class{1, 3}};
  ///     std::cout << std::setprecision(32) << y;
  ///     // -> [0.33333333333333333331526329712524 +/- 2.72e-20]
  ///
  explicit Arb(const mpq_class&) noexcept;

  /// Create an element containing this rational using
  /// \rst{:c:func:`arb_set_fmpq`, i.e.,} by performing the division of
  /// numerator and denominator with precision `prec`.
  ///
  ///     exactreal::Arb x{mpq_class{1, 2}, 256};
  ///     std::cout << x;
  ///     // -> 0.50000000000000000000000000000000
  ///
  ///     exactreal::Arb y{mpq_class{1, 3}, 256};
  ///     std::cout << std::setprecision(32) << y;
  ///     // -> [0.33333333333333333333333333333333 +/- 3.34e-33]
  ///
  explicit Arb(const mpq_class&, const prec) noexcept;

  /// Create an element containing this number field element.
  /// The precision of the Arb ball, depends on the internal representation of
  /// the number field element.
  ///
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto a = eantic::renf_elem_class(*K, std::vector{-1, 1});
  ///
  ///     exactreal::Arb x{a};
  ///     std::cout << x;
  ///     // -> [0.41421356237309504876378807303183 +/- 1.09e-19]
  ///
  explicit Arb(const eantic::renf_elem_class&) noexcept;

  /// Create an element containing this number field element in a ball of
  /// precision at least `prec`.
  ///
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto a = eantic::renf_elem_class(*K, std::vector{-1, 1});
  ///
  ///     exactreal::Arb x{a, 8};
  ///     std::cout << std::setprecision(32) << x;
  ///     // -> [0.41406250000000000000000000000000 +/- 7.82e-3]
  ///
  ///     exactreal::Arb y{a, 256};
  ///     std::cout << std::setprecision(32) << y;
  ///     // -> [0.41421356237309504880168872420970 +/- 1.93e-33]
  ///
  ///     exactreal::Arb z{a, 8};
  ///     std::cout << std::setprecision(32) << z;
  ///     // -> [0.41406250000000000000000000000000 +/- 7.82e-3]
  ///
  explicit Arb(const eantic::renf_elem_class&, const prec) noexcept;

  /// Create an Arb ball with lower and upper bound as given by the pair, see
  /// \rst{:c:func:`arb_set_interval_arf`.}
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf lower{0}, upper{1};
  ///     exactreal::Arb x{std::pair{lower, upper}};
  ///     std::cout << x;
  ///     // -> [0.50000000000000000000000000000000 +/- 0.501]
  ///
  ///     exactreal::Arb y{std::pair{lower, upper}, 64};
  ///     std::cout << y;
  ///     // -> [0.50000000000000000000000000000000 +/- 0.501]
  ///
  explicit Arb(const std::pair<Arf, Arf>&, const prec = ARF_PREC_EXACT);

  /// Create an exact element equal to the given floating point element, see
  /// \rst{:c:func:`arb_set_arf`.}
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arb y{x};
  ///     std::cout << y;
  ///     // -> 1.0000000000000000000000000000000
  ///
  explicit Arb(const Arf& midpoint);

  /// Create an exact element, equal to this integer.
  ///
  ///     exactreal::Arb x{1};
  ///     std::cout << x;
  ///     // -> 1.0000000000000000000000000000000
  ///
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  explicit Arb(Integer) noexcept;

  /// Create an element from this string, see \rst{:c:func:`arb_set_str`.}
  ///
  ///     exactreal::Arb x{"[3.25 +/- 0.0001]", 64};
  ///     std::cout << x;
  ///     // -> [3.2500000000000000000000000000000 +/- 1.01e-4]
  ///
  explicit Arb(const std::string&, const prec);

  ~Arb() noexcept;

  /// \name operator=
  /// Reset this element to the one given.
  ///
  ///     exactreal::Arb x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> 1.0000000000000000000000000000000
  ///
  ///     x = y;
  ///     std::cout << y;
  ///     // -> 1.0000000000000000000000000000000
  ///
  ///     x = 1;
  ///     std::cout << x;
  ///     // -> 1.0000000000000000000000000000000
  ///
  ///@{
  Arb& operator=(const Arb&) noexcept;
  Arb& operator=(Arb&&) noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  Arb& operator=(Integer) noexcept;
  Arb& operator=(const mpz_class&) noexcept;
  ///@}

  /// Return the negative of this element.
  /// This method returns a ball whose lower and upper bound is the negative of
  /// the upper and lower bound, respectively.
  ///
  ///     exactreal::Arb x{1};
  ///     std::cout << -x;
  ///     // -> -1.0000000000000000000000000000000
  ///
  Arb operator-() const noexcept;

  /// \name Comparison Operators
  /// The comparison operators return a value if the relation is true for all
  /// elements in the ball described by the element, e.g., x < y, returns true
  /// if the relation is true for every element in x and y, they return false if
  /// the relation is false for every element in x and y, and nothing otherwise.
  /// Note that this is different from the semantic in Arb where false is
  /// returned in both of the latter cases.
  ///
  ///     exactreal::Arb x{mpq_class{1, 3}};
  ///     (x < 1).has_value()
  ///     // -> true
  ///
  ///     (x < 1).value()
  ///     // -> true
  ///
  ///     (x < x).has_value()
  ///     // -> false
  ///
  ///@{
  std::optional<bool> operator==(const Arb&) const noexcept;
  std::optional<bool> operator!=(const Arb&) const noexcept;
  std::optional<bool> operator<(const Arb&) const noexcept;
  std::optional<bool> operator>(const Arb&) const noexcept;
  std::optional<bool> operator<=(const Arb&) const noexcept;
  std::optional<bool> operator>=(const Arb&) const noexcept;

  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator<(Integer) const noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator>(Integer) const noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator<=(Integer) const noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator>=(Integer) const noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator==(Integer) const noexcept;
  template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
  std::optional<bool> operator!=(Integer) const noexcept;

  std::optional<bool> operator<(const mpq_class&) const noexcept;
  std::optional<bool> operator>(const mpq_class&) const noexcept;
  std::optional<bool> operator<=(const mpq_class&) const noexcept;
  std::optional<bool> operator>=(const mpq_class&) const noexcept;
  std::optional<bool> operator==(const mpq_class&) const noexcept;
  std::optional<bool> operator!=(const mpq_class&) const noexcept;
  ///@}

  /// Return whether this Arb element exactly represents a floating point
  /// number, i.e., whether its radius is zero, see
  /// \rst{:c:func:`arb_is_exact`.}
  ///
  ///     exactreal::Arb x{1};
  ///     x.is_exact()
  ///     // -> true
  ///
  ///     exactreal::Arb y{mpq_class{1, 3}};
  ///     y.is_exact()
  ///     // -> false
  ///
  bool is_exact() const noexcept;

  /// Return whether this Arb element does contain neither plus nor minus
  /// infinity, see \rst{:c:func:`arb_is_finite`.}
  ///
  ///     exactreal::Arb x{1};
  ///     x.is_finite()
  ///     // -> true
  ///
  ///     exactreal::Arb y{1};
  ///     arb_div_si(y.arb_t(), y.arb_t(), 0, 64);
  ///     y.is_finite()
  ///     // -> false
  ///
  bool is_finite() const noexcept;

  /// Return the lower and the upper bound of this ball.
  /// See \rst{:c:func:`arb_get_interval_arf`.}
  ///
  ///     exactreal::Arb x{mpq_class{1, 3}};
  ///     auto bounds = static_cast<std::pair<exactreal::Arf, exactreal::Arf>>(x);
  ///     std::cout << bounds.first << ", " << bounds.second;
  ///     // -> 0.33333333333333331482961625624739=1537228672809129301p-62, 0.33333333333333331482961625624739=3074457345618258603p-63
  ///
  explicit operator std::pair<Arf, Arf>() const noexcept;

  /// Return the midpoint of this ball rounded to the closest double.
  /// Note that ties are rounded to even.
  ///
  ///     exactreal::Arb x{mpq_class{1, 3}};
  ///     static_cast<double>(x)
  ///     // -> 0.33333333333333331482961625624739
  ///
  explicit operator double() const noexcept;
  
  /// Return the exact midpoint of this ball.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arb x{mpq_class{1, 3}};
  ///     std::cout << static_cast<exactreal::Arf>(x);
  ///     // -> 0.33333333333333331482961625624739=6148914691236517205p-64
  ///
  explicit operator Arf() const noexcept;

  /// Write this element to the output stream, see \rst{:c:func:`arb_get_str`.}
  friend std::ostream& operator<<(std::ostream&, const Arb&) LIBEXACTREAL_API;

  /// Return a reference to the underlying \rst{:c:type:`arb_t`} element for
  /// direct manipulation with the C API of Arb.
  ::arb_t& arb_t() noexcept;

  /// Return a const reference to the underlying \rst{:c:type:`arb_t`} element
  /// for direct manipulation with the C API of Arb.
  const ::arb_t& arb_t() const noexcept;


  /// Return an exact zero element, i.e., the ball of radius zero centered at
  /// zero.
  ///
  ///     std::cout << exactreal::Arb::zero();
  ///     // -> 0
  ///
  static Arb zero() noexcept;

  /// Return an exact one element, i.e., the ball of radius zero centered at
  /// one.
  ///
  ///     std::cout << exactreal::Arb::one();
  ///     // -> 1.0000000000000000000000000000000
  ///
  static Arb one() noexcept;

  /// Return plus infinity, i.e., the ball of radius zero centered at plus
  /// infinity, see \rst{:c:func:`arb_pos_inf`}.
  /// Note that the result is printed as `[+/- inf]` unfortunately, see
  /// https://github.com/fredrik-johansson/arb/issues/332.
  ///
  ///     std::cout << exactreal::Arb::pos_inf();
  ///     // -> [+/- inf]
  ///
  static Arb pos_inf() noexcept;

  /// Return minus infinity, i.e., the ball of radius zero centered at minus
  /// infinity, see [arb_neg_inf]().
  /// Note that the result is printed as `[+/- inf]` unfortunately, see
  /// https://github.com/fredrik-johansson/arb/issues/332.
  ///
  ///     std::cout << exactreal::Arb::neg_inf();
  ///     // -> [+/- inf]
  ///
  static Arb neg_inf() noexcept;

  /// Return the extended real line, i.e., the interval [-∞,∞], see
  /// \rst{:c:func:`arb_zero_pm_inf`.}
  ///
  ///     std::cout << exactreal::Arb::zero_pm_inf();
  ///     // -> [+/- inf]
  ///
  static Arb zero_pm_inf() noexcept;

  /// Return an indeterminate, i.e., [NaN±∞] see
  /// \rst{:c:func:`arb_indeterminate`.}
  ///
  ///     std::cout << exactreal::Arb::indeterminate();
  ///     // -> nan
  ///
  static Arb indeterminate() noexcept;

  /// Return the interval [-1, 1], i.e., the ball of radius one centered at
  /// zero, see \rst{:c:func:`arb_zero_pm_one`.}
  /// Note that this prints as `[+/- 1.01]` instead of `[+/- 1.00]`, see
  /// https://github.com/fredrik-johansson/arb/issues/391
  ///
  ///     std::cout << exactreal::Arb::zero_pm_one();
  ///     // -> [+/- 1.01]
  ///
  static Arb zero_pm_one() noexcept;

  /// Return the unit interval [0, 1], i.e., the ball of radius 1/2 centered at
  /// 1/2, see \rst{:c:func:`arb_unit_interval`.}
  /// Note that that this does not print as `[0.5 +/- 0.5]`, see
  /// https://github.com/fredrik-johansson/arb/issues/391
  ///
  ///     std::cout << exactreal::Arb::unit_interval();
  ///     // -> [0.50000000000000000000000000000000 +/- 0.501]
  ///
  static Arb unit_interval() noexcept;

  /// Return a random element, see \rst{:c:func:`arb_randtest`.}
  ///
  ///     #include <flint/flint.h>
  ///     flint_rand_t rnd;
  ///     flint_randinit(rnd);
  ///
  ///     auto a = exactreal::Arb::randtest(rnd, 64, 16);
  ///     auto b = exactreal::Arb::randtest(rnd, 64, 16);
  ///     a.equal(b)
  ///     // -> false
  ///
  static Arb randtest(flint_rand_t, prec precision, prec magbits) noexcept;

  /// Return a random element, see \rst{:c:func:`arb_randtest_exact`.}
  ///
  ///     auto a = exactreal::Arb::randtest_exact(rnd, 64, 16);
  ///     auto b = exactreal::Arb::randtest_exact(rnd, 64, 16);
  ///     a.equal(b)
  ///     // -> false
  ///
  static Arb randtest_exact(flint_rand_t, prec precision, prec magbits) noexcept;

  /// Return whether elements have the same midpoint and radius.
  ///
  ///     exactreal::Arb a;
  ///     a.equal(a)
  ///     // -> true
  ///
  bool equal(const Arb&) const noexcept;

#ifndef DOXYGEN_DOCUMENTATION_BUILD
  // Syntactic sugar for Yap, so that expressions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  decltype(auto) operator()(Args&&...) const noexcept LIBEXACTREAL_LOCAL;

  // Construction from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arb(const yap::ArbExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arb(yap::ArbExpr<Kind, Tuple>&&) noexcept LIBEXACTREAL_LOCAL;

  // Assignment from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arb& operator=(const yap::ArbExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;
#endif

 private:
  /// The underlying \rst{:c:type:`arb_t`; use :cpp:func:`arb_t()
  /// <exactreal::Arb::arb_t>`} to get a reference to it.
  ::arb_t t;
};


/// Swap two elements efficiently, see \rst{:c:func:`arb_swap`.}
/// Used by some STL containers.
///
///     exactreal::Arb a{1}, b;
///     std::cout << "a = " << a << ", b = " << b;
///     // -> a = 1.0000000000000000000000000000000, b = 0
///
///     swap(a, b);
///     std::cout << "a = " << a << ", b = " << b;
///     // -> a = 0, b = 1.0000000000000000000000000000000
///
void LIBEXACTREAL_API swap(Arb& lhs, Arb& rhs);
}  // namespace exactreal

#endif
