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
  mutable exactreal::Element<R> current;

  std::shared_ptr<const exactreal::Module<R>> parent;

  std::vector<typename R::ElementClass> coefficients;

  ElementGenerator(const exactreal::Module<R>& parent) : parent(parent.shared_from_this()) {
    coefficients.push_back(0);
    coefficients.push_back(1);
    coefficients.push_back(-2);
    if constexpr (!std::is_same_v<R, exactreal::IntegerRing>) {
      coefficients.push_back(mpq_class{-1, 3});
      coefficients.push_back(mpq_class{2, 5});
    }
    if constexpr (std::is_same_v<R, exactreal::NumberField>) {
      coefficients.push_back(parent.ring().parameters->gen());
      coefficients.push_back(parent.ring().parameters->gen() / 3 + 1);
    }

    current = parent.zero();
  }

  bool next() override {
    auto coefficients = current.coefficients();

    const std::function<bool(int)> increment = [&](int c) -> bool {
      if (c == coefficients.size())
        return false;

      for (int i = 0; i < this->coefficients.size() - 1; i++) {
        if (coefficients[c] == this->coefficients[i]) {
          coefficients[c] = this->coefficients[i + 1];
          return true;
        }
      }

      coefficients[c] = this->coefficients[0];
      return increment(c + 1);
    };
    
    if (!increment(0))
      return false;

    current = exactreal::Element(parent, coefficients);

    return true;
  }

  const exactreal::Element<R>& get() const override {
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


