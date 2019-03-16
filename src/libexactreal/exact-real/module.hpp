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

#ifndef LIBMODEANTIC_MODULE_HPP
#define LIBMODEANTIC_MODULE_HPP

#include <vector>
#include "exact-real/external/spimpl/spimpl.h"

#include "exact-real/exact-real.hpp"
#include "exact-real/ring.hpp"

namespace exactreal {

struct RealNumber;

template <typename Ring>
struct Element;

template <typename Ring>
struct Module {
  template <typename RingWithoutParameters = Ring,
            typename std::enable_if_t<
                std::is_same_v<Ring, RingWithoutParameters> &&
                    !has_parameters<RingWithoutParameters>::value,
                int> = 0>
  explicit Module(const std::vector<std::shared_ptr<RealNumber>>& basis,
                  long precision = 64);

  template <
      typename RingWithParameters = Ring,
      typename std::enable_if_t<std::is_same_v<Ring, RingWithParameters> &&
                                    has_parameters<RingWithParameters>::value,
                                int> = 0>
  ///	typename std::enable_if_t<has_parameters<Ring>::value>>
  explicit Module(const std::vector<std::shared_ptr<RealNumber>>&,
                  const typename RingWithParameters::Parameters& ring,
                  long precision = 64);

  // Return the specific ring (e.g., the number field); only enabled if the
  // ring has such data, e.g., not for the integers or the rationals.
  template <class RingWithParameters = Ring>
  std::enable_if_t<std::is_same_v<Ring, RingWithParameters> &&
                       has_parameters<RingWithParameters>::value,
                   typename RingWithParameters::Parameters>&
  ring();

  size_t rank() const;

  std::vector<std::shared_ptr<RealNumber>> const& gens() const;
  Element<Ring> zero() const;
  Element<Ring> one() const;

 private:
  struct Implementation;
  spimpl::unique_impl_ptr<Implementation> impl;
};

}  // namespace exactreal

#endif
