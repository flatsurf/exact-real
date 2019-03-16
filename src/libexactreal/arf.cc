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

#include "exact-real/arf.hpp"

#include <arf.h>

using std::string;

namespace exactreal {
	Arf::Arf() {
		arf_init(t);
	}

	Arf::Arf(const string& mantissa, int base, long exponent) : Arf() {
		fmpz_t m, e;
		fmpz_init(m);
		fmpz_init(e);

		fmpz_set_str(m, mantissa.c_str(), base);
		fmpz_set_si(e, exponent);
		arf_set_fmpz_2exp(t, m, e);

		fmpz_clear(e);
		fmpz_clear(m);
	}

	Arf::Arf(const mpz_class& mantissa, long exponent) : Arf() {
		fmpz_t m, e;
		fmpz_init(m);
		fmpz_init(e);

		fmpz_set_mpz(m, mantissa.get_mpz_t());
		fmpz_set_si(e, exponent);
		arf_set_fmpz_2exp(t, m, e);

		fmpz_clear(e);
		fmpz_clear(m);
	}

	Arf::Arf(const long value) : Arf() {
		arf_set_si(t, value);
	}

	Arf::Arf(const Arf& value) : Arf() {
		arf_set(t, value.t);
	}

	Arf::Arf(Arf&& value) : Arf() {
		this->operator=(std::move(value));
	}

	Arf::~Arf() {
		arf_clear(t);
	}

	Arf::operator double() const {
		return arf_get_d(t,  ARF_RND_NEAR);
	}

	Arf& Arf::operator=(const Arf& rhs) {
		arf_set(t, rhs.t);
		return *this;
	}

	Arf& Arf::operator=(Arf&& rhs) {
		arf_swap(t, rhs.t);
		return *this;
	}

	Arf& Arf::iadd(const Arf& rhs, mp_limb_signed_t precision) {
		arf_add(t, t, rhs.t, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::isub(const Arf& rhs, mp_limb_signed_t precision) {
		arf_sub(t, t, rhs.t, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::imul(const Arf& rhs, mp_limb_signed_t precision) {
		arf_mul(t, t, rhs.t, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::idiv(const Arf& rhs, mp_limb_signed_t precision) {
		arf_div(t, t, rhs.t, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::iadd(const long rhs, mp_limb_signed_t precision) {
		arf_add_si(t, t, rhs, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::isub(const long rhs, mp_limb_signed_t precision) {
		arf_sub_si(t, t, rhs, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::imul(const long rhs, mp_limb_signed_t precision) {
		arf_mul_si(t, t, rhs, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf& Arf::idiv(const long rhs, mp_limb_signed_t precision) {
		arf_div_si(t, t, rhs, precision, ARF_RND_NEAR);
		return *this;
	}

	Arf Arf::abs() const {
		Arf ret;
		arf_abs(ret.t, t);
		return ret;
	}
}
