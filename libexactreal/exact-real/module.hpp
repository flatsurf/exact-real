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

#ifndef LIBEXACTREAL_MODULE_HPP
#define LIBEXACTREAL_MODULE_HPP

#include <boost/operators.hpp>
#include <vector>

#include "external/spimpl/spimpl.h"
#include "forward.hpp"

namespace exactreal {
template <typename Ring>
class LIBEXACTREAL_API Module : boost::equality_comparable<Module<Ring>>,
                                public std::enable_shared_from_this<Module<Ring>> {
 public:
  using Basis = std::vector<std::shared_ptr<const RealNumber>>;

  static std::shared_ptr<const Module<Ring>> make(const Basis&);

  static std::shared_ptr<const Module<Ring>> make(const Basis&, const Ring&);

  // Return the underlying ring (e.g., the number field)
  const Ring& ring() const;

  size rank() const;

  bool submodule(const Module&) const;

  const Basis& basis() const;
  Element<Ring> gen(size i) const;
  Element<Ring> zero() const;

  /// Return a one element in this module.
  /// If there is no such element, an exception is raised.
  Element<Ring> one() const;

  // Return whether this module has the same generators in the same order over the same ring.
  bool operator==(const Module<Ring>& rhs) const;

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Module<R>&);

  static std::shared_ptr<const Module<Ring>> span(const std::shared_ptr<const Module<Ring>>&, const std::shared_ptr<const Module<Ring>>&);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::unique_impl_ptr<Implementation> impl;

  Module(spimpl::unique_impl_ptr<Implementation>&&) LIBEXACTREAL_LOCAL;
};

template <typename R>
LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const Module<R>&);

}  // namespace exactreal

#endif
