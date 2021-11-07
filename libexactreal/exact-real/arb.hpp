/* ********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019-2021 Julian Rüth
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
 * *******************************************************************/

/// C++ Wrappers for a ball arithmetic [arb_t]().

#ifndef LIBEXACTREAL_ARB_HPP
#define LIBEXACTREAL_ARB_HPP

#include <arb.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>

#include <boost/yap/algorithm_fwd.hpp>
#include <e-antic/renfxx_fwd.hpp>
#include <memory>
#include <optional>

#include "arf.hpp"

namespace exactreal {

// After some discussion with the Arb author, it seems that 64 and 128 are good
// default precisions for which Arb should be particularly fast. We use this
// only when we need some random precision to at which to start an algorithm.
// For callers of the library, this should not have any effect other than
// changing the performance of certain calls slightly.
inline constexpr const prec ARB_PRECISION_FAST = 64;

/// A wrapper for [::arb_t]() elements, i.e., floating point numbers surrounded
/// by a real ball of imprecision, so we get C++ style memory management. We
/// use some Yap magic to get nice operators (which is tricky otherwise because
/// we cannot pass the additional prec and rnd parameters to operators in C++.)
///
/// If you don't like that magic and only want memory management, just create
/// element and then use Arb functions directly:
///
///     #include <exact-real/arb.hpp>
///     Arb x, y;
///
///     arb_add(x.arb_t(), x.arb_t(), y.arb_t(), 64);
///
/// Using yap this can be rewritten as any of the following:
///
///     #include <exact-real/yap/arb.hpp>
///
///     Arb x, y;
///
///     x += y(64);
///     x = (x + y)(64);
///
/// Note that the latter might use an additional temporary Arb. See the
/// yap/arb.hpp header for more details.
///
/// Note that methods here are usually named as their counterparts in arb.h with
/// the leading arb_ removed.
///
/// Note that many methods provided by arb's C API are not yet provided by this
/// C++ wrapper. If something is missing for you, please let us know on our
/// [GitHub issues page](https://github.com/flatsurf/arbxx/issues).
class LIBEXACTREAL_API Arb : yap::Terminal<Arb, yap::ArbExpr> {
 public:
  /// Create an exact zero element.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x;
  ///     std::cout << x;
  ///     // -> (...)
  ///
  Arb() noexcept;

  /// Create a copy of `x`.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x = 1337;
  ///     Arb y{x};
  ///     x == y
  ///     // -> true
  ///
  Arb(const Arb&) noexcept;

  /// Create a new element from `x`.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x = 1337;
  ///     Arb y{std::move(x)};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  Arb(Arb&&) noexcept;

  /// Create an exact element equal to this integer.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x = mpz_class{1337};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  explicit Arb(const mpz_class&);

  /// Create an element containing this rational.
  /// Typically, the result won't be exact, in particular not if the rational
  /// cannot be represented exactly in base 2.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x = mpq_class{3, 1024};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  ///     Arb y = mpq_class{3, 1023};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  Arb(const mpq_class&);

  /// Create an element containing this rational using [arb_set_fmpq](), i.e.,
  /// by performing the division of numerator and denominator with precision
  /// `prec`.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{mpq_class{3, 1024}, 256};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  ///     Arb y{mpq_class{3, 1023}, 256};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  Arb(const mpq_class&, const prec);

  /// Create an element containing this number field element.
  /// The precision of the Arb ball, depends on the internal representation of
  /// the number field element.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto a = eantic::renf_elem_class(*K, std::vector{-1, 1});
  ///
  ///     Arb x{a};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  explicit Arb(const eantic::renf_elem_class&);

  /// Create an element containing this number field element in a ball of
  /// precision at least `prec`.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto a = eantic::renf_elem_class(*K, std::vector{-1, 1});
  ///
  ///     Arb x{a, 64};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  ///     Arb y{a, 256};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  ///     Arb z{a, 64};
  ///     std::cout << z;
  ///     // -> (...)
  ///
  /// Note how in this example, the precision increases from `x` to `z`.
  Arb(const eantic::renf_elem_class&, const prec);

  /// Create an Arb ball with lower and upper bound as given by the pair, see
  /// [arb_set_interval_arf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///     #include <exact-real/arf.hpp>
  ///
  ///     Arf lower{0}, upper{1};
  ///     Arb x{std::pair{lower, upper}};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  ///     Arb y{std::pair{lower, upper}, 64};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  explicit Arb(const std::pair<Arf, Arf>&, const prec = ARF_PREC_EXACT);

