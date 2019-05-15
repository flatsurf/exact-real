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
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <set>

#include "exact-real/element.hpp"
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/module.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"
#include "exact-real/real_number.hpp"

using namespace exactreal;
using boost::adaptors::transformed;
using std::is_same_v;
using std::set;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

namespace {
template <typename Ring, typename SameRing>
constexpr void assertSame() {
  static_assert(is_same_v<Ring, SameRing>,
                "Template arguments of class and method must be identical. You specified two different ring traits but "
                "this is not supported.");
}

template <typename Ring>
class ModuleImplementation {
 public:
  using Basis = vector<shared_ptr<RealNumber>>;

  ModuleImplementation() : basis({}) {}

  explicit ModuleImplementation(const Basis& basis) : basis(basis) {
    // See https://github.com/flatsurf/exact-real/issues/5 for missing input
    // checks.
  }

  Basis basis;
};

template <typename Ring>
class ModuleImplementationWithoutParameters : public ModuleImplementation<Ring> {
 public:
  using ModuleImplementation<Ring>::ModuleImplementation;
};

template <typename Ring>
class ModuleImplementationWithParameters : public ModuleImplementation<Ring> {
 public:
  ModuleImplementationWithParameters() : ModuleImplementation<Ring>(), parameters(&trivial) {}

  static typename Ring::Parameters trivial;

  explicit ModuleImplementationWithParameters(const typename ModuleImplementation<Ring>::Basis& basis,
                                              const typename Ring::Parameters& parameters)
      : ModuleImplementation<Ring>(basis), parameters(&parameters) {}

  typename Ring::Parameters const* parameters;
};

template <typename Ring>
typename Ring::Parameters ModuleImplementationWithParameters<Ring>::trivial = typename Ring::Parameters();

struct CompareGenerators {
  bool operator()(const shared_ptr<RealNumber>& lhs, const shared_ptr<RealNumber>& rhs) const {
    return *lhs < *rhs;
  }
};

}  // namespace

namespace exactreal {
template <typename Ring>
class Module<Ring>::Implementation
    : public std::conditional_t<is_parametrized_v<Ring>, ModuleImplementationWithParameters<Ring>,
                                ModuleImplementationWithoutParameters<Ring>> {
 public:
  using std::conditional_t<is_parametrized_v<Ring>, ModuleImplementationWithParameters<Ring>,
                           ModuleImplementationWithoutParameters<Ring>>::conditional_t;
};
}  // namespace exactreal

namespace exactreal {
template <typename Ring>
Module<Ring>::Module() : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>()) {}

template <typename Ring>
template <typename RingWithoutParameters>
Module<Ring>::Module(const Basis& basis)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis)) {
  assertSame<Ring, RingWithoutParameters>();
  static_assert(!is_parametrized_v<Ring>, "constructor is only valid for rings without parameters");
}

template <typename Ring>
template <typename RingWithParameters>
Module<Ring>::Module(const Basis& basis, const typename RingWithParameters::Parameters& parameters)
    : impl(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis, parameters)) {
  assertSame<Ring, RingWithParameters>();
  static_assert(is_parametrized_v<Ring>, "constructor is only valid for rings with parameters");
}

template <typename Ring>
shared_ptr<Module<Ring>> Module<Ring>::make() {
  return shared_ptr<Module<Ring>>(new Module<Ring>());
}

template <typename Ring>
template <typename RingWithoutParameters>
shared_ptr<Module<Ring>> Module<Ring>::make(const Basis& basis) {
  return shared_ptr<Module<Ring>>(new Module<Ring>(basis));
}

template <typename Ring>
template <typename RingWithParameters>
shared_ptr<Module<Ring>> Module<Ring>::make(const Basis& basis, const typename RingWithParameters::Parameters& parameters) {
  return shared_ptr<Module<Ring>>(new Module<Ring>(basis, parameters));
}

template <typename Ring>
size Module<Ring>::rank() const {
  return impl->basis.size();
}

