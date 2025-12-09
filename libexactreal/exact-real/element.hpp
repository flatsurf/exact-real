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

#ifndef LIBEXACTREAL_ELEMENT_HPP
#define LIBEXACTREAL_ELEMENT_HPP

#include <gmpxx.h>

#include <boost/blank.hpp>
#include <boost/operators.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11/algorithm.hpp>
#include <e-antic/renfxx_fwd.hpp>
#include <type_traits>
#include <vector>

#include "external/spimpl/spimpl.h"
#include "forward.hpp"
#include "integer_ring.hpp"
#include "rational_field.hpp"

namespace exactreal {

/// An element of a \ref Module.
///
/// An element is presented as a \ref coefficients "linear combination" of the
/// generators of the underlying \ref module.
///
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <exact-real/element.hpp>
///     #include <exact-real/integer_ring.hpp>
///     #include <exact-real/rational_field.hpp>
///     #include <exact-real/number_field.hpp>
///
///     auto M = exactreal::Module<exactreal::RationalField>::make({
///       exactreal::RealNumber::rational(1),
///       exactreal::RealNumber::random()});
///     auto one = M->gen(0);
///     auto x = M->gen(1);
///     std::cout << x + one;
///     // -> ℝ(<...>) + 1
///
template <typename Ring>
class LIBEXACTREAL_API Element 
#ifndef DOXYGEN_DOCUMENTATION_BUILD
                               : boost::additive<Element<Ring>>,
                                 boost::multipliable<Element<Ring>>,
                                 boost::totally_ordered<Element<Ring>>,
                                 // Element can be <= compared with many other
                                 // types. We need to deduplicate these
                                 // comparisons since ElementClass might be
                                 // mpz_class or mpq_class.
                                 boost::mp11::mp_apply<boost::mp11::mp_inherit, boost::mp11::mp_unique<boost::mp11::mp_list<
                                   boost::totally_ordered<Element<Ring>, RealNumber>,
                                   boost::totally_ordered<Element<Ring>, typename Ring::ElementClass>,
                                   boost::totally_ordered<Element<Ring>, mpq_class>,
                                   boost::totally_ordered<Element<Ring>, mpz_class>,
                                   boost::totally_ordered<Element<Ring>, short>,
                                   boost::totally_ordered<Element<Ring>, unsigned short>,
                                   boost::totally_ordered<Element<Ring>, int>,
                                   boost::totally_ordered<Element<Ring>, unsigned int>,
                                   boost::totally_ordered<Element<Ring>, long>,
                                   boost::totally_ordered<Element<Ring>, unsigned long>,
                                   boost::totally_ordered<Element<Ring>, long long>,
                                   boost::totally_ordered<Element<Ring>, unsigned long long>>>>,
                                 boost::multipliable<Element<Ring>, RealNumber>,
                                 // Element supports multiplication and
                                 // division  with many other types. We need to
                                 // deduplicate these operators since
                                 // ElementClass might be mpz_class or
                                 // mpq_class.
                                 boost::mp11::mp_apply<boost::mp11::mp_inherit, boost::mp11::mp_unique<boost::mp11::mp_list<
                                   boost::multiplicative<Element<Ring>, typename Ring::ElementClass>,
                                   boost::multiplicative<Element<Ring>, mpz_class>,
                                   boost::multiplicative<Element<Ring>, mpq_class>,
                                   boost::multiplicative<Element<Ring>, short>,
                                   boost::multiplicative<Element<Ring>, unsigned short>,
                                   boost::multiplicative<Element<Ring>, int>,
                                   boost::multiplicative<Element<Ring>, unsigned int>,
                                   boost::multiplicative<Element<Ring>, long>,
                                   boost::multiplicative<Element<Ring>, unsigned long>,
                                   boost::multiplicative<Element<Ring>, long long>,
                                   boost::multiplicative<Element<Ring>, unsigned long long>>>>
#endif
{
 public:
  /// \name Element(…)
  ///@{
  /// Create the zero element in the trivial module.
  ///
  ///     exactreal::Element<exactreal::RationalField> zero;
  ///     std::cout << zero;
  ///     // -> 0
  ///
  ///     std::cout << *zero.module();
  ///     // -> ℚ-Module()
  ///
  Element();

  /// Create an element in the `parent` module.
  ///
  /// The element is given by \f$ c_i x_i \f$ where \f$ c_i \f$ are the entries
  /// of \p coefficients and \f$ x_i \f$ are the generators of the module. The
  /// number of coefficients must match the number of generators.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     std::cout << a;
  ///     // -> 3*ℝ(<...>) + 2
  ///
  Element(const std::shared_ptr<const Module<Ring>>& parent, const std::vector<typename Ring::ElementClass>& coefficients);

  /// Create \p value as an element in the module generated by 1.
  ///
  ///     auto a = exactreal::Element<exactreal::RationalField>(3);
  ///     std::cout << a;
  ///     // -> 3
  ///
  ///     std::cout << *a.module();
  ///     // -> ℚ-Module(1)
  ///
  explicit Element(const typename Ring::ElementClass& value);

  /// Create an element equal to \p value with coefficients in a \p Ring, i.e.,
  /// coerce the coefficients of \p value to a bigger \p Ring.
  ///
  ///     auto M = exactreal::Module<exactreal::IntegerRing>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::IntegerRing>(M, {2, 3});
  ///     auto b = exactreal::Element<exactreal::RationalField>(a);
  ///    
  ///     std::cout << *b.module();
  ///     // -> ℚ-Module(1, ℝ(<...>))
  ///
  template <bool Enabled = !std::is_same_v<Ring, IntegerRing>, typename = std::enable_if_t<Enabled>>
  Element(const Element<IntegerRing>& value);

  /// Create an element equal to \p value with coefficients in a \p Ring, i.e.,
  /// coerce the coefficients of \p value to a bigger \p Ring.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     auto b = exactreal::Element<exactreal::NumberField>(a);
  ///    
  ///     std::cout << *b.module();
  ///     // -> K-Module(1, ℝ(<...>))
  ///
  template <bool Enabled = !std::is_same_v<Ring, RationalField> && Ring::contains_rationals, typename = std::enable_if_t<Enabled>>
  Element(const Element<RationalField>& value);
  ///@}

  /// \name Coefficient Access
  /// Return the coefficient of the `i`th generator when writing this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a[0]
  ///     // -> 2
  ///
  ///     a[1]
  ///     // -> 3
  ///     
  typename Ring::ElementClass operator[](const size i) const;

  /// Return the coefficients of this element when writing it as a linear
  /// combination of its module's generators.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a.coefficients() == std::vector<mpq_class>{2, 3}
  ///     // -> true
  ///
  std::vector<typename Ring::ElementClass> coefficients() const;

  /// Return the rational coefficients when writing this element as a linear
  /// combination \f$ \sum c_i x_i y_i \f$ where the \f$ x_i \f$ are the
  /// generators of this element's module and the \f$ y_i \f$ are the
  /// generators of the underlying coefficient \p Ring over the rational
  /// numbers.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a.rationalCoefficients() == std::vector<mpq_class>{2, 3}
  ///     // -> true
  ///
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto N = exactreal::Module<exactreal::NumberField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()}, exactreal::NumberField{K});
  ///     auto b = exactreal::Element<exactreal::NumberField>(N, {1, 2 * K->gen() + 3});
  ///     b.rationalCoefficients() == std::vector<mpq_class>{1, 0, 3, 2}
  ///     // -> true
  ///
  std::vector<mpq_class> rationalCoefficients() const;
  ///\}

  /// Return a ball containing this element such that it's accuracy is at least
  /// \p accuracy, defined as in http://arblib.org/arb.html#c.arb_rel_accuracy_bits, i.e.,
  /// the position of the top bit of the midpoint minus the position of the top
  /// bit of the radius minus one.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto M = exactreal::Module<exactreal::NumberField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()}, exactreal::NumberField{K});
  ///     auto a = exactreal::Element<exactreal::NumberField>(M, {1, 2 * K->gen() + 3});
  ///     a.arb(64)
  ///     // -> [1.63175 +/- 5.88e-7]
  ///
  Arb arb(long accuracy) const;

  /// \name Arithmetic with other Module Elements
  ///@{
  /// Add the argument to this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {1, 2});
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a += b;
  ///     a
  ///     // -> 5*ℝ(<...>) + 3
  ///
  /// Through boost's operator machinery, this also provides a regular operator
  /// `+`:
  ///
  ///     a + b
  ///     // -> 8*ℝ(<...>) + 5
  ///
  /// If the operands live in different modules, the result is returned in
  /// their sum:
  ///
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto c = exactreal::Element<exactreal::RationalField>(N, {3, 4});
  ///     c += b;
  ///     c
  ///     // -> 3*ℝ(<...>) + 4*ℝ(<...>) + 5
  ///
  ///     *c.module()
  ///     // -> ℚ-Module(1, ℝ(<...>), ℝ(<...>))
  ///
  Element& operator+=(const Element&);

  /// Subtract the argument from the element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {1, 2});
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a -= b;
  ///     a
  ///     // -> -ℝ(<...>) - 1
  ///
  /// The specifics on \ref operator+= apply equally.
  Element& operator-=(const Element&);

  /// Multiply this element with the argument.
  ///
  /// The result of this operation lives in the module generated by all
  /// possible products of the generators of both modules.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1)});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2});
  ///     a *= a;
  ///     a
  ///     // -> 4
  ///
  ///     *a.module()
  ///     // -> ℚ-Module(1)
  ///
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto b = exactreal::Element<exactreal::RationalField>(N, {2, 3});
  ///     a *= b;
  ///     a
  ///     // -> 12*ℝ(<...>) + 8
  ///
  ///     a *= b;
  ///     a
  ///     // -> 36*ℝ(<...>)^2 + 48*ℝ(<...>) + 16
  ///
  ///     *a.module()
  ///     // -> ℚ-Module(1, ℝ(<...>), ℝ(<...>)^2)
  ///
  Element& operator*=(const Element&);

  /// Return the result of the division of this element by the argument.
  ///
  /// This operation is only supported when the corresponding multivariate
  /// division does not have a remainder, otherwise a `std::nullopt` is
  /// returned:
  /// 
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     auto b = (a * a).truediv(a);
  ///     b.has_value()
  ///     // -> true
  ///
  ///     *b
  ///     // -> 3*ℝ(<...>) + 2
  ///
  ///     auto c = exactreal::Element<exactreal::RationalField>(M, {1, 2});
  ///     b = (a * a).truediv(c);
  ///     b.has_value()
  ///     // -> false
  ///
  std::optional<Element> truediv(const Element&) const;

  /// Return the floor of the quotient of this element by the argument.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {1, 1});
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {1, 2});
  ///
  ///     a.floordiv(b)
  ///     // -> 0
  ///
  mpz_class floordiv(const Element&) const;
  ///@}

  /// Return the product of this element with the given generator.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     auto gen = exactreal::RealNumber::random();
  ///     a * *gen
  ///     // -> 3*ℝ(<...>)*ℝ(<...>) + 2*ℝ(<...>)
  ///
  Element& operator*=(const RealNumber&);

  /// \name Arithmetic Operators with Constants
  ///
  /// Multiplication and division with most primitive types are supported. Note
  /// that not all operators are listed below since some are provided
  /// automatically by boost's operator machinery.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     3 * a / 12
  ///     // -> 3/4*ℝ(<...>) + 1/2
  ///
  /// Currently, there is no addition/subtraction with primitive types. You
  /// need to explicitly convert such objects to the module with generator 1:
  ///
  ///     a + exactreal::Element<exactreal::RationalField>(1)
  ///     // -> 3*ℝ(<...>) + 3
  ///
  ///@{
  Element& operator*=(short);
  Element& operator*=(unsigned short);
  Element& operator*=(int);
  Element& operator*=(unsigned int);
  Element& operator*=(long);
  Element& operator*=(unsigned long);
  Element& operator*=(long long);
  Element& operator*=(unsigned long long);
  Element& operator*=(const mpz_class&);
  Element& operator*=(const mpq_class&);

  /// The template magic here is needed to only explicitly provide the
  /// arithmetic operator with the underlying base ring if they are not already
  /// one of the above ones, i.e., `mpz_class` or `mpq_class` (since C++ does
  /// not want us to define the same function twice.)
  template <bool Enabled = !std::is_same_v<typename Ring::ElementClass, mpz_class> && !std::is_same_v<typename Ring::ElementClass, mpq_class>, typename = std::enable_if_t<Enabled>>
  Element& operator*=(const typename Ring::ElementClass&);

  Element& operator/=(short);
  Element& operator/=(unsigned short);
  Element& operator/=(int);
  Element& operator/=(unsigned int);
  Element& operator/=(long);
  Element& operator/=(unsigned long);
  Element& operator/=(long long);
  Element& operator/=(unsigned long long);
  Element& operator/=(const mpz_class&);
  Element& operator/=(const mpq_class&);

  /// The template magic here is needed to only explicitly provide the
  /// arithmetic operator with the underlying base ring if they are not already
  /// one of the above ones, i.e., `mpz_class` or `mpq_class` (since C++ does
  /// not want us to define the same function twice.)
  template <bool Enabled = !std::is_same_v<typename Ring::ElementClass, mpz_class> && !std::is_same_v<typename Ring::ElementClass, mpq_class>, typename = std::enable_if_t<Enabled>>
  Element& operator/=(const typename Ring::ElementClass&);
  ///@}

  /// Return the negative of this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     -a
  ///     // -> -3*ℝ(<...>) - 2
  ///
  Element operator-() const;

  /// Return the integer floor of this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {3, 1});
  ///     a.floor()
  ///     // -> 3
  ///
  mpz_class floor() const;

  /// Return the integer ceiling of this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {3, 1});
  ///     a.ceil()
  ///     // -> 4
  ///
  mpz_class ceil() const;

  /// Return whether this element is a unit, i.e., whether its inverse exists in
  /// the parent module.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a.unit()
  ///     // -> false
  ///
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {2, 0});
  ///     b.unit()
  ///     // -> true
  ///
  bool unit() const;

  /// \name Relational Operators
  ///
  /// The operators `<`, `<=`, `==`, `!=`, `>=`, `>` are available to compare
  /// elements to other elements, generators, integers, rationals, ...
  /// 
  /// Some of these operators are not listed explicitly because they are
  /// provide through boost operators.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto b = exactreal::Element<exactreal::RationalField>(N, {2, 3});
  ///
  ///     a == b
  ///     // -> false
  ///
  ///     a < b || a > b
  ///     // -> true
  ///
  ///     a > 2
  ///     // -> true
  ///
  ///     a < 5
  ///     // -> true
  ///
  ///@{
  bool operator==(const Element&) const;
  bool operator<(const Element&) const;

  bool operator==(const RealNumber&) const;
  bool operator<(const RealNumber&) const;
  bool operator>(const RealNumber&) const;

  bool operator==(const mpq_class&) const;
  bool operator<(const mpq_class&) const;
  bool operator>(const mpq_class&) const;

  bool operator==(const mpz_class&) const;
  bool operator<(const mpz_class&) const;
  bool operator>(const mpz_class&) const;

  bool operator==(short) const;
  bool operator<(short) const;
  bool operator>(short) const;

  bool operator==(unsigned short) const;
  bool operator<(unsigned short) const;
  bool operator>(unsigned short) const;

  bool operator==(int) const;
  bool operator<(int) const;
  bool operator>(int) const;

  bool operator==(unsigned int) const;
  bool operator<(unsigned int) const;
  bool operator>(unsigned int) const;

  bool operator==(long) const;
  bool operator<(long) const;
  bool operator>(long) const;

  bool operator==(unsigned long) const;
  bool operator<(unsigned long) const;
  bool operator>(unsigned long) const;

  bool operator==(long long) const;
  bool operator<(long long) const;
  bool operator>(long long) const;

  bool operator==(unsigned long long) const;
  bool operator<(unsigned long long) const;
  bool operator>(unsigned long long) const;
  ///@}

  /// \name Cast Operators
  ///@{

  /// Return whether this element is non-zero.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     static_cast<bool>(a)
  ///     // -> true
  ///
  explicit operator bool() const;

  /// Return the closest double to this element; ties are rounded to even.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 0});
  ///     static_cast<double>(a)
  ///     // -> 2
  ///
  explicit operator double() const;

  /// Return this element as an integer if it is an integer, otherwise return
  /// `std::nullopt`.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     static_cast<std::optional<mpz_class>>(a).has_value()
  ///     // -> false
  ///
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {2, 0});
  ///     static_cast<std::optional<mpz_class>>(b).value()
  ///     // -> 2
  ///     
  explicit operator std::optional<mpz_class>() const;

  /// Return this element as a rational number if it is a rational, otherwise
  /// return `std::nullopt`.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     static_cast<std::optional<mpq_class>>(a).has_value()
  ///     // -> false
  ///
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {2, 0});
  ///     static_cast<std::optional<mpq_class>>(b).value()
  ///     // -> 2
  ///     
  explicit operator std::optional<mpq_class>() const;
  ///@}

  /// Return the parent module of this element.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     a.module() == M
  ///     // -> true
  ///
  const std::shared_ptr<const Module<Ring>> module() const;

  /// Make this element an element of \p module. All the module generators used
  /// in this element must be present, e.g., module must be a supermodule of
  /// this element's module.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1)});
  ///
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2});
  ///     a
  ///     // -> 2
  ///
  ///     a.promote(N);
  ///     a
  ///     // -> 2
  ///
  ///     a.module() == N
  ///     // -> true
  ///
  Element& promote(const std::shared_ptr<const Module<Ring>>& module);

  /// Make this element an element of a module defined over the same ring which
  /// has only the generators necessary to represent this element. This method
  /// is especially useful after chains of arithmetic operations involving
  /// products as these introduce lots of generators that might not be necessary
  /// in the final result.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto a = exactreal::Element<exactreal::RationalField>(M, {2, 3});
  ///     auto b = exactreal::Element<exactreal::RationalField>(M, {4, 0});
  ///
  ///     a *= a;
  ///     a -= b;
  ///     a
  ///     // -> 9*ℝ(<...>)^2 + 12*ℝ(<...>)
  ///
  ///     *a.module()
  ///     // -> ℚ-Module(1, ℝ(<...>), ℝ(<...>)^2)
  ///
  ///     a.simplify();
  ///     *a.module()
  ///     // -> ℚ-Module(ℝ(<...>), ℝ(<...>)^2)
  ///
  Element& simplify();

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Element<R>&);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

template <typename Ring, typename... Args>
Element(const std::shared_ptr<const Module<Ring>>&, Args...) -> Element<Ring>;

Element(int)->Element<IntegerRing>;

Element(unsigned int)->Element<IntegerRing>;

Element(long)->Element<IntegerRing>;

Element(unsigned long)->Element<IntegerRing>;

Element(const mpz_class&)->Element<IntegerRing>;

Element(const mpq_class&)->Element<RationalField>;

Element(const RealNumber&)->Element<IntegerRing>;

template <typename R>
LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const Element<R>&);

}  // namespace exactreal

namespace std {
template <typename Ring>
struct LIBEXACTREAL_API hash<exactreal::Element<Ring>> {
  size_t LIBEXACTREAL_API operator()(const exactreal::Element<Ring>&) const noexcept;
};
}  // namespace std

#endif
