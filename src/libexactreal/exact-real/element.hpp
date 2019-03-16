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

#ifndef LIBMODEANTIC_ELEMENT_HPP
#define LIBMODEANTIC_ELEMENT_HPP

#include <gmpxx.h>
#include <boost/blank.hpp>
#include <boost/operators.hpp>
#include <optional>
#include <type_traits>
#include <vector>
#include "exact-real/external/spimpl/spimpl.h"

#include "exact-real/exact-real.hpp"

namespace exactreal {

struct Arb;

template <typename Ring>
struct Module;

struct RealNumber;

template <typename Ring>
struct Element
    : boost::additive<Element<Ring>>,
      boost::totally_ordered<Element<Ring>>,
      boost::totally_ordered<Element<Ring>, RealNumber>,
      boost::multiplicative<Element<Ring>, typename Ring::ElementClass>,
      std::conditional_t<
          std::is_same<typename Ring::ElementClass, mpz_class>::value,
          boost::blank, boost::multiplicative<Element<Ring>, mpz_class>>,
      boost::multiplicative<Element<Ring>, int> {
  explicit Element(const Module<Ring>& parent);
  Element(const Module<Ring>& parent,
          const std::vector<typename Ring::ElementClass>& coefficients);
  Element(const Module<Ring>& parent, const size_t gen);

  typename Ring::ElementClass operator[](const size_t) const;
  std::conditional<Ring::isField, mpq_class, mpz_class> operator[](
      const std::pair<size_t, size_t>&) const;

  Arb arb(long prec) const;

  Element& operator+=(const Element&);
  Element& operator-=(const Element&);
  Element operator-() const;
  Element& operator*=(const typename Ring::ElementClass&);
  // Define a operator*=(const mpz_class&) if Ring::ElementClass != mpz_class
  template <typename mpz = mpz_class>
  Element& operator*=(
      const typename std::enable_if_t<
          std::is_same_v<mpz, mpz_class> &&
              !std::is_same_v<typename Ring::ElementClass, mpz_class>,
          mpz>&);
  Element& operator*=(const int&);
  // Define a operator/=(const Ring::ElementClass&) if Ring is a field
  template <typename element = typename Ring::ElementClass>
  Element& operator/=(
      const typename std::enable_if_t<
          std::is_same_v<element, typename Ring::ElementClass> && Ring::isField,
          element>&);
  bool operator==(const Element&) const;
  bool operator<(const Element&) const;

  bool operator==(const RealNumber&) const;
  bool operator<(const RealNumber&) const;

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Element<R>&);

 private:
  struct Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

}  // namespace exactreal

#endif
