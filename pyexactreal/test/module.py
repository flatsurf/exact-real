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

from pyexactreal import exactreal, ZZModule, QQModule, NumberFieldModule, NumberField
RealNumber = exactreal.RealNumber

def test_ZZ_module():
    trivial = ZZModule()
    ZZ = ZZModule(RealNumber.rational(1))
    M = ZZModule(RealNumber.rational(1), RealNumber.random())

    assert str(trivial) == "ℤ-Module()"
    assert trivial.rank() == 0
    assert ZZ.rank() == 1
    assert M.rank() == 2

def test_QQ_module():
    trivial = QQModule()
    QQ = QQModule(RealNumber.rational(1))
    M = QQModule(RealNumber.rational(1), RealNumber.random())

    assert str(trivial) == "ℚ-Module()"
    assert trivial.rank() == 0
    assert QQ.rank() == 1
    assert M.rank() == 2

def test_NF_module():
    K = NumberField("a^2 - 2", "a", "1.414 +/- .1")
    trivial = NumberFieldModule(K)
    NF = NumberFieldModule(K, RealNumber.rational(1))
    M = NumberFieldModule(K, RealNumber.rational(1), RealNumber.random())

    assert str(trivial) == "K-Module()"
    assert trivial.rank() == 0
    assert NF.rank() == 1
    assert M.rank() == 2

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))
