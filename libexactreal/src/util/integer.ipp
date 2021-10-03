/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019      Vincent Delecroix
 *        Copyright (C) 2019-2021 Julian Rüth
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

namespace exactreal {
namespace {
template <typename Integer>
auto to_supported_integer(Integer value) {
  using S = std::remove_cv_t<std::remove_reference_t<Integer>>;

  using Long = std::conditional_t<std::numeric_limits<S>::is_signed, slong, ulong>;
  if constexpr (std::numeric_limits<Long>::min() <= std::numeric_limits<S>::min() && std::numeric_limits<Long>::max() >= std::numeric_limits<S>::max()) {
    // We can safely cast to a supported type without overflow
    return static_cast<Long>(value);
  } else {
    return gmpxxll::mpz_class(value);
  }
}
}  // namespace
}  // namespace exactreal
