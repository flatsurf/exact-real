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
RealNumber = exactreal.RealNumber
Arf = exactreal.Arf
from cppyy.gbl import mpq_class, mpz_class

def test_random():
    r = RealNumber.random()
    s = RealNumber.random()

    assert r > 0 and r < 1
    assert r != s
    assert r == r
    assert r < s or s < r

    r = RealNumber.random(13.37)

    assert float(r) == 13.37
    assert str(r) == 'ℝ(13.37=3853640129148385635p-58 + ℝ(0.178808…)p-60)'
    assert float(r.arb(64)) == 13.37
    assert float(r.arf(64)) == 13.37

    r = RealNumber.random(Arf(-2), Arf(-1))
    s = RealNumber.random(Arf(1), Arf(2))

    assert r > -2 and r < -1
    assert s > 1 and s < 2

def test_rational():
    r = RealNumber.rational(1)

    assert float(r) == 1

    n = mpz_class(13)
    d = mpz_class(37)
    r = RealNumber.rational(mpq_class(n, d))
    s = RealNumber.rational(mpq_class(n, d))

    assert r == s

    assert float(r) == pytest.approx(13/37)

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))
