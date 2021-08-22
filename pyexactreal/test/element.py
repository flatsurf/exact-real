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

def test_module():
    from pyexactreal import ZZModule, RealNumber

    M = ZZModule(RealNumber.rational(1), RealNumber.random())

    x = M.gen(1)
    # there used to be a segfault in calling module() on a temporary
    assert str((x*x).module()) == "ℤ-Module(1, ℝ(0.303644…), ℝ(0.303644…)^2)"

def test_multiplication():
    from pyexactreal import RealNumber, NumberFieldModule, NumberField

    # The following came up when doing arithmetic between renf_elem_class and
    # Element<NumberField> when trying to construct a hexagon with random side
    # lengths
    K = NumberField("x^2 - 3", "x", "1.73 +/- 0.1")
    x = K.gen()

    M = NumberFieldModule(K, RealNumber.rational(1), RealNumber.random(), RealNumber.random())
    one = M.gen(0)
    μ = M.gen(1)
    ν = M.gen(2)

    assert 2*one == one + one
    assert 0*one == one - one
    assert x*μ == μ*x
    assert 2*one + μ - ν - 2*one - μ + ν == 0
    assert 0*one + x*μ + x*ν - 0*one - x*ν - x*μ == 0

def test_serialization():
    from pyexactreal import RealNumber, ZZModule, QQModule, NumberFieldModule, NumberField
    M = ZZModule(RealNumber.rational(1))

    assert M.gen(0).to_json() == '{"parent": {"shared": 2147483649, "basis": [{"shared": 2147483650, "kind": "rational", "value": "1"}], "ring": {}}, "coefficients": ["1"]}'

    M = QQModule(RealNumber.rational(1))

    assert M.gen(0).to_json() == '{"parent": {"shared": 2147483649, "basis": [{"shared": 2147483650, "kind": "rational", "value": "1"}], "ring": {}}, "coefficients": ["1"]}'

    K = NumberField("x^2 - 3", "x", "1.73 +/- 0.1")
    M = NumberFieldModule(K, RealNumber.rational(1))

    assert M.gen(0).to_json() == '{"parent": {"shared": 2147483649, "basis": [{"shared": 2147483650, "kind": "rational", "value": "1"}], "ring": {"field": {"id": 2147483651, "name": "x", "embedding": "[1.7320508075688772935274463415058723669 +/- 5.08e-38]", "minpoly": "x^2 - 3", "precision": 64}}}, "coefficients": [{"parent": {"id": 2147483652, "name": "a", "embedding": "[+/- 2.01]", "minpoly": "a-1", "precision": 64}, "value": "1"}]}'


if __name__ == '__main__': sys.exit(pytest.main(sys.argv))

