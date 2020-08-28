/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
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

#include <set>

#include <e-antic/renfxx.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"
#include "../exact-real/real_number.hpp"

#include "external/unique-factory/unique_factory.hpp"

using namespace exactreal;
using boost::numeric_cast;
using boost::adaptors::transformed;
using std::is_same_v;
using std::set;
using std::shared_ptr;
using std::string;
using std::vector;

namespace exactreal {
template <typename Ring>
class Module<Ring>::Implementation {
 public:
  using Basis = typename Module<Ring>::Basis;

  Implementation() : basis({}), parameters(trivial()) {}

  explicit Implementation(const Basis& basis, const Ring& parameters) : basis(basis), parameters(parameters) {
    for (auto it = basis.begin(); it != basis.end(); it++) {
      for (auto jt = it + 1; jt != basis.end(); jt++) {
        if (static_cast<std::optional<mpq_class>>(**it) && static_cast<std::optional<mpq_class>>(**jt)) {
          throw std::logic_error("at most one generator can be rational");
        }
        if (**it == **jt) {
          throw std::logic_error("generators must be distinct");
        }
      }
    }
  }

  static Ring& trivial() {
    static Ring trivial = {};
    return trivial;
  }

  Basis basis;
  Ring parameters;

  using Factory = unique_factory::UniqueFactory<std::weak_ptr<Module<Ring>>, Basis, const Ring>;
  static Factory& factory() {
    static Factory* factory = new Factory();
    return *factory;
  }
};

template <typename Ring>
Module<Ring>::Module(spimpl::unique_impl_ptr<Module<Ring>::Implementation>&& impl) : impl(std::move(impl)) {}

template <typename Ring>
shared_ptr<const Module<Ring>> Module<Ring>::make(const Basis& basis) {
  return make(basis, Ring());
}

template <typename Ring>
shared_ptr<const Module<Ring>> Module<Ring>::make(const Basis& basis_, const Ring& ring) {
  Basis basis = basis_;
  std::sort(begin(basis), end(basis), [](const auto& lhs, const auto& rhs) { return lhs->deglex(*rhs); });
  return Module<Ring>::Implementation::factory().get(basis, ring, [&]() {
    return new Module<Ring>(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis, ring));
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
const Ring& Module<Ring>::ring() const {
  return impl->parameters;
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
    return m;
  }

  if (m->impl->parameters != n->impl->parameters) {
    auto parameters = Ring::compositum(m->impl->parameters, n->impl->parameters);
    return span(Module<Ring>::make(m->basis(), parameters), Module<Ring>::make(n->basis(), parameters));
  }

  if (n->submodule(*m)) {
    return m;
  }

  if (m->submodule(*n)) {
    return n;
  }

  auto basis = m->basis();
  for (auto& ngen : n->basis()) {
    if (find_if(basis.begin(), basis.end(), [&](const auto& gen) { return *gen == *ngen; }) == basis.end()) {
      basis.push_back(ngen);
    }
  }

  return Module<Ring>::make(basis, m->impl->parameters);
}

template <typename Ring>
bool Module<Ring>::submodule(const Module<Ring>& supermodule) const {
  auto super = supermodule.basis();
  for (auto& gen : basis()) {
    if (find_if(super.begin(), super.end(), [&](const auto& sgen) { return *gen == *sgen; }) == super.end()) {
      return false;
    }
  }
  return true;
}

template <typename Ring>
Element<Ring> Module<Ring>::gen(size j) const {
  std::vector<typename Ring::ElementClass> coefficients(this->rank());
  coefficients[numeric_cast<size_t>(j)] = 1;
  return Element(this->shared_from_this(), std::move(coefficients));
}

template <typename Ring>
Element<Ring> Module<Ring>::zero() const {
  return Element(this->shared_from_this(), std::vector<typename Ring::ElementClass>(this->rank()));
}

template <typename R>
std::ostream& operator<<(std::ostream& os, const Module<R>& self) {
  if constexpr (is_same_v<R, IntegerRing>) {
    os << "ℤ-Module(";
  } else if constexpr (is_same_v<R, RationalField>) {
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
#include "../exact-real/detail/smart_ostream.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"

template class exactreal::Module<IntegerRing>;
template std::ostream& exactreal::operator<<(std::ostream&, const Module<IntegerRing>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<IntegerRing>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<IntegerRing>>&);
template class exactreal::Module<RationalField>;
template std::ostream& exactreal::operator<<(std::ostream&, const Module<RationalField>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<RationalField>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<RationalField>>&);
template class exactreal::Module<NumberField>;
template std::ostream& exactreal::operator<<(std::ostream&, const Module<NumberField>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<const Module<NumberField>>&);
template std::ostream& exactreal::operator<<(std::ostream&, const shared_ptr<Module<NumberField>>&);
