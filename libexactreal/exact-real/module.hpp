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

#ifndef LIBEXACTREAL_MODULE_HPP
#define LIBEXACTREAL_MODULE_HPP

#include <boost/operators.hpp>
#include <vector>

#include "external/spimpl/spimpl.h"
#include "forward.hpp"

namespace exactreal {

/// A submodule of the real numbers.
///
/// Use \ref make to create a module.
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
/// Note that modules are always wrapped in a shared pointer so that elements
/// can keep their module alive.
///
template <typename Ring>
class LIBEXACTREAL_API Module
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : boost::equality_comparable<Module<Ring>>,
    public std::enable_shared_from_this<Module<Ring>>
#endif
{
 public:
  /// The generators of a module are (independent) \ref RealNumber "real numbers".
  using Basis = std::vector<std::shared_ptr<const RealNumber>>;

  /// Create a module with the given generators over the trivial \p Ring.
  ///
  /// This function is meant to be used with rings that are not parametrized,
  /// such as the integer or the rationals.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     *M
  ///     // -> ℚ-Module(1, ℝ(<...>))
  ///
  static std::shared_ptr<const Module<Ring>> make(const Basis&);

  /// Create a module with the given generators over the given \p Ring.
  ///
  ///     #include <e-antic/renf_class.hpp>
  ///     #include <e-antic/renf_elem_class.hpp>
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto M = exactreal::Module<exactreal::NumberField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()}, exactreal::NumberField{K});
  ///     *M
  ///     // -> K-Module(1, ℝ(<...>))
  ///
  static std::shared_ptr<const Module<Ring>> make(const Basis&, const Ring&);

  /// Return the \p Ring underlying this module.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto M = exactreal::Module<exactreal::NumberField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()}, exactreal::NumberField{K});
  ///     *M->ring().parameters
  ///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
  ///
  const Ring& ring() const;

  /// Return the rank of this module, i.e., the number of generators.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     M->rank()
  ///     // -> 2
  ///
  size rank() const;

  /// Return whether this module is trivially a submodule of \p other i.e.,
  /// whether all the generators of this module are also generators of \p other.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1)});
  ///     M->submodule(*N)
  ///     // -> false
  ///
  ///     N->submodule(*M)
  ///     // -> true
  ///
  bool submodule(const Module& other) const;

  /// Return the generators of this module.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     M->basis().size()
  ///     // -> 2
  ///
  const Basis& basis() const;

  /// Return the i-th generator of this module.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     M->gen(0)
  ///     // -> 1
  ///
  Element<Ring> gen(size i) const;

  /// Return the zero element in this module.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     M->zero()
  ///     // -> 0
  ///
  Element<Ring> zero() const;

  /// Return a one element in this module.
  /// If there is no such element, an exception is raised.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     M->one()
  ///     // -> 1
  ///
  Element<Ring> one() const;

  /// Return whether two modules are indistinguishable, i.e., they have the
  /// same generators in the same order and are defined over the same ring.
  ///
  ///     auto M = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1),
  ///       exactreal::RealNumber::random()});
  ///     auto N = exactreal::Module<exactreal::RationalField>::make({
  ///       exactreal::RealNumber::rational(1)});
  ///
  ///     *M == *N
  ///     // -> false
  ///
  ///     *M != *N
  ///     // -> true
  ///
  bool operator==(const Module<Ring>& rhs) const;

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Module<R>&);

  static std::shared_ptr<const Module<Ring>> span(const std::shared_ptr<const Module<Ring>>&, const std::shared_ptr<const Module<Ring>>&);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::unique_impl_ptr<Implementation> impl;

  Module(spimpl::unique_impl_ptr<Implementation>&&) LIBEXACTREAL_LOCAL;
};

template <typename R>
LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const Module<R>&);

}  // namespace exactreal

#endif
