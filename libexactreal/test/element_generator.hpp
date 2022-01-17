/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2022 Julian Rüth
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

#ifndef EXACT_REAL_TEST_ELEMENT_GENERATOR_HPP
#define EXACT_REAL_TEST_ELEMENT_GENERATOR_HPP

#include <memory>

#include "../exact-real/module.hpp"
#include "../exact-real/element.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

namespace {

template<typename R>
struct ElementGenerator : public Catch::Generators::IGenerator<exactreal::Element<R>>
{
  int generator = -1;

  mutable exactreal::Element<R> current;

  std::shared_ptr<const exactreal::Module<R>> parent;

  ElementGenerator(const exactreal::Module<R>& parent) : parent(parent.shared_from_this()) {}

  bool next() override {
    generator++;
    return generator < parent->rank();
  }

  const exactreal::Element<R>& get() const override {
    if (generator == -1)
      current = parent->zero();
    else
      current = parent->gen(generator);

    return current;
  }
};

/*
 * Wrap ElementGenerator for use as GENERATE(elements(...))
 */
template <typename R>
inline Catch::Generators::GeneratorWrapper<exactreal::Element<R>> elements(const exactreal::Module<R>& parent) {
    return Catch::Generators::GeneratorWrapper<exactreal::Element<R>>(std::unique_ptr<Catch::Generators::IGenerator<exactreal::Element<R>>>(new ElementGenerator<R>(parent)));
}

}

#endif


