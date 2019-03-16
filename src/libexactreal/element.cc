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

#include "exact-real/element.hpp"
#include "exact-real/module.hpp"
#include "exact-real/arb.hpp"

using namespace exactreal;
using boost::numeric_cast;
using std::vector;
using std::ostream;
using std::find_if;
using std::logic_error;

namespace {
template<typename Ring>
struct ElementImplementation {
	explicit ElementImplementation(const Module<Ring>& parent) : parent(parent), coefficients(vector<typename Ring::ElementClass>(parent.rank())) {}

	ElementImplementation(const Module<Ring>& parent, const vector<typename Ring::ElementClass>& coefficients) : parent(parent), coefficients(coefficients) {
		assert(coefficients.size() == parent.rank());
	}

	const Module<Ring>& parent;
	vector<typename Ring::ElementClass> coefficients;
};
}

template<typename LHS, typename RHS>
bool lt_assuming_ne(const LHS& lhs, const RHS& rhs) {
  for (long prec = 64;; prec*= 2) {
  	Arb self = lhs.arb(prec);
  	Arb other = rhs.arb(prec);
  	auto cmp = self.cmp(other);
  	if (cmp.has_value()) {
  		return *cmp < 0;
  	}
  }
}

namespace exactreal {
template<typename Ring>
struct Element<Ring>::Implementation : ElementImplementation<Ring> {
	using ElementImplementation<Ring>::ElementImplementation;
};

template<typename Ring>
Element<Ring>::Element(const Module<Ring>& parent) : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {}

template<typename Ring>
Element<Ring>::Element(const Module<Ring>& parent, const vector<typename Ring::ElementClass>& coefficients) : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent, coefficients)) {}

template<typename Ring>
Element<Ring>::Element(const Module<Ring>& parent, const size_t gen) : impl(spimpl::make_impl<Element<Ring>::Implementation>(parent)) {
	impl->coefficients[gen] = 1;
}

template<typename Ring>
Arb Element<Ring>::arb(long prec) const {
	prec += numeric_cast<long>(ceil(log2(numeric_cast<double>(impl->parent.rank()))));
	Arb ret;
	for (size_t i = 0; i < impl->parent.rank(); i++) {
		ret += impl->parent.gens()[i]->arb(prec) * Ring::arb(impl->coefficients[i], prec);
	}
	return ret;
}

template<typename Ring>
Element<Ring>& Element<Ring>::operator+=(const Element<Ring>& rhs) {
	if (&impl->parent != &rhs.impl->parent) {
		throw logic_error("not implemented - addition of Element with unrelated Element");
	}
	for (size_t i = 0; i < impl->parent.rank(); i++) {
		impl->coefficients[i] += rhs.impl->coefficients[i];
	}
	return *this;
}

template<typename Ring>
Element<Ring>& Element<Ring>::operator-=(const Element<Ring>& rhs) {
	return *this += -rhs;
}

template<typename Ring>
Element<Ring> Element<Ring>::operator-() const {
	std::vector<typename Ring::ElementClass> coefficients = impl->coefficients;
	for (auto& c : coefficients) {
		c = -c;
	}
	return Element<Ring>(impl->parent, coefficients);
}

template<typename Ring>
Element<Ring>& Element<Ring>::operator*=(const int& rhs) {
	for (auto& c : impl->coefficients) {
		c *= rhs;
	}
	return *this;
}

template<typename Ring>
Element<Ring>& Element<Ring>::operator*=(const typename Ring::ElementClass& rhs) {
	// TODO: Check that rhs is coming from the same number field.
	for (auto& c : impl->coefficients) {
		c *= rhs;
	}
	return *this;
}

template<typename Ring>
template<typename mpz>
Element<Ring>& Element<Ring>::operator*=(const typename std::enable_if_t<std::is_same_v<mpz, mpz_class> && !std::is_same_v<typename Ring::ElementClass, mpz_class>, mpz>& rhs) {
	for (auto& c : impl->coefficients) {
		c *= rhs;
	}
	return *this;
}

template<typename Ring>
bool Element<Ring>::operator<(const Element<Ring>& rhs) const {
	if (this->operator==(rhs)) {
		return false;
	}
	return lt_assuming_ne(*this, rhs);
}

template<typename Ring>
bool Element<Ring>::operator<(const RealNumber& rhs) const {
	if (this->operator==(rhs)){
		return false;
	}
	return lt_assuming_ne(*this, rhs);
}

template<typename Ring>
bool Element<Ring>::operator==(const Element<Ring>& rhs) const {
	return &impl->parent == &rhs.impl->parent
		&& impl->coefficients == rhs.impl->coefficients;
}

template<typename Ring>
bool Element<Ring>::operator==(const RealNumber& rhs) const {
	auto gens = impl->parent.gens();
	auto it = find_if(gens.begin(), gens.end(), [&](auto other) { return other.operator*() == rhs; });
	if (it == gens.end()) {
		throw logic_error("not implemented - equality of Element with unrelated RealNumber");
	}
	for(size_t i = 0; i<impl->parent.rank();i++){
		if (impl->coefficients[i] == 0) {
			if (i == numeric_cast<size_t>(it -gens.begin())) {
				return false;
			}
		}
		if (impl->coefficients[i] == 1) {
			if (i != numeric_cast<size_t>(it -gens.begin())) {
				return false;
			}
		} else {
			return false;
		}
	}
	return true;
}

template<typename Ring>
ostream& operator<<(ostream& out, const Element<Ring>& self) {
	bool empty = true;
	for(size_t i = 0; i < self.impl->parent.rank(); i++) {
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
}

// Explicit instantiations of templates so that code is generated for the linker.
#include "exact-real/integer_ring.hpp"
#include "exact-real/rational_field.hpp"
#include "exact-real/number_field.hpp"

template struct exactreal::Element<IntegerRing>;
template ostream& exactreal::operator<<<IntegerRing>(ostream&, const Element<IntegerRing>&);
template struct exactreal::Element<RationalField>;
template ostream& exactreal::operator<<<RationalField>(ostream&, const Element<RationalField>&);
template Element<RationalField>& exactreal::Element<RationalField>::operator*=<mpz_class>(const mpz_class& rhs);
template struct exactreal::Element<NumberField>;
template ostream& exactreal::operator<<<NumberField>(ostream&, const Element<NumberField>&);
template Element<NumberField>& exactreal::Element<NumberField>::operator*=<mpz_class>(const mpz_class& rhs);
