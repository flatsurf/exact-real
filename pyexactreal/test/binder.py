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

# Check that the contents of the Binder sample notebook work as expected; see
# /binder/Sample.pyexactreal.ipynb

def test_binder():
    from pyexactreal import RealNumber, ZZModule

    M = ZZModule(RealNumber.rational(1), RealNumber.random())
    assert str(M) == "ℤ-Module(1, ℝ(0.303644…))"

    x = M.gen(1)
    assert str(x) == "ℝ(0.303644…)"

    y = x + 2*x
    assert str(y) == "3*ℝ(0.303644…)"

    z = x*x + y;
    assert str(z) == "ℝ(0.303644…)^2 + 3*ℝ(0.303644…)"

    assert str(z.module()) == "ℤ-Module(1, ℝ(0.303644…), ℝ(0.303644…)^2)"

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))
