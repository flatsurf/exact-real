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

#include "exact-real/detail/unique_factory.hpp"
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
  using Basis = typename exactreal::Module<Ring>::Basis;

  ModuleImplementation() : basis({}) {}

  explicit ModuleImplementation(const Basis& basis) : basis(basis) {
    for (auto it = basis.begin(); it != basis.end(); it++) {
      for (auto jt = it + 1; jt != basis.end(); jt++) {
        assert((!static_cast<std::optional<mpq_class>>(**it) || !static_cast<std::optional<mpq_class>>(**jt)) && "at most one generator can be rational");
        assert(**it != **jt && "generators must be distinct");
      }
    }
  }

  Basis basis;
};

template <typename Ring>
class ModuleImplementationWithoutParameters : public ModuleImplementation<Ring> {
  using typename ModuleImplementation<Ring>::Basis;

 public:
  using ModuleImplementation<Ring>::ModuleImplementation;

  using Factory = UniqueFactory<Module<Ring>, Basis>;
  static Factory factory;
};

template <typename Ring>
typename ModuleImplementationWithoutParameters<Ring>::Factory ModuleImplementationWithoutParameters<Ring>::factory = typename ModuleImplementationWithoutParameters<Ring>::Factory();

template <typename Ring>
class ModuleImplementationWithParameters : public ModuleImplementation<Ring> {
  using typename ModuleImplementation<Ring>::Basis;

 public:
  ModuleImplementationWithParameters() : ModuleImplementation<Ring>(), parameters(&trivial) {}

  static typename Ring::Parameters trivial;

  explicit ModuleImplementationWithParameters(const Basis& basis,
                                              const typename Ring::Parameters& parameters)
      : ModuleImplementation<Ring>(basis), parameters(&parameters) {}

  typename Ring::Parameters const* parameters;

  using Factory = UniqueFactory<Module<Ring>, Basis, const typename Ring::Parameters>;
  static Factory factory;
};

template <typename Ring>
typename Ring::Parameters ModuleImplementationWithParameters<Ring>::trivial = typename Ring::Parameters();

template <typename Ring>
typename ModuleImplementationWithParameters<Ring>::Factory ModuleImplementationWithParameters<Ring>::factory = typename ModuleImplementationWithParameters<Ring>::Factory();

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

template <typename Ring>
Module<Ring>::Module(spimpl::unique_impl_ptr<Module<Ring>::Implementation>&& impl) : impl(std::move(impl)) {}

template <typename Ring>
template <typename RingWithoutParameters>
shared_ptr<Module<Ring>> Module<Ring>::make(const Basis& basis) {
  assertSame<Ring, RingWithoutParameters>();
  static_assert(!is_parametrized_v<Ring>, "constructor is only valid for rings without parameters");
  return Module<Ring>::Implementation::factory.get(basis, [](const Basis& gens) {
    return new Module<Ring>(spimpl::make_unique_impl<Module<Ring>::Implementation>(gens));
  });
}

template <typename Ring>
template <typename RingWithParameters>
shared_ptr<Module<Ring>> Module<Ring>::make(const Basis& basis, const typename RingWithParameters::Parameters& parameters) {
  assertSame<Ring, RingWithParameters>();
  static_assert(is_parametrized_v<Ring>, "constructor is only valid for rings with parameters");
  return Module<Ring>::Implementation::factory.get(basis, parameters, [](const Basis& gens, const typename RingWithParameters::Parameters& params) {
    return new Module<Ring>(spimpl::make_unique_impl<Module<Ring>::Implementation>(gens, params));
  });
}

template <typename Ring>
size Module<Ring>::rank() const {
  return impl->basis.size();
}

template <typename Ring>
vector<shared_ptr<const RealNumber>> const& Module<Ring>::basis() const {
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

  return *impl->parameters;
}

template <typename Ring>
bool Module<Ring>::operator==(const Module<Ring>& rhs) const {
  // Since this is a unique parent, there can be no duplicates and it sufficies to compare pointers.
  return this == &rhs;
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

  if (!parameters_match) {
    throw std::logic_error("Module::span() not implemented when a new module would need to be constructed");
  }

  // This is a very naive O(n²) implementation. Doing something smart here used
  // to be very difficult due to some unique_ptr mess.
  for (auto& mgen : m->basis()) {
    if (find_if(n->basis().begin(), n->basis().end(), [&](const auto& gen) { return *gen == *mgen; }) == n->basis().end()) {
      for (auto& ngen : n->basis()) {
        if (find_if(m->basis().begin(), m->basis().end(), [&](const auto& gen) { return *gen == *ngen; }) == m->basis().end()) {
          throw std::logic_error("Module::span() not implemented when a new module would need to be constructed");
        }
      }
      return m;
    }
  }
  return n;
}

template <typename Ring>
const shared_ptr<const Module<Ring>> Module<Ring>::trivial = std::shared_ptr<Module<Ring>>(new Module<Ring>(spimpl::make_unique_impl<Module<Ring>::Implementation>()));

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
#include "exact-real/detail/smart_ostream.hpp"
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"

template class exactreal::Module<IntegerRingTraits>;
template shared_ptr<exactreal::Module<IntegerRingTraits>> exactreal::Module<IntegerRingTraits>::make(const vector<shared_ptr<const RealNumber>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<IntegerRingTraits>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<IntegerRingTraits>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<IntegerRingTraits>>&);
template class exactreal::Module<RationalFieldTraits>;
template shared_ptr<exactreal::Module<RationalFieldTraits>> exactreal::Module<RationalFieldTraits>::make(const vector<shared_ptr<const RealNumber>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const Module<RationalFieldTraits>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<RationalFieldTraits>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<RationalFieldTraits>>&);
template class exactreal::Module<NumberFieldTraits>;
template shared_ptr<exactreal::Module<NumberFieldTraits>> exactreal::Module<NumberFieldTraits>::make(const vector<shared_ptr<const RealNumber>>&,
                                                                                                     const NumberFieldTraits::Parameters&);
template const NumberFieldTraits::Parameters& exactreal::Module<NumberFieldTraits>::ring<NumberFieldTraits>() const;
template std::ostream& exactreal::operator<<(std::ostream&, const Module<NumberFieldTraits>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<NumberFieldTraits>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<NumberFieldTraits>>&);
