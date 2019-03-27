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

#include "exact-real/element.hpp"
#include "exact-real/module.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arb.hpp"

using namespace exactreal;
using boost::numeric_cast;
using std::find_if;
using std::logic_error;
using std::ostream;
using std::vector;

namespace {
template <typename Ring>
struct ElementImplementation {
  ElementImplementation() = delete;

  explicit ElementImplementation(const Module<Ring>& parent)
      : ElementImplementation(parent, vector<typename Ring::ElementClass>(parent.rank())) {}

  ElementImplementation(const Module<Ring>& parent, const vector<typename Ring::ElementClass>& coefficients)
      : parent(parent), coefficients(coefficients) {
    assert(size(coefficients.size()) == parent.rank());
  }

  ElementImplementation(const ElementImplementation& rhs) : ElementImplementation(rhs.parent, rhs.coefficients) {}

  ElementImplementation(const ElementImplementation&& rhs) =
      delete;  //: ElementImplementation(rhs.parent, rhs.coefficients) {}

  ElementImplementation& operator=(ElementImplementation&&) = delete;
  ElementImplementation& operator=(const ElementImplementation&) = delete;

  const Module<Ring>& parent;
  vector<typename Ring::ElementClass> coefficients;
};
}  // namespace

template <typename LHS, typename RHS>
bool lt_assuming_ne(const LHS& lhs, const RHS& rhs) {
  for (long prec = 64;; prec *= 2) {
    Arb self = lhs.arb(prec);
    Arb other = rhs.arb(prec);
    if (self < other) {
      return true;
    } else if (self > other) {
      return false;
    }
  }
}

namespace exactreal {
template <typename Ring>
struct Element<Ring>::Implementation : ElementImplementation<Ring> {
  using ElementImplementation<Ring>::ElementImplementation;
};

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent) : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {}

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent, const vector<typename Ring::ElementClass>& coefficients)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent, coefficients)) {}

template <typename Ring>
Element<Ring>::Element(const Module<Ring>& parent, const size gen)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {
  impl->coefficients[gen] = 1;
}

template <typename Ring>
Arb Element<Ring>::arb(long prec) const {
  prec += numeric_cast<long>(ceil(log2(numeric_cast<double>(impl->parent.rank()))));
  Arb ret;
  for (size i = 0; i < impl->parent.rank(); i++) {
    ret += (impl->parent.gens()[i]->arb(prec) * Ring::arb(impl->coefficients[i], prec))(prec);
  }
  return ret;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator+=(const Element<Ring>& rhs) {
  if (&impl->parent != &rhs.impl->parent) {
    throw logic_error("not implemented - addition of Element with unrelated Element");
  }
  for (size i = 0; i < impl->parent.rank(); i++) {
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

  for (size i = 0; i < impl->parent.rank(); i++) {
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
  return &impl->parent == &rhs.impl->parent && impl->coefficients == rhs.impl->coefficients;
}

template <typename Ring>
bool Element<Ring>::operator==(const RealNumber& rhs) const {
  auto gens = impl->parent.gens();
  auto it = find_if(gens.begin(), gens.end(), [&](auto other) { return other.operator*() == rhs; });
  if (it == gens.end()) {
    throw logic_error("not implemented - equality of Element with unrelated RealNumber");
  }
  for (size i = 0; i < impl->parent.rank(); i++) {
    if (impl->coefficients[i] == 0) {
      if (i == (it - gens.begin())) {
        return false;
      }
    }
    if (impl->coefficients[i] == 1) {
      if (i != (it - gens.begin())) {
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
ostream& operator<<(ostream& out, const Element<Ring>& self) {
  bool empty = true;
  for (size i = 0; i < self.impl->parent.rank(); i++) {
    if (self.impl->coefficients[i] != 0) {
      if (!empty) {
        out << " + ";
      }
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
template ostream& exactreal::operator<<<IntegerRingTraits>(ostream&, const Element<IntegerRingTraits>&);
template struct exactreal::Element<RationalFieldTraits>;
template ostream& exactreal::operator<<<RationalFieldTraits>(ostream&, const Element<RationalFieldTraits>&);
template Element<RationalFieldTraits>& exactreal::Element<RationalFieldTraits>::operator*=
    <mpz_class>(const mpz_class& rhs);
template struct exactreal::Element<NumberFieldTraits>;
template ostream& exactreal::operator<<<NumberFieldTraits>(ostream&, const Element<NumberFieldTraits>&);
template Element<NumberFieldTraits>& exactreal::Element<NumberFieldTraits>::operator*=<mpz_class>(const mpz_class& rhs);
