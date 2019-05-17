#!/usr/bin/env python

######################################################################
#  This file is part of exact-real.
#
#        Copyright (C) 2019 Vincent Delecroix
#        Copyright (C) 2019 Julian Rüth
#
#  exact-real is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
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

def test_module():
    from pyexactreal import exactreal
    ZZModule = exactreal.ZZModule
    RealNumber = exactreal.RealNumber

    M = ZZModule(RealNumber.rational(1), RealNumber.random())

    x = M.gen(1)
    # there used to be a segfault in calling module() on a temporary
    assert str((x*x).module()) == "ℤ-Module(ℝ(0.303644…)*ℝ(0.303644…), ℝ(0.303644…), 1)"

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))