  /// Create an exact element equal to the given floating point element, see
  /// [arb_set_arf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///     #include <exact-real/arf.hpp>
  ///
  ///     Arf x{1};
  ///     Arb y{x};
  ///     std::cout << y;
  ///     // -> (...)
  ///
  explicit Arb(const Arf&);

  /// ==* `Arb(integer)` *==
  /// Create an exact element, equal to this integer.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{1};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  explicit Arb(short);
  explicit Arb(unsigned short);
  explicit Arb(int);
  explicit Arb(unsigned int);
  explicit Arb(long);
  explicit Arb(unsigned long);
  explicit Arb(long long);
  explicit Arb(unsigned long long);

  /// Create an element from this string, see [arb_set_str]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{"1/3", 64};
  ///     std::cout << x;
  ///     // -> (...)
  ///
  Arb(const std::string&, const prec);

  ~Arb() noexcept;

  /// ==* `operator=(Arb)` *==
  /// Reset this element to the one given.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> (...)
  ///
  ///     x = y;
  ///     std::cout << y;
  ///     // -> (...)
  ///
  Arb& operator=(const Arb&) noexcept;
  Arb& operator=(Arb&&) noexcept;

  /// ==* `operator=(integer)` *==
  /// Reset this elemen to an exact element representing this integer.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x;
  ///     x = 1
  ///     std::cout << x;
  ///     // -> (...)
  ///
  Arb& operator=(short);
  Arb& operator=(unsigned short);
  Arb& operator=(int);
  Arb& operator=(unsigned int);
  Arb& operator=(long);
  Arb& operator=(unsigned long);
  Arb& operator=(long long);
  Arb& operator=(unsigned long long);
  Arb& operator=(const mpz_class&);

  /// Return the negative of this element.
  /// This method returns a ball whose lower and upper bound is the negative of
  /// the upper and lower bound, respectively.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{1};
  ///     std::cout << -x;
  ///     // -> (...)
  ///
  Arb operator-() const;

