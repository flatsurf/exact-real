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
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <map>
#include <set>

#include "exact-real/element.hpp"
#include "exact-real/module.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arb.hpp"

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
// Forward a smart pointers < to the underlying type
template <class P>
struct smart_less {
  constexpr bool operator()(const P& lhs, const P& rhs) const {
    if (lhs == nullptr) {
      return rhs == nullptr;
    }
    if (rhs == nullptr) {
      return false;
    }
    return *lhs < *rhs;
  }
};

template <typename Ring>
class ElementImplementation {
 public:
  ElementImplementation() : ElementImplementation(Module<Ring>::trivial) {}

  explicit ElementImplementation(const shared_ptr<const Module<Ring>>& parent)
      : ElementImplementation(parent, vector<typename Ring::ElementClass>(parent->rank())) {}

  ElementImplementation(const shared_ptr<const Module<Ring>>& parent, const vector<typename Ring::ElementClass>& coefficients)
      : parent(parent), coefficients(coefficients) {
    assert(size(coefficients.size()) == parent->rank());
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
Element<Ring>::Element(const shared_ptr<const Module<Ring>>& parent) : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {}

template <typename Ring>
Element<Ring>::Element(const shared_ptr<const Module<Ring>>& parent, const vector<typename Ring::ElementClass>& coefficients)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent, coefficients)) {}

template <typename Ring>
Element<Ring>::Element(const shared_ptr<const Module<Ring>>& parent, const size gen)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {
  impl->coefficients[numeric_cast<size_t>(gen)] = 1;
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
  } else if constexpr (std::is_same_v<C, mpq_class> && std::is_same_v<typename Ring::ElementClass, eantic::renf_elem_class>) {
    std::vector<mpq_class> ret;
    for (auto& c : impl->coefficients) {
      mpz_class den = c.get_den();
      auto nums = c.get_num_vector();
      for (auto& num : nums) {
        ret.push_back(mpq_class(num, den));
      }
      for (size_t i = nums.size(); i < impl->parent->ring()->degree(); i++) {
        ret.push_back(0);
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
  if constexpr (is_parametrized_v<Ring>) {
    if (this->module()->ring() != rhs.module()->ring()) {
      throw std::logic_error("not implemented - multiplication in modules over different rings");
    }
  }

  // Order coefficients by size so we get a reproducible order of the
  // generators (helps with reproducible testing.)
  map<shared_ptr<const RealNumber>, typename Ring::ElementClass, smart_less<shared_ptr<const RealNumber>>> products;

  for (size_t i = 0; i < impl->parent->basis().size(); i++) {
    for (size_t j = 0; j < rhs.impl->parent->basis().size(); j++) {
      auto gen = *impl->parent->basis()[i] * *rhs.impl->parent->basis()[j];
      auto value = impl->coefficients[i] * rhs.impl->coefficients[j];

      products[gen] += value;
    }
  }

  vector<typename Ring::ElementClass> coefficients;
  vector<shared_ptr<const RealNumber>> basis;
  for (auto& v : products) {
    basis.push_back(v.first);
    coefficients.push_back(v.second);
  }

  if constexpr (is_parametrized_v<Ring>) {
    this->impl->parent = Module<Ring>::make(basis, this->impl->parent->ring());
  } else {
    this->impl->parent = Module<Ring>::make(basis);
  }
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
Element<Ring>& Element<Ring>::operator*=(const int& rhs) {
  for (auto& c : impl->coefficients) {
    // cppcheck-suppress useStlAlgorithm
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator*=(const typename Ring::ElementClass& rhs) {
  for (auto& c : impl->coefficients) {
    // cppcheck-suppress useStlAlgorithm
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
template <typename mpz>
Element<Ring>& Element<Ring>::operator*=(
    const typename std::enable_if_t<
        std::is_same_v<mpz, mpz_class> && !std::is_same_v<typename Ring::ElementClass, mpz_class>, mpz>& rhs) {
  for (auto& c : impl->coefficients) {
    // cppcheck-suppress useStlAlgorithm
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
std::optional<typename Ring::ElementClass> Element<Ring>::operator/(const Element<Ring>& rhs) const {
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
    return *this = Element(parent);
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
  bool empty = true;
  for (size i = 0; i < self.impl->parent->rank(); i++) {
    if (self.impl->coefficients[i] != 0) {
      if (!empty) {
        out << " + ";
      }
      empty = false;
      if (self.impl->coefficients[i] != 1) {
        out << self.impl->coefficients[i] << "*";
      }
      out << *self.impl->parent->basis()[i];
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
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"

template class exactreal::Element<IntegerRingTraits>;
template ostream& exactreal::operator<<<IntegerRingTraits>(ostream&, const Element<IntegerRingTraits>&);
template std::vector<typename IntegerRingTraits::ElementClass> exactreal::Element<IntegerRingTraits>::coefficients() const;
template class exactreal::Element<RationalFieldTraits>;
template ostream& exactreal::operator<<<RationalFieldTraits>(ostream&, const Element<RationalFieldTraits>&);
template Element<RationalFieldTraits>& exactreal::Element<RationalFieldTraits>::operator*=
    <mpz_class>(const mpz_class& rhs);
template std::vector<typename RationalFieldTraits::ElementClass> exactreal::Element<RationalFieldTraits>::coefficients() const;
template class exactreal::Element<NumberFieldTraits>;
template ostream& exactreal::operator<<<NumberFieldTraits>(ostream&, const Element<NumberFieldTraits>&);
template Element<NumberFieldTraits>& exactreal::Element<NumberFieldTraits>::operator*=<mpz_class>(const mpz_class& rhs);
template std::vector<typename NumberFieldTraits::ElementClass> exactreal::Element<NumberFieldTraits>::coefficients() const;
template std::vector<mpq_class> exactreal::Element<NumberFieldTraits>::coefficients<mpq_class>() const;
