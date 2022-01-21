/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#include "../exact-real/module.hpp"

#include <e-antic/renfxx.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <set>

#include "../exact-real/element.hpp"
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"
#include "../exact-real/real_number.hpp"
#include "impl/real_number_base.hpp"
#include "external/hash-combine/hash.hpp"
#include "external/unique-factory/unique-factory/unique-factory.hpp"

namespace exactreal {
template <typename Ring>
class Module<Ring>::Implementation {
 public:
  using Basis = typename Module<Ring>::Basis;

  Implementation() : basis({}), parameters(trivial()) {}

  explicit Implementation(const Basis& basis, const Ring& parameters) : basis(basis), parameters(parameters) {
    for (auto it = begin(basis); it != end(basis); it++) {
      for (auto jt = it + 1; jt != end(basis); jt++) {
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

  class Key {
   public:
    Key(const Basis& basis, Ring ring) : ring(std::move(ring)) {
      this->basis.reserve(basis.size());
      for (const auto& real : basis)
        this->basis.push_back(RealNumberBase::id(*real));
    }

    bool operator==(const Key& rhs) const {
      return this->ring == rhs.ring && this->basis == rhs.basis;
    }

    struct Hash {
      size_t operator()(const Key& key) const {
        using flatsurf::hash, flatsurf::hash_combine;

        const auto& [basis, ring] = key;

        size_t ret = hash(ring);
        for (const auto& b : basis)
          ret = hash_combine(ret, b);

        return ret;
      }
    };

   private:
    std::vector<size_t> basis;
    Ring ring;
  };

  static auto& factory() {
    static unique_factory::UniqueFactory<Key, Module<Ring>, unique_factory::KeepSetAlive<Module<Ring>, 1024>, typename Key::Hash> factory;
    return factory;
  }
};

template <typename Ring>
Module<Ring>::Module(spimpl::unique_impl_ptr<Module<Ring>::Implementation>&& impl) : impl(std::move(impl)) {}

template <typename Ring>
std::shared_ptr<const Module<Ring>> Module<Ring>::make(const Basis& basis) {
  return make(basis, Ring());
}

template <typename Ring>
std::shared_ptr<const Module<Ring>> Module<Ring>::make(const Basis& basis_, const Ring& ring) {
  Basis basis = basis_;
  std::sort(begin(basis), end(basis), [](const auto& lhs, const auto& rhs) { return lhs->deglex(*rhs); });
  return Module<Ring>::Implementation::factory().get(typename Module<Ring>::Implementation::Key{basis, ring}, [&]() {
    return new Module<Ring>(spimpl::make_unique_impl<Module<Ring>::Implementation>(basis, ring));
  });
}

template <typename Ring>
size Module<Ring>::rank() const {
  return impl->basis.size();
}

template <typename Ring>
std::vector<std::shared_ptr<const RealNumber>> const& Module<Ring>::basis() const {
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
std::shared_ptr<const Module<Ring>> Module<Ring>::span(const std::shared_ptr<const Module<Ring>>& m, const std::shared_ptr<const Module<Ring>>& n) {
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
    if (find_if(begin(basis), end(basis), [&](const auto& gen) { return *gen == *ngen; }) == end(basis)) {
      basis.push_back(ngen);
    }
  }

  return Module<Ring>::make(basis, m->impl->parameters);
}

template <typename Ring>
bool Module<Ring>::submodule(const Module<Ring>& supermodule) const {
  auto super = supermodule.basis();
  for (auto& gen : basis()) {
    if (find_if(begin(super), end(super), [&](const auto& sgen) { return *gen == *sgen; }) == end(super)) {
      return false;
    }
  }
  return true;
}

template <typename Ring>
Element<Ring> Module<Ring>::gen(size j) const {
  std::vector<typename Ring::ElementClass> coefficients(this->rank());
  coefficients[boost::numeric_cast<size_t>(j)] = 1;
  return Element(this->shared_from_this(), std::move(coefficients));
}

template <typename Ring>
Element<Ring> Module<Ring>::zero() const {
  return Element(this->shared_from_this(), std::vector<typename Ring::ElementClass>(this->rank()));
}

template <typename Ring>
Element<Ring> Module<Ring>::one() const {
  for (int g = 0; g < rank(); g++) {
    auto rational = static_cast<std::optional<mpq_class>>(*basis()[g]);
    if (rational) {
      if constexpr (std::is_same_v<Ring, IntegerRing>) {
        if (rational->get_num() == 1) {
          return rational->get_den() * gen(g);
        }
      } else {
          return mpq_class(rational->get_den(), rational->get_num()) * gen(g);
      }
    }
  }
  throw std::logic_error("Module contains no 1 element.");
}

template <typename R>
std::ostream& operator<<(std::ostream& os, const Module<R>& self) {
  if constexpr (std::is_same_v<R, IntegerRing>) {
    os << "ℤ-Module(";
  } else if constexpr (std::is_same_v<R, RationalField>) {
    os << "ℚ-Module(";
  } else {
    os << "K" /* self.ring() */ << "-Module(";
  }
  os << boost::algorithm::join(self.basis() | boost::adaptors::transformed(
                                                  [](auto& gen) { return boost::lexical_cast<std::string>(*gen); }),
                               ", ");
  return os << ")";
}

}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"

namespace exactreal {
template class Module<IntegerRing>;
template std::ostream& operator<<(std::ostream&, const Module<IntegerRing>&);
template class Module<RationalField>;
template std::ostream& operator<<(std::ostream&, const Module<RationalField>&);
template class Module<NumberField>;
template std::ostream& operator<<(std::ostream&, const Module<NumberField>&);
}  // namespace exactreal
