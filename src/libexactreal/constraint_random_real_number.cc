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

#include <arb.h>
#include <gmpxx.h>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <sstream>

#include "exact-real/real_number.hpp"
#include "exact-real/yap/arf.hpp"

using namespace exactreal;
using boost::lexical_cast;
using std::isfinite;
using std::make_unique;
using std::nextafter;
using std::numeric_limits;
using std::ostream;
using std::string;
using std::unique_ptr;

namespace {
unsigned int nextSeed = 1337;

// A random real number in [a, b]
class ConstraintRandomRealNumber final : public RealNumber {
 public:
  ConstraintRandomRealNumber(const Arf& a, const Arf& b) : inner(RealNumber::random()) {
    if (a >= b) {
      throw std::logic_error("interval must have an interior");
    }

    // We first rewrite a and b as mantissa + 2^exponent with the same exponent
    // and then select a random mantissa. Our random number is then going to be
    // this "mantissa" followed by random bits. Specifically, if we write
    // initial = m·2^e, then this returns initial + inner·2^(e - 1)

    auto a_mantissa = a.mantissa();
    auto a_exponent = a.exponent();

    auto b_mantissa = b.mantissa();
    auto b_exponent = b.exponent();

    // If a and b are very different in size, these shifts could lead to Out Of
    // Memory situations.

    if (a_exponent < b_exponent) {
      mpz_class delta = b_exponent - a_exponent;
      mpz_mul_2exp(b_mantissa.get_mpz_t(), b_mantissa.get_mpz_t(), delta.get_si());
      b_exponent = a_exponent;
    }

    if (b_exponent < a_exponent) {
      mpz_class delta = a_exponent - b_exponent;
      mpz_mul_2exp(a_mantissa.get_mpz_t(), a_mantissa.get_mpz_t(), delta.get_si());
      a_exponent = b_exponent;
    }

    mpz_class length = b_mantissa - a_mantissa;
    assert(length > 0);

    mpz_class mantissa;

    gmp_randstate_t rnd;
    gmp_randinit_default(rnd);
    gmp_randseed_ui(rnd, nextSeed++);
    mpz_urandomm(mantissa.get_mpz_t(), rnd, length.get_mpz_t());
    gmp_randclear(rnd);

    mantissa += a_mantissa;

    e = a_exponent.get_si();
    initial = Arf(mantissa, e);
  }

  virtual Arf arf(long prec) const override {
    if (prec < 1) {
      prec = 0;
    }

    long necessary_digits = prec + 1;

    Arf ret(initial);
    long initial_digits = initial == 0 ? 0 : (mpz_sizeinbase(initial.mantissa().get_mpz_t(), 2));
    long missing_digits = necessary_digits - initial_digits;

    // initial does not have enough valid digits, we need to ask inner for more
    // digits
    if (missing_digits > 0) {
      // If inner is smaller than ½, then this might be asking for a few
      // digits too many, but it does not matter in practice. (And we round
      // them away below anyway.)
      Arf additional(inner->arf(missing_digits - 1));
      additional <<= (e - 1);
      ret += additional(prec + 1)(Arf::Round::NEAR);
    }

    arf_set_round(ret.arf_t(), ret.arf_t(), prec + 1, ARF_RND_NEAR);
    return ret;
  }

  bool operator==(const RealNumber& rhs) const override {
    if (typeid(rhs) == typeid(*this)) {
      return this->initial == static_cast<const ConstraintRandomRealNumber*>(&rhs)->initial &&
             *(this->inner) == *(static_cast<const ConstraintRandomRealNumber*>(&rhs)->inner);
    } else {
      return false;
    }
  }

  RealNumber const& operator>>(ostream& out) const override {
    out << "ℝ(" << initial << " + " << *inner << "p" << e << ")";
    if (lexical_cast<string>(static_cast<double>(initial)).compare(lexical_cast<string>(static_cast<double>(*this)))) {
      out << "[∼" << static_cast<double>(*this) << "]";
    }
    return *this;
  }

 private:
  Arf initial;
  long e;
  unique_ptr<RealNumber> inner;
};
}  // namespace

namespace exactreal {
unique_ptr<RealNumber> RealNumber::random(const Arf& lower, const Arf& upper) {
  if (lower == 0 && upper == 1) {
    return RealNumber::random();
  } else {
    return make_unique<ConstraintRandomRealNumber>(lower, upper);
  }
}

unique_ptr<RealNumber> RealNumber::random(const double x) {
  if (!isfinite(x)) {
    throw std::logic_error("not implemented - random number close to non-finite");
  }

  // We rewrite x as a small interval such that any element of that interval
  // is closer to x than to another double.
  // We start with an interval that ends at the preceding and following
  // double…
  Arf lower = Arf(nextafter(x, -numeric_limits<double>::infinity()));
  Arf midpoint = Arf(x);
  Arf upper = Arf(nextafter(x, numeric_limits<double>::infinity()));

  // … and shrink it so it only contains midpoint.
  // We use 64 bits of precision, more than a double can possibly have:
  lower = ((lower + 2 * midpoint) / 3)(64, Arf::Round::NEAR);
  upper = ((2 * midpoint + upper) / 3)(64, Arf::Round::NEAR);

  // Check that x is the nearest double to [lower, upper]
  assert(static_cast<double>(lower) == x);
  assert(static_cast<double>(upper) == x);

  // Note that the result is smaller than 2^-1023 if x == 0 (which is probably
  // not what you wanted.)
  return random(lower, upper);
}
}  // namespace exactreal
