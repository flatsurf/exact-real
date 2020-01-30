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

#include <cmath>
#include <map>
#include <set>

#include <e-antic/renfxx.h>
#include <boost/numeric/conversion/cast.hpp>

#include "../exact-real/element.hpp"
#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/yap/arb.hpp"

#include "external/gmpxxll/gmpxxll/mpz_class.hpp"

using namespace exactreal;
using boost::numeric_cast;
using std::find_if;
using std::logic_error;
using std::map;
using std::ostream;
using std::set;
using std::shared_ptr;
using std::vector;

namespace {
template <typename Ring>
class ElementImplementation {
 public:
  ElementImplementation() : ElementImplementation(Module<Ring>::make({})) {}

  explicit ElementImplementation(const shared_ptr<const Module<Ring>>& parent)
      : ElementImplementation(parent, vector<typename Ring::ElementClass>(parent->rank())) {}

  ElementImplementation(const shared_ptr<const Module<Ring>>& parent, const vector<typename Ring::ElementClass>& coefficients)
      : parent(parent), coefficients(coefficients) {
    assert(static_cast<size>(coefficients.size()) == parent->rank());
  }

  shared_ptr<const Module<Ring>> parent;
  vector<typename Ring::ElementClass> coefficients;
};

template <typename Ring, typename RHS>
bool lt(const Element<Ring>& lhs, const RHS& rhs) {
  if (lhs == rhs)
    return false;
  for (long prec = 64;; prec *= 2) {
    Arb self = lhs.arb(prec);
    std::optional<bool> lt;
    if constexpr (std::is_same_v<RHS, Element<Ring>> || std::is_same_v<RHS, RealNumber>) {
      Arb other = rhs.arb(prec);
      lt = self < other;
    } else {
      lt = self < rhs;
    }
    if (lt.has_value()) {
      return *lt;
    }
  }
}

template <typename Ring, typename RHS>
bool gt(const Element<Ring>& lhs, const RHS& rhs) {
  if (lhs == rhs)
    return false;
  return !(lhs < rhs);
}

}  // namespace

namespace exactreal {
template <typename Ring>
class Element<Ring>::Implementation : public ElementImplementation<Ring> {
 public:
  using ElementImplementation<Ring>::ElementImplementation;
};

template <typename Ring>
Element<Ring>::Element() : impl(spimpl::make_impl<Element<Ring>::Implementation>()) {}

template <typename Ring>
Element<Ring>::Element(const shared_ptr<const Module<Ring>>& parent, const vector<typename Ring::ElementClass>& coefficients)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent, coefficients)) {}

template <typename Ring>
template <bool Enabled, std::enable_if_t<Enabled, bool>>
Element<Ring>::Element(const Element<IntegerRing>& value)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(
          Module<Ring>::make(value.module()->basis()),
          [&](const auto& coefficients) {
            return std::vector<typename Ring::ElementClass>(coefficients.begin(), coefficients.end());
          }(value.coefficients()))) {}

template <typename Ring>
template <bool Enabled, std::enable_if_t<Enabled, bool>>
Element<Ring>::Element(const Element<RationalField>& value)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(
          Module<Ring>::make(value.module()->basis()),
          [&](const auto& coefficients) {
            return std::vector<typename Ring::ElementClass>(coefficients.begin(), coefficients.end());
          }(value.coefficients()))) {}

template <typename Ring>
Element<Ring>::Element(const typename Ring::ElementClass& value) : Element(Module<Ring>::make({RealNumber::rational(1)}, Ring(value)), {value}) {
}

template <typename Ring>
typename Ring::ElementClass Element<Ring>::operator[](const size i) const {
  return impl->coefficients.at(i);
}

