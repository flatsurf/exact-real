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

/*
 * This file is included by all headers that are shipped with this library.
 * It should therefore contain global definitions but only those that can
 * be safely shipped to the client, i.e., not the ones in config.h
 */

#ifndef LIBMODEANTIC_MODEANTIC_HPP
#define LIBMODEANTIC_MODEANTIC_HPP

#ifdef __CLING__

#pragma cling add_library_path("/usr/local/lib")
#pragma cling load("libexactreal")

#endif  // __CLING__

#endif  // LIBMODEANTIC_MODEANTIC_HPP
