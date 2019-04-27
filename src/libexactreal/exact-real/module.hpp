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

#ifndef LIBEXACTREAL_MODULE_HPP
#define LIBEXACTREAL_MODULE_HPP

#include <vector>
#include "exact-real/external/spimpl/spimpl.h"

#include "exact-real/exact-real.hpp"
#include "exact-real/forward.hpp"
#include "exact-real/ring_traits.hpp"

namespace exactreal {
template <typename Ring>
class Module : std::enable_shared_from_this<Module<Ring>> {
 public:
  using Basis = std::vector<std::shared_ptr<RealNumber>>;

  Module();

  template <typename RingWithoutParameters = Ring>
  explicit Module(const Basis&);

  template <typename RingWithParameters = Ring>
  explicit Module(const Basis&, const typename RingWithParameters::Parameters&);

  // Return the specific ring (e.g., the number field); only enabled if the
  // ring has such data, e.g., not for the integers or the rationals.
  template <class RingWithParameters = Ring>
  const typename RingWithParameters::Parameters& ring() const;

  size rank() const;

  const Basis& gens() const;

  static const std::shared_ptr<const Module> trivial;

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Module<R>&);

  static std::shared_ptr<const Module<Ring>> span(const std::shared_ptr<const Module<Ring>>&, const std::shared_ptr<const Module<Ring>>&);

 private:
  struct Implementation;
  spimpl::unique_impl_ptr<Implementation> impl;
};

}  // namespace exactreal

#endif
