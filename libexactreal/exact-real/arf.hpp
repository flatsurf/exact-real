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

#ifndef LIBEXACTREAL_ARF_HPP
#define LIBEXACTREAL_ARF_HPP

#include <flint/flint.h>

#if __FLINT_RELEASE < 30000
#include <arf.h>
#else
#include <flint/arf.h>
#endif

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <memory>

#include "yap/terminal.hpp"


namespace exactreal {

/// A wrapper for arf_t elements so we get C++ style memory management.
/// We use some Yap magic to get nice operators (which is tricky otherwise
/// because we cannot pass the additional prec and rnd parameters to operators
/// in C++.)
///
/// If you don't like that magic and only want memory management, just create
/// elements
///
///     #include <exact-real/arf.hpp>
///     exactreal::Arf x, y;
///
/// and then use the Arf functions directly:
///
///     arf_add(x.arf_t(), x.arf_t(), y.arf_t(), 64, ARF_RND_NEAR);
///
/// Using yap this can be rewritten as any of the following:
///
///     #include <exact-real/yap/arf.hpp>
///
///     x += y(64)(exactreal::Arf::Round::NEAR);
///     x = (x + y)(64, exactreal::Arf::Round::NEAR);
///
/// Note that the latter might use an additional temporary Arf. See the
/// yap/arf.hpp header for more details.
///
/// Note that methods here are usually named as their counterparts in arf.h with
/// the leading arf_ removed.
///
/// \note We do not really recommend that you use the yap interface. We are
/// considering to remove it because it is not terribly useful in practice and
/// annoying to maintain.
///
/// \note This class has nothing to do with exact-real but should be
/// implemented by FLINT itself. Eventually we would like to migrate these
/// classes into a proper C++ wrapper that provides C++ memory management for
/// FLINT.
class LIBEXACTREAL_API Arf
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : yap::Terminal<Arf, yap::ArfExpr>, boost::totally_ordered<Arf>, boost::totally_ordered<Arf, long>, boost::shiftable<Arf, long>
#endif
{
 public:
  /// Rounding modes for arithmetic operations.
  enum class Round {
    /// See
    /// \rst{:c:macro:`ARF_RND_NEAR`.} 
    NEAR = ARF_RND_NEAR,
    /// See
    /// \rst{:c:macro:`ARF_RND_DOWN`.}
    DOWN = ARF_RND_DOWN,
    /// See
    /// \rst{:c:macro:`ARF_RND_UP`.}
    UP = ARF_RND_UP,
    /// See
    /// \rst{:c:macro:`ARF_RND_FLOOR`.}
    FLOOR = ARF_RND_FLOOR,
    /// See
    /// \rst{:c:macro:`ARF_RND_CEIL`.}
    CEIL = ARF_RND_CEIL,
  };

  /// Create a zero element.
  ///
  ///     exactreal::Arf x;
  ///     std::cout << x;
  ///     // -> 0
  ///
  Arf() noexcept;
  /// Create a copy of an element.
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arf y{x};
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf(const Arf&) noexcept;
  /// Create an element from another one.
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arf y{std::move(x)};
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf(Arf&&) noexcept;
  /// Create the element `mantissa * 2^exponent` where `mantissa` is an integer
  /// written in `base`.
  ///
  ///     exactreal::Arf x{"abc", 16, -8};
  ///     std::cout << x;
  ///     // -> 10.7344=687p-6
  ///
  Arf(const std::string& mantissa, int base, long exponent);
  /// Create the element `mantissa * 2^exponent`.
  ///
  ///     exactreal::Arf x{2748, -8};
  ///     std::cout << x;
  ///     // -> 10.7344=687p-6
  ///
  Arf(const mpz_class& mantissa, long exponent) noexcept;

  /// Create a floating point number from an integer.
  ///
  ///     exactreal::Arf x{2748};
  ///     std::cout << x;
  ///     // -> 2748
  ///
  explicit Arf(const int) noexcept;
  explicit Arf(const slong) noexcept;

  /// Create a floating point element identical to this double.
  ///
  ///     exactreal::Arf x{0.3};
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  explicit Arf(const double) noexcept;

  ~Arf() noexcept;

  /// Make this element an identical copy of the provided one.
  ///
  ///     exactreal::Arf x{1}, y;
  ///     y = x;
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(const Arf&) noexcept;

  /// Move the value of the element into this one.
  ///
  ///     exactreal::Arf x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(Arf&&) noexcept;

  /// Reset the value to this integer.
  ///
  ///     exactreal::Arf x;
  ///     x = 1;
  ///     std::cout << x;
  ///     // -> 1
  ///
  Arf& operator=(int) noexcept;
  Arf& operator=(slong) noexcept;

  /// Reset the value to the value of this double.
  ///
  ///     exactreal::Arf x;
  ///     x = 0.3;
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  Arf& operator=(double) noexcept;

  /// Return the negative of this value, see \rst{:c:func:`arf_neg`.}
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arf y = -x;
  ///     std::cout << y;
  ///     // -> -1
  ///
  Arf operator-() const noexcept;

  /// Return the double precision floating point number closest to this
  /// element, see \rst{:c:func:`arf_get_d`.}
  ///
  ///     exactreal::Arf x{1, -65536};
  ///     std::cout << static_cast<double>(x);
  ///     // -> 0
  ///
  ///     x = (x * exactreal::Arf{1, 65536})(64, exactreal::Arf::Round::NEAR);
  ///     std::cout << static_cast<double>(x);
  ///     // -> 1
  ///
  explicit operator double() const noexcept;

  /// Return the absolute value of this element, see \rst{:c:func:`arf_abs`.}
  ///
  ///     exactreal::Arf x{-1};
  ///     exactreal::Arf y = x.abs();
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf abs() const noexcept;

  /// Return the largest integer which is less than or equal to this element,
  /// see \rst{:c:func:`arf_floor`.}
  ///
  ///     exactreal::Arf x{-.5};
  ///     std::cout << x.floor();
  ///     // -> -1
  ///
  mpz_class floor() const noexcept;

  /// Return the largest integer which is greater than or equal to this
  /// element, see \rst{:c:func:`arf_ceil`.}
  ///
  ///     exactreal::Arf x{-.5};
  ///     std::cout << x.ceil();
  ///     // -> 0
  ///
  mpz_class ceil() const noexcept;

  /// Return the mantissa of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See \rst{:c:func:`arf_get_fmpz_2exp`.}
  ///
  ///     exactreal::Arf x{-1};
  ///     std::cout << x.mantissa();
  ///     // -> -1
  ///
  mpz_class mantissa() const noexcept;

  /// Return the exponent of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See \rst{:c:func:`arf_get_fmpz_2exp`.}
  ///
  ///     exactreal::Arf x{-1};
  ///     std::cout << x.exponent();
  ///     // -> 0
  ///
  mpz_class exponent() const noexcept;

  /// Return the floor of the logarithm of this element to base 2.
  ///
  ///     exactreal::Arf x{1025};
  ///     x.logb()
  ///     // -> 10
  ///
  long logb() const noexcept;

  /// Return this element shifted down by `e`, i.e., multiplied by `2^-e`.
  ///
  ///     exactreal::Arf x{-1};
  ///     x >>= 8;
  ///     std::cout << x;
  ///     // -> -0.00390625=-1p-8
  ///
  Arf& operator>>=(const long e) noexcept;

  /// Return this element shifted up by `e`, i.e., multiplied by `2^e`.
  ///
  ///     exactreal::Arf x{-1};
  ///     x <<= 8;
  ///     std::cout << x;
  ///     // -> -256
  ///
  Arf& operator<<=(const long e) noexcept;

  /// \name Relational operators
  ///
  /// Comparison operators are available for comparison with other `Arf`
  /// instances and integer types.
  ///
  ///     exactreal::Arf x{1};
  ///     x < 2
  ///     // -> true
  ///
  ///     x <= exactreal::Arf{1}
  ///     // -> true
  ///
  /// Note that operators not explicitly listed here are provided through boost
  /// operators:
  ///@{
  bool operator<(const Arf&) const noexcept;
  bool operator==(const Arf&) const noexcept;

  bool operator<(long) const noexcept;
  bool operator>(long) const noexcept;
  bool operator==(long) const noexcept;
  ///@}

  /// Return a random element, see \rst{:c:func:`arf_randtest`.}
  ///
  ///     #include <flint/flint.h>
  ///
  ///     flint_rand_t rnd;
  ///     flint_randinit(rnd);
  ///     auto a = exactreal::Arf::randtest(rnd, 64, 16);
  ///     auto b = exactreal::Arf::randtest(rnd, 64, 16);
  ///     a == b
  ///     // -> false
  ///
  static Arf randtest(flint_rand_t, prec precision, prec magbits) noexcept;

  /// Write this element to the output stream.
  ///
  ///     exactreal::Arf x{1};
  ///     std::cout << x;
  ///     // -> 1
  ///
  /// Non-integers and large numbers are written as their double approximation
  /// and exactly, with an exponent for base two, here is for example `2^-16`:
  ///
  ///     exactreal::Arf x{1, -16};
  ///     std::cout << x;
  ///     // -> 1.52588e-05=1p-16
  ///
  friend std::ostream& operator<<(std::ostream&, const Arf&) LIBEXACTREAL_API;


  /// Return a reference to the underlying \rst{:c:type:`arf_t`} element for direct
  /// manipulation with C API of Arb.
  ::arf_t& arf_t() noexcept;

  /// Return a reference to the underlying \rst{:c:type:`arf_t`} element for direct
  /// manipulation with C API of Arb.
  const ::arf_t& arf_t() const noexcept;

#ifndef DOXYGEN_DOCUMENTATION_BUILD
  // Syntactic sugar for Yap, so that expressions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  decltype(auto) operator()(Args&&...) const noexcept LIBEXACTREAL_LOCAL;

  // Construction from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arf(const yap::ArfExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arf(yap::ArfExpr<Kind, Tuple>&&) noexcept LIBEXACTREAL_LOCAL;

  // Assignment from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arf& operator=(const yap::ArfExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;
#endif

 private:
  /// The underlying \rst{:c:type:`arf_t`; use :cpp:func:`arf_t()
  /// <exactreal::Arb::arf_t>`} to get a reference to it.
  ::arf_t t;
};

/// Swap two elements efficiently.
/// This is used by some STL containers.
///
///     exactreal::Arf x{1}, y;
///     std::cout << "x = " << x << ", y = " << y;
///     // -> x = 1, y = 0
///
///     swap(x, y);
///     std::cout << "x = " << x << ", y = " << y;
///     // -> x = 0, y = 1
///
void LIBEXACTREAL_API swap(Arf& lhs, Arf& rhs);

}  // namespace exactreal

namespace std {

template <>
struct hash<exactreal::Arf> {
  size_t operator()(const exactreal::Arf&) const;
};

}  // namespace std

#endif
