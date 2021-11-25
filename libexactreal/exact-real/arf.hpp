/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019      Vincent Delecroix
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
 *********************************************************************/

/// C++ Wrappers for a floating point number [arf_t]().

#ifndef LIBEXACTREAL_ARF_HPP
#define LIBEXACTREAL_ARF_HPP

#include <arf.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>

#include <boost/operators.hpp>
#include <memory>

#include "local.hpp"
#include "yap/terminal.hpp"

namespace exactreal {

/// A wrapper for [arf_t]() elements, i.e., arbitrary precision floating point
/// numbers, so we get C++ style memory management. We use some Yap magic to
/// get nice operators (which is tricky otherwise because we cannot pass the
/// additional prec and rnd parameters to operators in C++.)
///
/// If you don't like that magic and only want memory management, just create
/// elements and then use Arb functions directly:
///
///     #include <exact-real/arf.hpp>
///     exactreal::Arf x, y;
///
///     arf_add(x.arf_t(), x.arf_t(), y.arf_t(), 64, ARF_RND_NEAR);
///
/// Using yap this can be rewritten as any of the following:
///
///     #include <exact-real/yap/arf.hpp>
///
///     exactreal::Arf x, y;
///     x += y(64)(exactreal::Arf::Round::NEAR);
///     x = (x + y)(64, exactreal::Arf::Round::NEAR);
///
/// Note that the latter might use an additional temporary Arf. See the
/// yap/arf.hpp header for more details.
///
/// Note that methods here are usually named as their counterparts in arf.h with
/// the leading arf_ removed.
///
/// Note that many methods provided by arb's C API are not yet provided by this
/// C++ wrapper. If something is missing for you, please let us know on our
/// [GitHub issues page](https://github.com/flatsurf/arbxx/issues).
class LIBEXACTREAL_API Arf : yap::Terminal<Arf, yap::ArfExpr>,
                             boost::totally_ordered<Arf>,
                             boost::totally_ordered<Arf, short>,
                             boost::totally_ordered<Arf, unsigned short>,
                             boost::totally_ordered<Arf, int>,
                             boost::totally_ordered<Arf, unsigned int>,
                             boost::totally_ordered<Arf, long>,
                             boost::totally_ordered<Arf, unsigned long>,
                             boost::totally_ordered<Arf, long long>,
                             boost::totally_ordered<Arf, unsigned long long>,
                             boost::totally_ordered<Arf, const mpz_class&>,
                             boost::shiftable<Arf, long> {
 public:
  /// Rounding modes for arithmetic operations.
  enum class Round {
    /// See [ARF_RND_NEAR]().
    NEAR = ARF_RND_NEAR,
    /// See [ARF_RND_DOWN]().
    DOWN = ARF_RND_DOWN,
    /// See [ARF_RND_UP]().
    UP = ARF_RND_UP,
    /// See [ARF_RND_FLOOR]().
    FLOOR = ARF_RND_FLOOR,
    /// See [ARF_RND_CEIL]().
    CEIL = ARF_RND_CEIL,
  };

  /// Create a zero element.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x;
  ///     std::cout << x;
  ///     // -> 0
  ///
  Arf() noexcept;

  /// Create a copy of an element.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arf y{x};
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf(const Arf&) noexcept;

  /// Create an element from another one.
  ///
  ///     #include <exact-real/arf.hpp>
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
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{"abc", 16, -8};
  ///     std::cout << x;
  ///     // -> 10.7344=687p-6
  ///
  Arf(const std::string& mantissa, int base, long exponent);

  /// Create the element `mantissa * 2^exponent`.
  ///
  ///     #include <exact-real/arf.hpp>
  ///     exactreal::Arf x{2748, -8};
  ///     std::cout << x;
  ///     // -> 10.7344=687p-6
  ///
  explicit Arf(const mpz_class& mantissa, long exponent=0);

  /// ==* Arf(integer) *==
  /// Create a floating point number from an integer.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{2748};
  ///     std::cout << x;
  ///     // -> 2748
  ///
  explicit Arf(short);
  explicit Arf(unsigned short);
  explicit Arf(int);
  explicit Arf(unsigned int);
  explicit Arf(long);
  explicit Arf(unsigned long);
  explicit Arf(long long);
  explicit Arf(unsigned long long);

  /// Create a floating point element identical to this double.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{0.3};
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  explicit Arf(double);

  ~Arf() noexcept;

  /// Make this element an identical copy of the provided one.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1}, y;
  ///     y = x;
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(const Arf&) noexcept;

  /// Move the value of the element into this one.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(Arf&&) noexcept;

  /// ==* operator=(integer) *==
  /// Reset the value to this integer.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x;
  ///     x = 1;
  ///     std::cout << x;
  ///     // -> 1
  ///
  Arf& operator=(short);
  Arf& operator=(unsigned short);
  Arf& operator=(int);
  Arf& operator=(unsigned);
  Arf& operator=(long);
  Arf& operator=(unsigned long);
  Arf& operator=(long long);
  Arf& operator=(unsigned long long);
  Arf& operator=(const mpz_class&);

  /// Reset the value to the value of this double.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x;
  ///     x = 0.3;
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  Arf& operator=(double);

  /// Return the negative of this value, see [arf_neg]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1};
  ///     exactreal::Arf y = -x;
  ///     std::cout << y;
  ///     // -> -1
  ///
  Arf operator-() const;

  /// Return the double precision floating point number closest to this
  /// element, see [arf_get_d]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1, -65536};
  ///     std::cout << static_cast<double>(x);
  ///     // -> 0
  ///
  ///     #include <exact-real/yap/arf.hpp>
  ///
  ///     x = (x * exactreal::Arf{1, 65536})(64, exactreal::Arf::Round::NEAR);
  ///     std::cout << static_cast<double>(x);
  ///     // -> 1
  ///
  explicit operator double() const;

  /// Return the absolute value of this element, see [arf_abs]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-1};
  ///     exactreal::Arf y = x.abs();
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf abs() const;

  /// Return the largest integer which is less than or equal to this element,
  /// see [arf_floor]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-.5};
  ///     std::cout << x.floor();
  ///     // -> -1
  ///
  mpz_class floor() const;

  /// Return the largest integer which is greater than or equal to this
  /// element, see [arf_ceil]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-.5};
  ///     std::cout << x.ceil();
  ///     // -> 0
  ///
  mpz_class ceil() const;

  /// Return the mantissa of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See [arf_get_fmpz_2exp]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-1};
  ///     std::cout << x.mantissa();
  ///     // -> -1
  ///
  mpz_class mantissa() const;

  /// Return the exponent of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See [arf_get_fmpz_2exp]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///     exactreal::Arf x{-1};
  ///     std::cout << x.exponent();
  ///     // -> 0
  ///
  mpz_class exponent() const;

  /// Return the floor of the logarithm of this element to base 2.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1025};
  ///     x.logb()
  ///     // -> 10
  ///
  long logb() const;

  /// Return this element shifted down by `e`, i.e., multiplied by `2^-e`.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-1};
  ///     x >>= 8;
  ///     std::cout << x;
  ///     // -> -0.00390625=-1p-8
  ///
  Arf& operator>>=(const long e);

  /// Return this element shifted up by `e`, i.e., multiplied by `2^e`.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{-1};
  ///     x <<= 8;
  ///     std::cout << x;
  ///     // -> -256
  ///
  Arf& operator<<=(const long e);

  /// ==* Comparison Operators *==
  /// Comparison operators are available for comparison with other `Arf`
  /// instances and integer types.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1};
  ///     x < 2
  ///     // -> true
  ///
  ///     x <= mpz_class{1}
  ///     // -> true
  ///
  LIBEXACTREAL_API friend bool operator<(const Arf&, const Arf&);
  LIBEXACTREAL_API friend bool operator==(const Arf&, const Arf&);

  LIBEXACTREAL_API friend bool operator<(const Arf&, short);
  LIBEXACTREAL_API friend bool operator>(const Arf&, short);
  LIBEXACTREAL_API friend bool operator==(const Arf&, short);

  LIBEXACTREAL_API friend bool operator<(const Arf&, unsigned short);
  LIBEXACTREAL_API friend bool operator>(const Arf&, unsigned short);
  LIBEXACTREAL_API friend bool operator==(const Arf&, unsigned short);

  LIBEXACTREAL_API friend bool operator<(const Arf&, int);
  LIBEXACTREAL_API friend bool operator>(const Arf&, int);
  LIBEXACTREAL_API friend bool operator==(const Arf&, int);

  LIBEXACTREAL_API friend bool operator<(const Arf&, unsigned int);
  LIBEXACTREAL_API friend bool operator>(const Arf&, unsigned int);
  LIBEXACTREAL_API friend bool operator==(const Arf&, unsigned int);

  LIBEXACTREAL_API friend bool operator<(const Arf&, long);
  LIBEXACTREAL_API friend bool operator>(const Arf&, long);
  LIBEXACTREAL_API friend bool operator==(const Arf&, long);

  LIBEXACTREAL_API friend bool operator<(const Arf&, unsigned long);
  LIBEXACTREAL_API friend bool operator>(const Arf&, unsigned long);
  LIBEXACTREAL_API friend bool operator==(const Arf&, unsigned long);

  LIBEXACTREAL_API friend bool operator<(const Arf&, long long);
  LIBEXACTREAL_API friend bool operator>(const Arf&, long long);
  LIBEXACTREAL_API friend bool operator==(const Arf&, long long);

  LIBEXACTREAL_API friend bool operator<(const Arf&, unsigned long long);
  LIBEXACTREAL_API friend bool operator>(const Arf&, unsigned long long);
  LIBEXACTREAL_API friend bool operator==(const Arf&, unsigned long long);

  LIBEXACTREAL_API friend bool operator<(const Arf&, const mpz_class&);
  LIBEXACTREAL_API friend bool operator>(const Arf&, const mpz_class&);
  LIBEXACTREAL_API friend bool operator==(const Arf&, const mpz_class&);

  /// Return a random element, see [arf_randtest]().
  ///
  ///     #include <exact-real/arf.hpp>
  ///     #include <flint/flintxx/frandxx.h>
  ///
  ///     flint::frandxx rand;
  ///     auto a = exactreal::Arf::randtest(rand, 64, 16);
  ///     auto b = exactreal::Arf::randtest(rand, 64, 16);
  ///     a == b
  ///     // -> false
  ///
  LIBEXACTREAL_API static Arf randtest(flint::frandxx&, prec precision, prec magbits);

  /// Write this element to the output stream.
  ///
  ///     #include <exact-real/arf.hpp>
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
  LIBEXACTREAL_API friend std::ostream& operator<<(std::ostream&, const Arf&);

  /// Swap two elements efficiently.
  /// This is used by some STL containers.
  ///
  ///     #include <exact-real/arf.hpp>
  ///
  ///     exactreal::Arf x{1}, y;
  ///     std::cout << "x = " << x << ", y = " << y;
  ///     // -> x = 1, y = 0
  ///
  ///     swap(x, y);
  ///     std::cout << "x = " << x << ", y = " << y;
  ///     // -> x = 0, y = 1
  ///
  LIBEXACTREAL_API friend void swap(Arf& lhs, Arf& rhs);

  /// Return a reference to the underlying [arf_t]() element for direct
  /// manipulation with C API of Arb.
  ::arf_t& arf_t();

  /// Return a reference to the underlying [arf_t]() element for direct
  /// manipulation with C API of Arb.
  const ::arf_t& arf_t() const;

  // Syntactic sugar for Yap, so that expresions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  LIBEXACTREAL_LOCAL decltype(auto) operator()(Args&&...) const;

  // Construction from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  LIBEXACTREAL_LOCAL Arf(const yap::ArfExpr<Kind, Tuple>&);

  // Construction from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  LIBEXACTREAL_LOCAL Arf(yap::ArfExpr<Kind, Tuple>&&);

  // Assignment from evaluating a Yap expression.
  template <boost::yap::expr_kind Kind, typename Tuple>
  LIBEXACTREAL_LOCAL Arf& operator=(const yap::ArfExpr<Kind, Tuple>&);

 private:
  // The underlying arf_t; use arf_t() to get a reference to it.
  ::arf_t t;
};

}  // namespace exactreal

namespace std {

template <>
struct LIBEXACTREAL_API hash<exactreal::Arf> {
  size_t LIBEXACTREAL_API operator()(const exactreal::Arf&) const;
};

}  // namespace std

#endif
