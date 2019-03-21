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

#include <boost/numeric/conversion/cast.hpp>
#include <cmath>

#include "exact-real/arb.hpp"
#include "exact-real/element.hpp"
#include "exact-real/module.hpp"

using namespace exactreal;
using boost::numeric_cast;
using std::find_if;
using std::logic_error;
using std::ostream;
using std::vector;

namespace {
template <typename Ring>
struct ElementImplementation {
  explicit ElementImplementation(const Module<Ring>& parent)
      : parent(parent),
        coefficients(vector<typename Ring::ElementClass>(parent.rank())) {}

  ElementImplementation(const Module<Ring>& parent,
                        const vector<typename Ring::ElementClass>& coefficients)
      : parent(parent), coefficients(coefficients) {
    assert(coefficients.size() == parent.rank());
  }

  const Module<Ring>& parent;
  vector<typename Ring::ElementClass> coefficients;
};
}  // namespace

template <typename LHS, typename RHS>
bool lt_assuming_ne(const LHS& lhs, const RHS& rhs) {
  for (long prec = 64;; prec *= 2) {
    Arb self = lhs.arb(prec);
    Arb other = rhs.arb(prec);
    auto cmp = self.cmp(other);
    if (cmp.has_value()) {
      return *cmp < 0;
    }
  }
}

namespace exactreal {
template <typename Ring>
struct Element<Ring>::Implementation : ElementImplementation<Ring> {
  using ElementImplementation<Ring>::ElementImplementation;
};

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {}

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent,
                       const vector<typename Ring::ElementClass>& coefficients)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent,
                                                            coefficients)) {}

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent, const size_t gen)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {
  impl->coefficients[gen] = 1;
}

template <typename Ring>
Arb Element<Ring>::arb(long prec) const {
  prec +=
      numeric_cast<long>(ceil(log2(numeric_cast<double>(impl->parent.rank()))));
  Arb ret;
  for (size_t i = 0; i < impl->parent.rank(); i++) {
    ret += impl->parent.gens()[i]->arb(prec) *
           Ring::arb(impl->coefficients[i], prec);
  }
  return ret;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator+=(const Element<Ring>& rhs) {
  if (&impl->parent != &rhs.impl->parent) {
    throw logic_error(
        "not implemented - addition of Element with unrelated Element");
  }
  for (size_t i = 0; i < impl->parent.rank(); i++) {
    impl->coefficients[i] += rhs.impl->coefficients[i];
  }
  return *this;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator-=(const Element<Ring>& rhs) {
  return *this += -rhs;
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
Element<Ring>& Element<Ring>::operator*=(
    const typename Ring::ElementClass& rhs) {
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
        std::is_same_v<mpz, mpz_class> &&
            !std::is_same_v<typename Ring::ElementClass, mpz_class>,
        mpz>& rhs) {
  for (auto& c : impl->coefficients) {
    // cppcheck-suppress useStlAlgorithm
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
bool Element<Ring>::operator<(const Element<Ring>& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  return lt_assuming_ne(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator<(const RealNumber& rhs) const {
  if (this->operator==(rhs)) {
    return false;
  }
  return lt_assuming_ne(*this, rhs);
}

template <typename Ring>
bool Element<Ring>::operator==(const Element<Ring>& rhs) const {
  return &impl->parent == &rhs.impl->parent &&
         impl->coefficients == rhs.impl->coefficients;
}

template <typename Ring>
bool Element<Ring>::operator==(const RealNumber& rhs) const {
  auto gens = impl->parent.gens();
  auto it = find_if(gens.begin(), gens.end(),
                    [&](auto other) { return other.operator*() == rhs; });
  if (it == gens.end()) {
    throw logic_error(
        "not implemented - equality of Element with unrelated RealNumber");
  }
  for (size_t i = 0; i < impl->parent.rank(); i++) {
    if (impl->coefficients[i] == 0) {
      if (i == numeric_cast<size_t>(it - gens.begin())) {
        return false;
      }
    }
    if (impl->coefficients[i] == 1) {
      if (i != numeric_cast<size_t>(it - gens.begin())) {
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
  return std::any_of(impl->coefficients.begin(), impl->coefficients.end(),
                     [](auto c) { return c != 0; });
}

template <typename Ring>
ostream& operator<<(ostream& out, const Element<Ring>& self) {
  bool empty = true;
  for (size_t i = 0; i < self.impl->parent.rank(); i++) {
    if (!empty) {
      out << " + ";
    }
    if (self.impl->coefficients[i] != 0) {
      empty = false;
      out << self.impl->coefficients[i] << "*" << *self.impl->parent.gens()[i];
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

template struct exactreal::Element<IntegerRingTraits>;
template ostream& exactreal::operator<<<IntegerRingTraits>(
    ostream&, const Element<IntegerRingTraits>&);
template struct exactreal::Element<RationalFieldTraits>;
template ostream& exactreal::operator<<<RationalFieldTraits>(
    ostream&, const Element<RationalFieldTraits>&);
template Element<RationalFieldTraits>& exactreal::Element<RationalFieldTraits>::
operator*=<mpz_class>(const mpz_class& rhs);
template struct exactreal::Element<NumberFieldTraits>;
template ostream& exactreal::operator<<<NumberFieldTraits>(
    ostream&, const Element<NumberFieldTraits>&);
template Element<NumberFieldTraits>& exactreal::Element<NumberFieldTraits>::
operator*=<mpz_class>(const mpz_class& rhs);