template <typename Ring>
vector<shared_ptr<RealNumber>> const& Module<Ring>::basis() const {
  return impl->basis;
}

template <typename Ring>
Element<Ring> Module<Ring>::gen(size i) const {
  return Element<Ring>(this->shared_from_this(), i);
}

template <typename Ring>
template <typename RingWithParameters>
const typename RingWithParameters::Parameters& Module<Ring>::ring() const {
  assertSame<Ring, RingWithParameters>();
  static_assert(is_parametrized_v<Ring>, "ring() is only valid over rings with parameters");

  return impl->parameters;
}

template <typename Ring>
shared_ptr<const Module<Ring>> Module<Ring>::span(const shared_ptr<const Module<Ring>>& m, const shared_ptr<const Module<Ring>>& n) {
  // When one of the modules is trivial, we do not need to worry about the parameters but just return the other
  if (m->basis().size() == 0) {
    return n;
  }
  if (n->basis().size() == 0) {
    return span(n, m);
  }

  bool parameters_match = true;
  if constexpr (is_parametrized_v<Ring>) {
    if (m->impl->parameters != n->impl->parameters) {
      parameters_match = false;
    }
  }

  using Set = set<shared_ptr<RealNumber>, CompareGenerators>;
  Set mgens = Set(m->basis().begin(), m->basis().end());
  Set ngens = Set(n->basis().begin(), n->basis().end());
  Set gens = Set(mgens);
  for (auto gen : ngens) gens.insert(gen);

  // Note that we cannot use a std::set.operator== with a custom Compare() here
  // since equality of sets relies on the operator== of the elements as well
  // which is not what we want (the shared_ptr.operator== does not do the right
  // thing.)
  auto eq = [](const Set& lhs, const Set& rhs) { return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](const auto& x, const auto& y) { return *x == *y; }); };

  if (eq(gens, mgens)) {
    if (parameters_match) {
      return m;
    }
  }

  if (eq(gens, ngens)) {
    if (parameters_match) {
      return n;
    }
  }

  throw std::logic_error("Module::span() not implemented when a new module would need to be constructed");
}

template <typename Ring>
const shared_ptr<const Module<Ring>> Module<Ring>::trivial = Module<Ring>::make();

template <typename R>
std::ostream& operator<<(std::ostream& os, const Module<R>& self) {
  if constexpr (is_same_v<R, IntegerRingTraits>) {
    os << "ℤ-Module(";
  } else if constexpr (is_same_v<R, RationalFieldTraits>) {
    os << "ℚ-Module(";
  } else {
    os << "K" /* self.ring() */ << "-Module(";
  }
  os << boost::algorithm::join(self.basis() | transformed(
                                                 [](auto& gen) { return boost::lexical_cast<string>(*gen); }),
                               ", ");
  return os << ")";
}

}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"

template class exactreal::Module<IntegerRingTraits>;
template shared_ptr<exactreal::Module<IntegerRingTraits>> exactreal::Module<IntegerRingTraits>::make(const vector<shared_ptr<RealNumber>>&);
template exactreal::Module<IntegerRingTraits>::Module(const vector<shared_ptr<RealNumber>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<IntegerRingTraits>&);
template class exactreal::Module<RationalFieldTraits>;
template shared_ptr<exactreal::Module<RationalFieldTraits>> exactreal::Module<RationalFieldTraits>::make(const vector<shared_ptr<RealNumber>>&);
template exactreal::Module<RationalFieldTraits>::Module(const vector<shared_ptr<RealNumber>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<RationalFieldTraits>&);
template class exactreal::Module<NumberFieldTraits>;
template shared_ptr<exactreal::Module<NumberFieldTraits>> exactreal::Module<NumberFieldTraits>::make(const vector<shared_ptr<RealNumber>>&,
                                                      const NumberFieldTraits::Parameters&);
template exactreal::Module<NumberFieldTraits>::Module(const vector<shared_ptr<RealNumber>>&,
                                                      const NumberFieldTraits::Parameters&);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<NumberFieldTraits>&);