template <typename Ring>
template <typename C>
std::vector<C> Element<Ring>::coefficients() const {
  if constexpr (std::is_same_v<C, typename Ring::ElementClass>) {
    return impl->coefficients;
  } else if constexpr (std::is_same_v<C, mpq_class> && std::is_same_v<typename Ring::ElementClass, mpz_class>) {
    std::vector<mpq_class> coefficients;
    for (const auto& c : impl->coefficients) coefficients.push_back(c);
    return coefficients;
  } else if constexpr (std::is_same_v<C, mpq_class> && std::is_same_v<typename Ring::ElementClass, eantic::renf_elem_class>) {
    std::vector<mpq_class> ret;
    for (auto& c : impl->coefficients) {
      mpz_class den = c.den();
      auto nums = c.num_vector();
      for (auto& num : nums) {
        ret.push_back(mpq_class(num, den));
      }
      if (impl->parent->ring().parameters == nullptr) {
        assert(ret.size() <= 1 && "module over the rationals cannot have a basis of length more than one");
        if (ret.size() < 1)
          ret.push_back(0);
      } else {
        for (size_t i = nums.size(); i < impl->parent->ring().parameters->degree(); i++) {
          ret.push_back(0);
        }
      }
    }
    return ret;
  } else {
    static_assert(false_v<C>, "unsupported coefficient type");
  }
}

