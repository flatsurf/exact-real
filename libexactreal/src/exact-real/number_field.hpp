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

#ifndef LIBEXACTREAL_NUMBER_FIELD_HPP
#define LIBEXACTREAL_NUMBER_FIELD_HPP

#include <memory>
#include <boost/operators.hpp>

#include "e-antic/renfxx_fwd.h"
#include "exact-real/exact-real.hpp"
#include "exact-real/forward.hpp"

namespace exactreal {

class NumberField : boost::equality_comparable<NumberField> {
 public:
  NumberField();
  NumberField(const std::shared_ptr<const eantic::renf_class>&);

  std::shared_ptr<const eantic::renf_class> parameters;

  bool operator==(const NumberField&) const;

  typedef eantic::renf_elem_class ElementClass;
  static constexpr bool isField = true;
  static Arb arb(const ElementClass& x, long prec);
};

}  // namespace exactreal

#endif
