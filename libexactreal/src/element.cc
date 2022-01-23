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

#include "../exact-real/element.hpp"

#include <arb.h>
#include <e-antic/renfxx.h>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <map>
#include <set>

#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/yap/arb.hpp"
#include "external/gmpxxll/gmpxxll/mpz_class.hpp"
#include "legacy/1_4_0/element.hpp"
#include "util/assert.ipp"

using namespace exactreal;
using boost::numeric_cast;
using std::find_if;
using std::logic_error;
using std::map;
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
    LIBEXACTREAL_ASSERT(static_cast<size>(coefficients.size()) == parent->rank(), "Number of Coefficients " << coefficients.size() << " does not match rank of module " << parent->rank());
  }

  shared_ptr<const Module<Ring>> parent;
  vector<typename Ring::ElementClass> coefficients;
};

template <typename Ring, typename RHS>
bool lt(const Element<Ring>& lhs, const RHS& rhs) {
  if (lhs == rhs)
    return false;
  for (long prec = ARB_PRECISION_FAST;; prec *= 2) {
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
            return std::vector<typename Ring::ElementClass>(begin(coefficients), end(coefficients));
          }(value.coefficients()))) {}

template <typename Ring>
template <bool Enabled, std::enable_if_t<Enabled, bool>>
Element<Ring>::Element(const Element<RationalField>& value)
    : impl(spimpl::make_impl<Element<Ring>::Implementation>(
          Module<Ring>::make(value.module()->basis()),
          [&](const auto& coefficients) {
            return std::vector<typename Ring::ElementClass>(begin(coefficients), end(coefficients));
          }(value.coefficients()))) {}

template <typename Ring>
Element<Ring>::Element(const typename Ring::ElementClass& value) : Element(Module<Ring>::make({RealNumber::rational(1)}, Ring(value)), {value}) {
}

template <typename Ring>
typename Ring::ElementClass Element<Ring>::operator[](const size i) const {
  return impl->coefficients.at(i);
}

template <typename Ring>
std::vector<typename Ring::ElementClass> Element<Ring>::coefficients() const {
  return impl->coefficients;
}

template <typename Ring>
std::vector<mpq_class> Element<Ring>::rationalCoefficients() const {
  if constexpr (std::is_same_v<typename Ring::ElementClass, mpz_class>) {
    std::vector<mpq_class> coefficients;
    for (const auto& c : impl->coefficients) coefficients.push_back(c);
    return coefficients;
  } else if constexpr (std::is_same_v<typename Ring::ElementClass, mpq_class>) {
    return this->coefficients();
  } else if constexpr (std::is_same_v<typename Ring::ElementClass, eantic::renf_elem_class>) {
    std::vector<mpq_class> ret;
    for (auto& c : impl->coefficients) {
      mpz_class den = c.den();
      auto nums = c.num_vector();
      for (auto& num : nums) {
        ret.push_back(mpq_class(num, den));
      }
      LIBEXACTREAL_ASSERT(nums.size() <= impl->parent->ring().parameters->degree(), "rational coefficient list cannot be larger than absolute degree of number field");
      for (size_t i = nums.size(); i < impl->parent->ring().parameters->degree(); i++) {
        ret.push_back(0);
      }
    }
    return ret;
  } else {
    static_assert(false_t<typename Ring::ElementClass>, "unsupported coefficient type");
  }
}