template <typename Ring>
Arb Element<Ring>::arb(long prec) const {
  if (!*this) {
    return Arb();
  }
  prec += numeric_cast<long>(ceil(log2(numeric_cast<double>(impl->parent->rank()))));
  Arb ret;
  for (int i = 0; i < impl->parent->rank(); i++) {
    ret += (impl->parent->basis()[i]->arb(prec) * Ring::arb(impl->coefficients[i], prec))(prec);
  }
  return ret;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator+=(const Element<Ring>& rhs) {
  if (impl->parent != rhs.impl->parent) {
    auto parent = Module<Ring>::span(this->impl->parent, rhs.impl->parent);
    return promote(parent) += Element<Ring>(rhs).promote(parent);
  } else {
    for (int i = 0; i < impl->parent->rank(); i++) {
      impl->coefficients[i] += rhs.impl->coefficients[i];
    }
    return *this;
  }
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator-=(const Element<Ring>& rhs) {
  return *this += -rhs;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator*=(const Element<Ring>& rhs) {
  if (!*this)
    return *this;
  if (!rhs)
    return *this = rhs;

  if (this->module()->ring() != rhs.module()->ring()) {
    auto compositum = Ring::compositum(this->module()->ring(), rhs.module()->ring());
    return this->promote(Module<Ring>::make(this->module()->basis(), compositum)) *=
           Element<Ring>(rhs).promote(Module<Ring>::make(rhs.module()->basis(), compositum));
  }

  map<shared_ptr<const RealNumber>, typename Ring::ElementClass> products;

  for (size_t i = 0; i < impl->parent->basis().size(); i++) {
    for (size_t j = 0; j < rhs.impl->parent->basis().size(); j++) {
      auto gen = *impl->parent->basis()[i] * *rhs.impl->parent->basis()[j];
      auto value = impl->coefficients[i] * rhs.impl->coefficients[j];

      products[gen] += value;
    }
  }

  vector<std::pair<shared_ptr<const RealNumber>, typename Ring::ElementClass>> sorted(products.begin(), products.end());
  std::sort(sorted.begin(), sorted.end(), [](const auto& lhs, const auto& rhs) {
    return *lhs.first < *rhs.first;
  });

  vector<shared_ptr<const RealNumber>> basis;
  vector<typename Ring::ElementClass> coefficients;
  for (auto& v : sorted) {
    basis.push_back(v.first);
    coefficients.push_back(v.second);
  }

  this->impl->parent = Module<Ring>::make(basis, this->impl->parent->ring());
  this->impl->coefficients = coefficients;

  return *this;
}

template <typename Ring>
Element<Ring> Element<Ring>::operator-() const {
  vector<typename Ring::ElementClass> coefficients = impl->coefficients;
  for (auto& c : coefficients) {
    // cppcheck-suppress useStlAlgorithm
    c = -c;
  }
  return Element<Ring>(impl->parent, coefficients);
}

template <typename Ring>
template <typename T, typename>
Element<Ring>& Element<Ring>::operator*=(const T& rhs) {
  for (auto& c : impl->coefficients) {
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
template <typename T, typename, typename>
Element<Ring>& Element<Ring>::operator/=(const T& rhs) {
  for (auto& c : impl->coefficients) {
    c /= rhs;
  }
  return *this;
}

template <typename Ring>
std::optional<typename Ring::ElementClass> Element<Ring>::operator/(const Element<Ring>& rhs) const {
  if (impl->parent != rhs.impl->parent) {
    auto parent = Module<Ring>::span(this->impl->parent, rhs.impl->parent);
    return Element<Ring>(*this).promote(parent) / Element<Ring>(rhs).promote(parent);
  }

  std::optional<typename Ring::ElementClass> ret;

  for (size i = 0; i < impl->parent->rank(); i++) {
    const auto& other = rhs.impl->coefficients[i];
    const auto& self = impl->coefficients[i];
    if (other == 0) {
      if (self != 0) {
        return {};
      }
    } else {
      auto quot = self / other;
      if (ret.has_value()) {
        if (quot != *ret) {
          return {};
        }
      } else {
        ret = quot;
      }
    }
  }

  return ret;
}

template <typename Ring>
bool Element<Ring>::operator<(const Element<Ring>& rhs) const {
  return lt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator<(const mpq_class& rhs) const {
  return lt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator>(const mpq_class& rhs) const {
  return gt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator<(const mpz_class& rhs) const {
  return lt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator>(const mpz_class& rhs) const {
  return gt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator<(long long rhs) const {
  return lt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator>(long long rhs) const {
  return gt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator<(const RealNumber& rhs) const {
  return lt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator>(const RealNumber& rhs) const {
  return gt(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator==(const mpz_class& rhs) const {
  return *this == mpq_class(rhs);
}

template <typename Ring>
bool Element<Ring>::operator==(long long rhs) const {
  return *this == mpq_class(boost::lexical_cast<std::string>(rhs));
}

template <typename Ring>
bool Element<Ring>::operator==(const mpq_class& rhs) const {
  for (size_t i = 0; i < module()->rank(); i++) {
    auto rational = static_cast<std::optional<mpq_class>>(*module()->basis()[i]);
    if (rational) {
      if (*rational * impl->coefficients[i] != rhs)
        return false;
    } else {
      if (impl->coefficients[i])
        return false;
    }
  }

  return true;
}

template <typename Ring>
bool Element<Ring>::operator==(const Element<Ring>& rhs) const {
  if (impl->parent != rhs.impl->parent) {
    auto parent = Module<Ring>::span(impl->parent, rhs.impl->parent);
    return Element<Ring>(*this).promote(parent) == Element<Ring>(rhs).promote(parent);
  }

  return impl->coefficients == rhs.impl->coefficients;
}

template <typename Ring>
bool Element<Ring>::operator==(const RealNumber& rhs) const {
  auto& gens = impl->parent->basis();
  auto it = find_if(gens.begin(), gens.end(), [&](const auto& other) { return *other == rhs; });
  if (it == gens.end()) {
    throw logic_error("not implemented - equality of Element with unrelated RealNumber");
  }
  const size at = it - gens.begin();
  for (size_t i = 0; i < impl->parent->rank(); i++) {
    if (impl->coefficients[i] == 0) {
      if (i == at) {
        return false;
      }
    } else if (impl->coefficients[i] == 1) {
      if (i != at) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

template <typename Ring>
Element<Ring>::operator bool() const {
  return std::any_of(impl->coefficients.begin(), impl->coefficients.end(), [](auto c) { return c != 0; });
}

template <typename Ring>
Element<Ring>::operator double() const {
  return static_cast<double>(arb(54));
}

template <typename Ring>
const shared_ptr<const Module<Ring>> Element<Ring>::module() const {
  return impl->parent;
}

template <typename Ring>
Element<Ring>& Element<Ring>::promote(const shared_ptr<const Module<Ring>>& parent) {
  if (this->impl->parent == parent) {
    return *this;
  }
  if (!*this) {
    return *this = parent->zero();
  }
  auto& our_gens = impl->parent->basis();
  assert(std::all_of(our_gens.begin(), our_gens.end(), [&](const auto& gen) { return std::find_if(parent->basis().begin(), parent->basis().end(), [&](const auto& ogen) { return *gen == *ogen; }) != parent->basis().end(); }) &&
         "can not promote to new parent since our parent is not a submodule");

  vector<typename Ring::ElementClass> new_coefficients;
  std::transform(parent->basis().begin(), parent->basis().end(), std::back_inserter(new_coefficients),
                 [&](const auto& gen) {
                   auto our_gen = std::find_if(our_gens.begin(), our_gens.end(), [&](auto& g) { return *g == *gen; });
                   if (our_gen == our_gens.end()) {
                     return typename Ring::ElementClass();
                   } else {
                     return impl->coefficients[our_gen - our_gens.begin()];
                   }
                 });
  return *this = Element<Ring>(parent, new_coefficients);
}

template <typename Ring>
ostream& operator<<(ostream& out, const Element<Ring>& self) {
  // Print summands sorted by absolute generator value to get stable outputs.
  // (additionally, print positive coefficients first so that we do not get
  // leading minus signs.)
  std::set<std::tuple<bool, Element<Ring>, typename Ring::ElementClass>> coefficients;
  for (size i = 0; i < self.impl->parent->rank(); i++) {
    coefficients.insert(std::tuple(
        self.impl->coefficients[i] < 0,
        Element<Ring>(Module<Ring>::make({self.impl->parent->basis()[i]}, self.impl->parent->ring()), std::vector<typename Ring::ElementClass>({1})),
        self.impl->coefficients[i]));
  }

  bool empty = true;
  for (auto& gc : coefficients) {
    auto c = std::get<2>(gc);
    if (c != 0) {
      if (c > 0) {
        if (empty) {
          ;
        } else {
          out << " + ";
        }
      } else {
        if (empty) {
          out << "-";
        } else {
          out << " - ";
        }
        c = -c;
      }
      empty = false;
      auto g = std::get<1>(gc).module()->basis()[0];
      if (c != 1) {
        out << c;
        if (*g != 1) {
          out << "*" << *g;
        }
      } else {
        out << *g;
      }
    }
  }
  if (empty) {
    out << "0";
  }
  return out;
}
}  // namespace exactreal

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"

namespace exactreal {
template class Element<IntegerRing>;
template ostream& operator<<<IntegerRing>(ostream&, const Element<IntegerRing>&);
template Element<IntegerRing>& Element<IntegerRing>::operator*=(const int&);
template Element<IntegerRing>& Element<IntegerRing>::operator*=(const mpz_class&);
template std::vector<typename IntegerRing::ElementClass> Element<IntegerRing>::coefficients() const;
template std::vector<mpq_class> Element<IntegerRing>::coefficients() const;

template class Element<RationalField>;
template Element<RationalField>::Element(const Element<IntegerRing>&);
template ostream& operator<<<RationalField>(ostream&, const Element<RationalField>&);
template Element<RationalField>& Element<RationalField>::operator*=(const int&);
template Element<RationalField>& Element<RationalField>::operator*=(const mpz_class&);
template Element<RationalField>& Element<RationalField>::operator*=(const mpq_class&);
template Element<RationalField>& Element<RationalField>::operator/=(const int&);
template Element<RationalField>& Element<RationalField>::operator/=(const mpz_class&);
template Element<RationalField>& Element<RationalField>::operator/=(const mpq_class&);
template std::vector<typename RationalField::ElementClass> Element<RationalField>::coefficients() const;

template class Element<NumberField>;
template ostream& operator<<<NumberField>(ostream&, const Element<NumberField>&);
template Element<NumberField>& Element<NumberField>::operator*=(const int&);
template Element<NumberField>& Element<NumberField>::operator*=(const mpz_class&);
template Element<NumberField>& Element<NumberField>::operator*=(const mpq_class&);
template Element<NumberField>& Element<NumberField>::operator*=(const eantic::renf_elem_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const int&);
template Element<NumberField>& Element<NumberField>::operator/=(const mpz_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const mpq_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const eantic::renf_elem_class&);
template std::vector<mpq_class> Element<NumberField>::coefficients<mpq_class>() const;
template std::vector<eantic::renf_elem_class> Element<NumberField>::coefficients<eantic::renf_elem_class>() const;
}  // namespace exactreal
