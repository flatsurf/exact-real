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

#include <cassert>
#include <memory>
#include <set>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "exact-real/cereal.hpp"
#include "exact-real/real_number.hpp"
#include "external/unique-factory/unique_factory.hpp"
#include "exact-real/yap/arf.hpp"

using namespace exactreal;
using std::make_shared;
using std::multiset;
using std::optional;
using std::shared_ptr;
using std::vector;

namespace {
// A product of transcendental reals
class RealNumberProduct final : public RealNumber {
 public:
  RealNumberProduct(const multiset<shared_ptr<const RealNumber>>& factors) : factors(factors) {}

  RealNumber const& operator>>(std::ostream& os) const override {
    bool first = true;
    for (auto& factor : factors) {
      if (!first) os << "*";
      first = false;
      os << *factor;
    }
    return *this;
  }

  explicit operator std::optional<mpq_class>() const override {
    return {};
  }

  Arf arf(long prec) const override {
    // Multiplications are quite stable, so adding that many digits of
    // precision is probably too much. Currently, factors.size() == 2 in our
    // application, so it does not matter.
    prec += factors.size() - 1;

    Arf ret(1);
    for (auto& factor : factors)
      ret *= factor->arf(prec)(prec, Arf::Round::NEAR);

    return ret;
  }

  multiset<shared_ptr<const RealNumber>> factors;

  template <typename Archive>
  void save(Archive& archive) const {
    archive(cereal::make_nvp("factors", vector<shared_ptr<const RealNumber>>(factors.begin(), factors.end())));
  }
};

auto& factory() {
  static UniqueFactory<RealNumberProduct, multiset<shared_ptr<const RealNumber>>> factory;
  return factory;
}
}  // namespace

namespace exactreal {
shared_ptr<const RealNumber> RealNumber::operator*(const RealNumber& rhs) const {
  auto rational = static_cast<std::optional<mpq_class>>(rhs);
  if (rational) {
    return rhs * *this;
  }

  multiset<shared_ptr<const RealNumber>> factors;
  for (auto factor : {this->shared_from_this(), rhs.shared_from_this()}) {
    assert(!static_cast<optional<mpq_class>>(*factor) && "All factors must be transcendental");
    if (typeid(factor) == typeid(std::shared_ptr<const RealNumberProduct>)) {
      for (auto& f : static_cast<const RealNumberProduct*>(&*factor)->factors) {
        factors.insert(f);
      }
    } else {
      factors.insert(factor);
    }
  }

  return factory().get(factors, [&]() { return new RealNumberProduct(factors); });
}

void save_product(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& base) {
  std::dynamic_pointer_cast<const RealNumberProduct>(base)->save(archive);
}

void load_product(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& base) {
  vector<shared_ptr<const RealNumber>> factors;
  archive(cereal::make_nvp("factors", factors));
  base = factory().get(multiset<shared_ptr<const RealNumber>>(factors.begin(), factors.end()), [&]() { return new RealNumberProduct(multiset<shared_ptr<const RealNumber>>(factors.begin(), factors.end())); });
}
}  // namespace exactreal
