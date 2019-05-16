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

#include "exact-real/detail/smart_less.hpp"
#include "exact-real/real_number.hpp"
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
  template <typename Iterable>
  RealNumberProduct(const Iterable& factors) {
    for (auto& factor : factors) {
      assert(!static_cast<optional<mpq_class>>(*factor) && "All factors must be transcendental");
      if (typeid(factor) == typeid(*this)) {
        for (auto& f : static_cast<const RealNumberProduct*>(&*factor)->factors) {
          this->factors.insert(f);
        }
      } else {
        this->factors.insert(factor);
      }
    }
  }

  RealNumber const& operator>>(std::ostream& os) const override {
    bool first = true;
    for (auto& factor : factors) {
      if (!first) os << "*";
      first = false;
      os << *factor;
    }
    return *this;
  }

  bool operator==(const RealNumber& rhs) const override {
    if (typeid(rhs) == typeid(*this)) {
      return smart_eq(this->factors, static_cast<const RealNumberProduct*>(&rhs)->factors);
    } else {
      return false;
    }
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

 private:
  multiset<shared_ptr<const RealNumber>, smart_less<shared_ptr<const RealNumber>>> factors;
};
}  // namespace

namespace exactreal {
shared_ptr<const RealNumber> RealNumber::operator*(const RealNumber& rhs) const {
  auto rational = static_cast<std::optional<mpq_class>>(rhs);
  if (rational) {
    return rhs * *this;
  }
  vector<shared_ptr<const RealNumber>> gens;
  gens.push_back(this->shared_from_this());
  gens.push_back(rhs.shared_from_this());
  return make_shared<RealNumberProduct>(gens);
}

}  // namespace exactreal
