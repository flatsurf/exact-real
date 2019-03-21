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

#include "exact-real/module.hpp"
#include "exact-real/element.hpp"
#include "exact-real/real_number.hpp"

using namespace exactreal;
using std::shared_ptr;
using std::vector;

namespace {
template <typename Ring>
struct ModuleImplementation {
  using Basis = vector<shared_ptr<RealNumber>>;

  explicit ModuleImplementation(const Basis& basis, long precision)
      : basis(basis), precision(precision) {
    // See https://github.com/flatsurf/exact-real/issues/5 for missing input
    // checks.
  }

  Basis basis;
  long precision;
};

template <typename Ring>
struct ModuleImplementationWithoutParameters : ModuleImplementation<Ring> {
  using ModuleImplementation<Ring>::ModuleImplementation;
};

template <typename Ring>
struct ModuleImplementationWithParameters : ModuleImplementation<Ring> {
  explicit ModuleImplementationWithParameters(
      const typename ModuleImplementation<Ring>::Basis& basis,
      const typename Ring::Parameters& parameters, long precision)
      : ModuleImplementation<Ring>(basis, precision), parameters(parameters) {}

  const typename Ring::Parameters& parameters;
};
}  // namespace

namespace exactreal {
template <typename Ring>
struct Module<Ring>::Implementation
    : std::conditional_t<has_parameters<Ring>::value,
                         ModuleImplementationWithParameters<Ring>,
                         ModuleImplementationWithoutParameters<Ring>> {
  using std::conditional_t<
      has_parameters<Ring>::value, ModuleImplementationWithParameters<Ring>,
      ModuleImplementationWithoutParameters<Ring>>::conditional_t;
};
}  // namespace exactreal

namespace exactreal {
template <typename Ring>
template <
    typename RingWithoutParameters,
    typename std::enable_if_t<std::is_same_v<Ring, RingWithoutParameters> &&
                                  !has_parameters<RingWithoutParameters>::value,
                              int>>
Module<Ring>::Module(const vector<shared_ptr<RealNumber>>& basis,
                     long precision)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis,
                                                                  precision)) {}

template <typename Ring>
template <
    typename RingWithParameters,
    typename std::enable_if_t<std::is_same_v<Ring, RingWithParameters> &&
                                  has_parameters<RingWithParameters>::value,
                              int>>
Module<Ring>::Module(const vector<shared_ptr<RealNumber>>& basis,
                     const typename RingWithParameters::Parameters& parameters,
                     long precision)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(
          basis, parameters, precision)) {}

template <typename Ring>
size_t Module<Ring>::rank() const {
  return impl->basis.size();
}

template <typename Ring>
Element<Ring> Module<Ring>::zero() const {
  return Element<Ring>(*this, vector<typename Ring::ElementClass>(rank()));
}

template <typename Ring>
vector<shared_ptr<RealNumber>> const& Module<Ring>::gens() const {
  return impl->basis;
}
}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"

template struct exactreal::Module<IntegerRingTraits>;
template exactreal::Module<IntegerRingTraits>::Module(
    const vector<shared_ptr<RealNumber>>&, long);
template struct exactreal::Module<RationalFieldTraits>;
template exactreal::Module<RationalFieldTraits>::Module(
    const vector<shared_ptr<RealNumber>>&, long);
template struct exactreal::Module<NumberFieldTraits>;
template exactreal::Module<NumberFieldTraits>::Module(
    const vector<shared_ptr<RealNumber>>&, const NumberFieldTraits::Parameters&,
    long);
