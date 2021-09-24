#!/usr/bin/env python
# -*- coding: utf-8 -*-

######################################################################
#  This file is part of exact-real.
#
#        Copyright (C) 2019 Vincent Delecroix
#        Copyright (C) 2019 Julian Rüth
#
#  exact-real is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  exact-real is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
######################################################################

import sys
import pytest

from pyexactreal import exactreal
Arf = exactreal.Arf

def test_arithmetic():
    a = Arf(1)
    b = Arf(2)

    # Expressions do not get evaluated without specifying a precision/rounding
    assert str(a + a).startswith("expr<+>")

    assert (a + a)(64, Arf.Round.DOWN) == b

    b += a

    assert b(64, Arf.Round.DOWN) == 3 

    assert -(-a) == a

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))
