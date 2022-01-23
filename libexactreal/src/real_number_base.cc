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

#include <atomic>

#include "impl/real_number_base.hpp"

namespace exactreal {

static std::atomic<size_t> next = 2147483659;

RealNumberBase::RealNumberBase() : uniqueId(next++) {}

size_t RealNumberBase::id(const RealNumber& self) { return static_cast<const RealNumberBase&>(self).uniqueId; }

}