  /// ==* Comparison Operators *==
  /// The comparison operators return a value if the relation is true for all
  /// elements in the ball described by the element, e.g., x < y, returns true
  /// if the relation is true for every element in x and y, they return false if
  /// the relation is false for every element in x and y, and nothing otherwise.
  /// Note that this is different from the semantic in Arb where false is
  /// returned in both of the latter cases.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{mpq_class{1, 3}};
  ///     (x < 1).has_value()
  ///     // -> true
  ///     (x < 1).value()
  ///     // -> true
  ///     (x < x).has_value()
  ///     // -> false
  ///
  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, short);
  LIBEXACTREAL_API friend std::optional<bool> operator==(short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(short, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, unsigned short);
  LIBEXACTREAL_API friend std::optional<bool> operator==(unsigned short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(unsigned short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(unsigned short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(unsigned short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(unsigned short, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(unsigned short, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, int);
  LIBEXACTREAL_API friend std::optional<bool> operator==(int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(int, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, unsigned int);
  LIBEXACTREAL_API friend std::optional<bool> operator==(unsigned int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(unsigned int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(unsigned int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(unsigned int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(unsigned int, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(unsigned int, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, long);
  LIBEXACTREAL_API friend std::optional<bool> operator==(long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(long, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, unsigned long);
  LIBEXACTREAL_API friend std::optional<bool> operator==(unsigned long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(unsigned long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(unsigned long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(unsigned long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(unsigned long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(unsigned long, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, long long);
  LIBEXACTREAL_API friend std::optional<bool> operator==(long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(long long, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, unsigned long long);
  LIBEXACTREAL_API friend std::optional<bool> operator==(unsigned long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(unsigned long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(unsigned long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(unsigned long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(unsigned long long, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(unsigned long long, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, const mpz_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator==(const mpz_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const mpz_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const mpz_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const mpz_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const mpz_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const mpz_class&, const Arb&);

  LIBEXACTREAL_API friend std::optional<bool> operator==(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const Arb&, const mpq_class&);
  LIBEXACTREAL_API friend std::optional<bool> operator==(const mpq_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator!=(const mpq_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<(const mpq_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>(const mpq_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator<=(const mpq_class&, const Arb&);
  LIBEXACTREAL_API friend std::optional<bool> operator>=(const mpq_class&, const Arb&);

  /// Return whether this Arb element exactly represents a floating point
  /// number, i.e., whether its radius is zero, see [arb_is_exact]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{1};
  ///     x.is_exact()
  ///     // -> true
  ///
  ///     Arb y{mpq_class{1, 3}};
  ///     y.is_exact()
  ///     // -> false
  ///
  bool is_exact() const;

  /// Return whether this Arb element does contain neither plus nor minus
  /// infinity, see [arb_is_finite]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{1};
  ///     x.is_finite()
  ///     // -> true
  ///
  ///     Arb y = x / Arb{0};
  ///     y.is_finit()
  ///     // -> false
  ///
  bool is_finite() const;

  /// Return the lower and the upper bound of this ball.
  /// See [arb_get_interval_arf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{mpq_class{1, 3}};
  ///     std::cout << static_cast<std::pair<Arf, Arf>>(x);
  ///     // -> (...)
  ///
  explicit operator std::pair<Arf, Arf>() const;

  /// Return the midpoint of this ball rounded to the closest double.
  /// Note that ties are rounded to even.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{mpq_class{1, 3}};
  ///     static_cast<double>(x)
  ///     // -> (...)
  ///
  explicit operator double() const;

  /// Return the exact midpoint of this ball.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     Arb x{mpq_class{1, 3}};
  ///     std::cout << static_cast<Arf>(x);
  ///     // -> (...)
  ///
  explicit operator Arf() const;

  /// Write this element to the output stream, see [arb_get_str]().
  LIBEXACTREAL_API friend std::ostream& operator<<(std::ostream&, const Arb&);

  /// Return a reference to the underlying [arb_t]() element for direct
  /// manipulation with the C API of Arb.
  ::arb_t& arb_t();

  /// Return a const reference to the underlying [arb_t]() element for direct
  /// manipulation with the C API of Arb.
  const ::arb_t& arb_t() const;

  /// Return an exact zero element, i.e., the ball of radius zero centered at
  /// zero.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::zero();
  ///     // -> (...)
  ///
  static Arb zero();

  /// Return an exact one element, i.e., the ball of radius zero centered at
  /// one.
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::one();
  ///     // -> (...)
  ///
  static Arb one();

  /// Return plus infinity, i.e., the ball of radius zero centered at plus
  /// infinity, see [arb_pos_inf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::pos_inf();
  ///     // -> (...)
  ///
  static Arb pos_inf();

  /// Return minus infinity, i.e., the ball of radius zero centered at minus
  /// infinity, see [arb_neg_inf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::pos_inf();
  ///     // -> (...)
  ///
  static Arb neg_inf();

  /// Return the extended real line, i.e., the interval [-∞,∞], see
  /// [arb_zero_pm_inf]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::zero_pm_inf();
  ///     // -> (...)
  ///
  static Arb zero_pm_inf();

  /// Return an indeterminate, i.e., [NaN±∞] see [arb_indeterminate]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::indeterminate();
  ///     // -> (...)
  ///
  static Arb indeterminate();

  /// Return the interval [-1, 1], i.e., the ball of radius one centered at
  /// zero, see [arb_zero_pm_one]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::zero_pm_one();
  ///     // -> (...)
  ///
  static Arb zero_pm_one();

  /// Return the unit interval [0, 1], i.e., the ball of radius 1/2 centered at
  /// 1/2, see [arb_unit_interval]().
  ///
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << Arb::unit_interval();
  ///     // -> (...)
  ///
  static Arb unit_interval();

  /// Return a random element, see [arb_randtest]().
  static Arb randtest(flint::frandxx&, prec precision, prec magbits);

  /// Retrun a random element, see [arb_randtest_exact]().
  static Arb randtest_exact(flint::frandxx&, prec precision, prec magbits);

  // Return whether elements have the same midpoint and radius.
  bool equal(const Arb&) const;

  /// Swap two elements efficiently.
  /// Used by some STL containers.
  LIBEXACTREAL_API friend void swap(Arb&, Arb&);

  // Syntactic sugar for Yap, so that expresions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  LIBEXACTREAL_LOCAL decltype(auto) operator()(Args&&...) const;

  // Construction from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  LIBEXACTREAL_LOCAL Arb(const yap::ArbExpr<Kind, Tuple>&);

  // Construction from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  LIBEXACTREAL_LOCAL Arb(yap::ArbExpr<Kind, Tuple>&&);

  // Assignment from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  LIBEXACTREAL_LOCAL Arb& operator=(const yap::ArbExpr<Kind, Tuple>&);

 private:
  // The underlying arb_t; use arb_t() to get a reference to it.
  ::arb_t t;
};

}  // namespace exactreal

#endif