template <typename Ring>
Arb Element<Ring>::arb(long accuracy) const {
  if (!*this)
    return Arb();

  using std::ceil;
  long prec = accuracy + numeric_cast<long>(ceil(log2(numeric_cast<double>(impl->parent->rank()))));

  while (true) {
    Arb ret;
    for (int i = 0; i < impl->parent->rank(); i++)
      ret += (impl->parent->basis()[i]->arb(prec) * Ring::arb(impl->coefficients[i], prec))(prec);

    if (arb_rel_accuracy_bits(ret.arb_t()) >= accuracy)
      return ret;

    prec *= 2;
  }
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

  vector<std::pair<shared_ptr<const RealNumber>, typename Ring::ElementClass>> sorted(begin(products), end(products));
  std::sort(begin(sorted), end(sorted), [](const auto& lhs, const auto& rhs) { return lhs.first->deglex(*rhs.first); });

  vector<shared_ptr<const RealNumber>> basis;
  vector<typename Ring::ElementClass> coefficients;
  for (auto& v : sorted) {
    basis.push_back(v.first);
    coefficients.push_back(v.second);
  }

  this->impl->parent = Module<Ring>::make(basis, this->impl->parent->ring());
  this->impl->coefficients = coefficients;

  LIBEXACTREAL_ASSERT(this->impl->parent->basis() == basis, "order of generators in module is not deglex which is the assumption by Element::operator/=");

  return *this;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator*=(const RealNumber& rhs) {
  return *this *= Module<Ring>::make({rhs.shared_from_this()}, this->module()->ring())->gen(0);
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
Element<Ring>& Element<Ring>::operator*=(const mpz_class& rhs) {
  for (auto& c : impl->coefficients) {
    c *= rhs;
  }
  return *this;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator*=(const mpq_class& rhs) {
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
Element<Ring>& Element<Ring>::operator/=(const mpz_class& rhs) {
  for (auto& c : impl->coefficients) {
    c /= rhs;
  }
  return *this;
}

template <typename Ring>
Element<Ring>& Element<Ring>::operator/=(const mpq_class& rhs) {
  for (auto& c : impl->coefficients) {
    c /= rhs;
  }
  return *this;
}

template <typename Ring>
std::optional<Element<Ring>> Element<Ring>::truediv(const Element<Ring>& rhs) const {
  LIBEXACTREAL_CHECK_ARGUMENT(rhs != 0, "division by zero");

  if (rhs == 1)
    return *this;

  if (impl->parent != rhs.impl->parent) {
    auto parent = Module<Ring>::span(this->impl->parent, rhs.impl->parent);
    return Element<Ring>(*this).promote(parent).truediv(Element<Ring>(rhs).promote(parent));
  }

  // Multivariate polynomial division.
  Element<Ring> remainder = *this;
  Element<Ring> quotient = this->module()->zero();

  const auto leading = [](const auto& x) {
    const auto coefficients = x.coefficients();
    for (size_t i = coefficients.size(); i > 0; i--) {
      if (coefficients[i - 1])
        return std::tuple(coefficients[i - 1], x.module()->basis()[i - 1]);
    }
    throw std::logic_error("zero element has no leading coefficient");
  };

  while (remainder) {
    auto [a, g] = leading(remainder);
    auto [b, h] = leading(rhs);

    auto generator_quotient = *g / *h;
    if (!generator_quotient.has_value())
      return std::nullopt;

    auto coefficient_quotient = a / b;

    if (coefficient_quotient * b != a) {
      LIBEXACTREAL_ASSERT(!Ring::isField, "division of coefficients must be exact in fields");
      return std::nullopt;
    }

    auto partial_quotient = coefficient_quotient * Module<Ring>::make({*generator_quotient}, this->module()->ring())->gen(0);

    quotient += partial_quotient;
    remainder -= partial_quotient * rhs;
  }

  return quotient;
}

template <typename Ring>
mpz_class Element<Ring>::floordiv(const Element<Ring>& rhs) const {
  if (!*this)
    return 0;

  if (impl->parent != rhs.impl->parent) {
    auto parent = Module<Ring>::span(this->impl->parent, rhs.impl->parent);
    return Element<Ring>(*this).promote(parent).floordiv(Element<Ring>(rhs).promote(parent));
  }

  // Try an exact division for the common case that the operands are integer
  // multiples of each other.
  const auto exact = [&]() -> std::optional<mpz_class> {
    std::optional<mpz_class> floor;

    for (size_t i = 0; i < impl->coefficients.size(); i++) {
      if (impl->coefficients[i]) {
        if (rhs.impl->coefficients[i]) {
          const auto f = Ring::floor(impl->coefficients[i] / rhs.impl->coefficients[i]);
          if (floor)
            if (*floor != f)
              return std::nullopt;
          floor = f;
          if (rhs.impl->coefficients[i] * f != impl->coefficients[i])
            return std::nullopt;
        } else {
          return std::nullopt;
        }
      } else {
        if (rhs.impl->coefficients[i])
          return std::nullopt;
      }
    }

    return floor;
  }();

  if (exact)
    return *exact;

  for (long prec = ARB_PRECISION_FAST;; prec *= 2) {
    const auto div = rhs.arb(prec);
    if (!(div != 0))
      // the divident ball contains zero
      continue;

    const Arb quotient = (arb(prec) / div)(prec);

    const auto [lower, upper] = static_cast<std::pair<Arf, Arf>>(quotient);

    if (lower.floor() == upper.floor())
      return lower.floor();
  }
}

template <typename Ring>
bool Element<Ring>::unit() const {
  if (!*this)
    return false;

  for (size i = 0; i < impl->parent->rank(); i++) {
    const auto& c = impl->coefficients[i];
    if (c == 0)
      continue;
    if (!static_cast<std::optional<mpq_class>>(impl->parent->gen(i)))
      return false;
    if (!Ring::unit(impl->coefficients[i]))
      return false;
  }

  return true;
}

template <typename Ring>
mpz_class Element<Ring>::floor() const {
  const auto integer = static_cast<std::optional<mpz_class>>(*this);
  if (integer) return *integer;

  for (long prec = ARB_PRECISION_FAST;; prec *= 2) {
    const Arb approximation = arb(prec);
    const auto [lower, upper] = static_cast<std::pair<Arf, Arf>>(approximation);

    if (lower.floor() == upper.floor())
      return lower.floor();
  }
}

template <typename Ring>
mpz_class Element<Ring>::ceil() const {
  const auto integer = static_cast<std::optional<mpz_class>>(*this);
  if (integer) return *integer;

  for (long prec = ARB_PRECISION_FAST;; prec *= 2) {
    const Arb approximation = arb(prec);
    const auto [lower, upper] = static_cast<std::pair<Arf, Arf>>(approximation);

    if (lower.ceil() == upper.ceil())
      return lower.ceil();
  }
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
  const auto rational = static_cast<std::optional<mpq_class>>(*this);
  return rational && *rational == rhs;
}

template <typename Ring>
Element<Ring>::operator std::optional<mpz_class>() const {
  const auto rational = static_cast<std::optional<mpq_class>>(*this);

  if (rational && rational->get_den() == 1)
    return rational->get_num();

  return std::nullopt;
}

template <typename Ring>
Element<Ring>::operator std::optional<mpq_class>() const {
  mpq_class ret;

  for (size_t i = 0; i < module()->rank(); i++) {
    const auto& coefficient = impl->coefficients[i];
    if (coefficient) {
      const auto rational = static_cast<std::optional<mpq_class>>(*module()->basis()[i]);
      if (rational) {
        const auto rationalCoefficient = Ring::rational(coefficient);
        if (!rationalCoefficient)
          return std::nullopt;
        ret += *rational * *rationalCoefficient;
      } else {
        return std::nullopt;
      }
    }
  }

  return ret;
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
  if (!*this)
    return !rhs;

  const auto& gens = impl->parent->basis();
  const auto it = find_if(begin(gens), end(gens), [&](const auto& other) { return *other == rhs; });

  LIBEXACTREAL_CHECK_ARGUMENT(it != end(gens), "not implemented - equality of Element " << *this << " with unrelated RealNumber " << rhs);

  const size at = it - begin(gens);
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
  return std::any_of(begin(impl->coefficients), end(impl->coefficients), [](const auto& c) { return c != 0; });
}

template <typename Ring>
Element<Ring>::operator double() const {
  {
    auto rational = static_cast<std::optional<mpq_class>>(*this);
    if (rational) {
      Arf a;

      fmpq_t q;
      fmpq_init_set_readonly(q, rational->get_mpq_t());

      arf_set_fmpq(a.arf_t(), q, DBL_MANT_DIG, ARF_RND_NEAR);

      fmpq_clear_readonly(q);

      return static_cast<double>(a);
    }
  }

  // Since this is not a rational number, we can look at the approximate ball
  // describing this value. Refining its radius, the entire ball will
  // eventually be closer to a single double than to any other double.
  // We will certainly need 53 bits to see the full mantissa.
  // Since we ask for an additional bit, the ball around our
  // approximation contains at most one double. Since we ask for
  // yet another bit, we know that any double contained in that
  // ball is closer to every point of the ball than any other
  // double.
  long prec = DBL_MANT_DIG + 2;

  while (true) {
    Arb a = arb(prec);
    auto bounds = static_cast<std::pair<Arf, Arf>>(a);
    auto lbound = static_cast<double>(bounds.first);
    auto ubound = static_cast<double>(bounds.second);
    if (lbound == ubound)
      return lbound;

    prec *= 2;
  }
}

template <typename Ring>
const shared_ptr<const Module<Ring>> Element<Ring>::module() const {
  return impl->parent;
}

template <typename Ring>
Element<Ring>& Element<Ring>::promote(const shared_ptr<const Module<Ring>>& parent) {
  if (this->impl->parent == parent)
    return *this;
  if (!*this)
    return *this = parent->zero();

  const auto& their_gens = parent->basis();
  const auto& our_gens = impl->parent->basis();

  auto their_gen = begin(their_gens);
  auto our_gen = begin(our_gens);

  vector<typename Ring::ElementClass> coefficients;

  while (their_gen != end(their_gens) || our_gen != end(our_gens)) {
    if (our_gen == end(our_gens)) {
      coefficients.push_back(typename Ring::ElementClass());
      their_gen++;
      continue;
    }
    if (their_gen == end(their_gens)) {
      LIBEXACTREAL_ASSERT(!impl->coefficients[our_gen - begin(our_gens)], "Cannot promote " << *this << " to " << *parent << " since " << *our_gen << " is not in " << *parent);
      our_gen++;
      continue;
    }
    if (**our_gen == **their_gen) {
      coefficients.push_back(parent->ring().coerce(impl->coefficients[our_gen - begin(our_gens)]));
      our_gen++;
      their_gen++;
      continue;
    }
    if ((**our_gen).deglex(**their_gen)) {
      our_gen++;
      continue;
    } else {
      coefficients.push_back(typename Ring::ElementClass());
      their_gen++;
      continue;
    }
  }

  return *this = Element<Ring>(parent, coefficients);
}

template <typename Ring>
Element<Ring>& Element<Ring>::simplify() {
  if (std::all_of(begin(impl->coefficients), end(impl->coefficients), [](const auto& c) { return c; }))
    return *this;

  typename Module<Ring>::Basis gens;

  auto& our_gens = impl->parent->basis();

  for (auto gen = begin(our_gens); gen != end(our_gens); gen++) {
    if (impl->coefficients[gen - begin(our_gens)])
      gens.push_back(*gen);
  }

  if (gens.size() == impl->parent->rank())
    return *this;

  const auto parent = Module<Ring>::make(gens, impl->parent->ring());
  return promote(parent);
}

template <typename Ring>
std::ostream& operator<<(std::ostream& out, const Element<Ring>& self) {
  // Print summands sorted by absolute generator value to get stable outputs.
  // (additionally, print positive coefficients first so that we do not get
  // leading minus signs.)
  std::set<std::tuple<bool, Element<Ring>, typename Ring::ElementClass>> coefficients;
  for (size i = 0; i < self.impl->parent->rank(); i++) {
    coefficients.insert(std::tuple(
        self.impl->coefficients[i] < 0,
        Element<Ring>(Module<Ring>::make({self.impl->parent->basis()[i]}, self.impl->parent->ring())->gen(0)),
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

template <typename Ring, typename C>
std::vector<C> Element_coefficients_1_4_0(const Element<Ring>& element) {
  if constexpr (std::is_same_v<C, typename Ring::ElementClass>) {
    return element.coefficients();
  } else if constexpr (std::is_same_v<C, mpq_class>) {
    return element.rationalCoefficients();
  } else {
    static_assert(false_t<C>, "no coefficients<C> for this C implemented");
  }
}

template <typename Ring, typename T>
Element<Ring>& Element_operator_mul_1_4_0(Element<Ring>& x, const T& c) {
  return x *= c;
}

template <typename Ring, typename T>
Element<Ring>& Element_operator_div_1_4_0(Element<Ring>& x, const T& c) {
  return x /= c;
}

}  // namespace exactreal

namespace std {
template <typename Ring>
size_t hash<exactreal::Element<Ring>>::operator()(const exactreal::Element<Ring>& x) const noexcept {
  return hash<double>()(static_cast<double>(x));
}
}  // namespace std

// Explicit instantiations of templates so that code is generated for the
// linker.
#include "../exact-real/integer_ring.hpp"
#include "../exact-real/number_field.hpp"
#include "../exact-real/rational_field.hpp"

namespace exactreal {

template class Element<IntegerRing>;
template std::ostream& operator<<(std::ostream&, const Element<IntegerRing>&);
template Element<IntegerRing>& Element<IntegerRing>::operator*=(const int&);
template std::vector<mpz_class> Element_coefficients_1_4_0<IntegerRing, mpz_class>(const Element<IntegerRing>&);
template std::vector<mpq_class> Element_coefficients_1_4_0<IntegerRing, mpq_class>(const Element<IntegerRing>&);
template Element<IntegerRing>& Element_operator_mul_1_4_0(Element<IntegerRing>&, const mpz_class&);

template class Element<RationalField>;
template Element<RationalField>::Element(const Element<IntegerRing>&);
template std::ostream& operator<<(std::ostream&, const Element<RationalField>&);
template Element<RationalField>& Element<RationalField>::operator*=(const int&);
template Element<RationalField>& Element<RationalField>::operator/=(const int&);
template std::vector<mpq_class> Element_coefficients_1_4_0<RationalField, mpq_class>(const Element<RationalField>&);
template Element<RationalField>& Element_operator_mul_1_4_0(Element<RationalField>&, const mpz_class&);
template Element<RationalField>& Element_operator_mul_1_4_0(Element<RationalField>&, const mpq_class&);
template Element<RationalField>& Element_operator_div_1_4_0(Element<RationalField>&, const mpz_class&);
template Element<RationalField>& Element_operator_div_1_4_0(Element<RationalField>&, const mpq_class&);

template class Element<NumberField>;
template Element<NumberField>::Element(const Element<IntegerRing>&);
template Element<NumberField>::Element(const Element<RationalField>&);
template std::ostream& operator<<(std::ostream&, const Element<NumberField>&);
template Element<NumberField>& Element<NumberField>::operator*=(const int&);
template Element<NumberField>& Element<NumberField>::operator*=(const eantic::renf_elem_class&);
template Element<NumberField>& Element<NumberField>::operator/=(const int&);
template Element<NumberField>& Element<NumberField>::operator/=(const eantic::renf_elem_class&);
template std::vector<mpq_class> Element_coefficients_1_4_0<NumberField, mpq_class>(const Element<NumberField>&);
template std::vector<eantic::renf_elem_class> Element_coefficients_1_4_0<NumberField, eantic::renf_elem_class>(const Element<NumberField>&);
template Element<NumberField>& Element_operator_mul_1_4_0(Element<NumberField>&, const mpz_class&);
template Element<NumberField>& Element_operator_mul_1_4_0(Element<NumberField>&, const mpq_class&);
template Element<NumberField>& Element_operator_div_1_4_0(Element<NumberField>&, const mpz_class&);
template Element<NumberField>& Element_operator_div_1_4_0(Element<NumberField>&, const mpq_class&);
}  // namespace exactreal

namespace std {
template struct hash<exactreal::Element<IntegerRing>>;
template struct hash<exactreal::Element<RationalField>>;
template struct hash<exactreal::Element<NumberField>>;
}  // namespace std
