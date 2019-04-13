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

#include <e-antic/renfxx.h>

#include "exact-real/element.hpp"
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/module.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"
#include "exact-real/real_number.hpp"

using namespace exactreal;
using std::is_same_v;
using std::shared_ptr;
using std::vector;

namespace {
template <typename Ring, typename SameRing>
constexpr void assertSame() {
  static_assert(is_same_v<Ring, SameRing>,
                "Template arguments of class and method must be identical. You specified two different ring traits but "
                "this is not supported.");
}

template <typename Ring>
struct ModuleImplementation {
  using Basis = vector<shared_ptr<RealNumber>>;

  ModuleImplementation() : basis({}), precision(0) {}

  explicit ModuleImplementation(const Basis& basis, long precision) : basis(basis), precision(precision) {
    // See https://github.com/flatsurf/exact-real/issues/5 for missing input
    // checks.
  }

  Basis basis;
  prec precision;
};

template <typename Ring>
struct ModuleImplementationWithoutParameters : ModuleImplementation<Ring> {
  using ModuleImplementation<Ring>::ModuleImplementation;
};

template <typename Ring>
struct ModuleImplementationWithParameters : ModuleImplementation<Ring> {
  ModuleImplementationWithParameters() : ModuleImplementation<Ring>(), parameters(&trivial) {}

  static typename Ring::Parameters trivial;

  explicit ModuleImplementationWithParameters(const typename ModuleImplementation<Ring>::Basis& basis,
                                              const typename Ring::Parameters& parameters, long precision)
      : ModuleImplementation<Ring>(basis, precision), parameters(&parameters) {}

  typename Ring::Parameters const* parameters;
};

template <typename Ring>
typename Ring::Parameters ModuleImplementationWithParameters<Ring>::trivial = typename Ring::Parameters();

}  // namespace

namespace exactreal {
template <typename Ring>
struct Module<Ring>::Implementation
    : std::conditional_t<is_parametrized_v<Ring>, ModuleImplementationWithParameters<Ring>,
                         ModuleImplementationWithoutParameters<Ring>> {
  using std::conditional_t<is_parametrized_v<Ring>, ModuleImplementationWithParameters<Ring>,
                           ModuleImplementationWithoutParameters<Ring>>::conditional_t;
};
}  // namespace exactreal

template <typename Ring>
Module<Ring>::Module() : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>()) {}

namespace exactreal {
template <typename Ring>
template <typename RingWithoutParameters>
Module<Ring>::Module(const Basis& basis, prec precision)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis, precision)) {
  assertSame<Ring, RingWithoutParameters>();
  static_assert(!is_parametrized_v<Ring>, "constructor is only valid for rings without parameters");
}

template <typename Ring>
template <typename RingWithParameters>
Module<Ring>::Module(const Basis& basis, const typename RingWithParameters::Parameters& parameters, prec precision)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis, parameters, precision)) {
  assertSame<Ring, RingWithParameters>();
  static_assert(is_parametrized_v<Ring>, "constructor is only valid for rings with parameters");
}

template <typename Ring>
size Module<Ring>::rank() const {
  return impl->basis.size();
}

template <typename Ring>
vector<shared_ptr<RealNumber>> const& Module<Ring>::gens() const {
  return impl->basis;
}

template <typename Ring>
template <typename RingWithParameters>
const typename RingWithParameters::Parameters& Module<Ring>::ring() const {
  assertSame<Ring, RingWithParameters>();
  static_assert(is_parametrized_v<Ring>, "ring() is only valid over rings with parameters");

  return impl->parameters;
}

template <typename Ring>
std::shared_ptr<const Module<Ring>> Module<Ring>::span(const std::shared_ptr<const Module<Ring>>& m, const std::shared_ptr<const Module<Ring>>& n) {
  const prec prec = std::max(m->impl->precision, n->impl->precision);
  if (m->gens() == n->gens()) {
    bool parameters_match = true;
    if constexpr (is_parametrized_v<Ring>) {
      if (m->impl->parameters != n->impl->parameters) {
        parameters_match = false;
      }
    }
    if (parameters_match) {
      if (m->impl->precision == prec) {
        return m;
      } else if (n->impl->precision == prec) {
        return n;
      }
    }
  }
  if (m->gens().size() == 0) {
    if (n->impl->precision == prec) {
      return n;
    }
  }
  if (n->gens().size() == 0) {
    return span(n, m);
  }
  throw std::logic_error("Module::span() not implemented for non-trivial cases");
}

template <typename Ring>
const std::shared_ptr<const Module<Ring>> Module<Ring>::trivial = std::make_shared<const Module<Ring>>();

template <typename R>
std::ostream& operator<<(std::ostream& os, const Module<R>& self) {
  if constexpr (is_same_v<R, IntegerRingTraits>) {
    os << "ℤ-Module(";
  } else if constexpr (is_same_v<R, RationalFieldTraits>) {
    os << "ℚ-Module(";
  } else {
    os << "K" /* self.ring() */ << "-Module(";
  }
  for (const auto& b : self.gens()) {
    os << *b << ", ";
  }
  return os << ")";
}

}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"

template struct exactreal::Module<IntegerRingTraits>;
template exactreal::Module<IntegerRingTraits>::Module(const vector<shared_ptr<RealNumber>>&, prec);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<IntegerRingTraits>&);
template struct exactreal::Module<RationalFieldTraits>;
template exactreal::Module<RationalFieldTraits>::Module(const vector<shared_ptr<RealNumber>>&, long);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<RationalFieldTraits>&);
template struct exactreal::Module<NumberFieldTraits>;
template exactreal::Module<NumberFieldTraits>::Module(const vector<shared_ptr<RealNumber>>&,
                                                      const NumberFieldTraits::Parameters&, prec);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<NumberFieldTraits>&);
