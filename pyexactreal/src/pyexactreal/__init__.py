r"""
Python and SageMath wrappers for libexactreal

EXAMPLES::

    >>> from pyexactreal import RealNumber
    >>> RealNumber.random()
    ℝ(0...)

"""
# ********************************************************************
#  This file is part of exact-real.
#
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
# ********************************************************************

from .cppyy_exactreal import exactreal, ZZModule, QQModule, NumberFieldModule

import pyeantic
NumberField = pyeantic.eantic.renf
RealNumber = exactreal.RealNumber

def ExactReals(base=None, *args, **kwargs):
    r"""
    The Real Numbers as a module over the real embedded number field ``base``.

    This parent wraps the finitely generated modules provided by the
    exact-real package for SageMath. Elements in this parent are typically
    linear combinations of some particular real numbers such as transcendental
    random reals.

    EXAMPLES::

        sage: from pyexactreal import ExactReals
        sage: K.<a> = NumberField(x^2 - 2, embedding=AA(sqrt(2)))
        sage: R = ExactReals(K)
        sage: o = R.rational(1); o
        1

    A random real number, i.e., a number whose digits are provided by a
    (pseudo) random number generator. Due to the periodicity of the random
    number generator, this is technically not a transcendental number. However,
    it can be used to model one for all practical purposes::

        sage: g = R.random_element(); g
        ℝ(0.178808…)
        sage: a * g
        (a ~ 1.414214)*ℝ(0.178808…)
        sage: g + o
        ℝ(0.178808…) + 1
        sage: a + g
        ℝ(0.178808…) + (a ~ 1.414214)

    Internally, elements live in different modules, generated by the generators
    their built from. However, you won't see any of that underlying machinery
    as these modules get constructed automatically as required::

        sage: g*g + g + a
        ℝ(0.178808…)^2 + ℝ(0.178808…) + (a ~ 1.414214)

    Basic arithmetic is supported, though some operations, such as divisions,
    only work in special cases::

        sage: g / 10
        (1/10 ~ 0.100000)*ℝ(0.178808…)
        sage: g / g
        1
        sage: R.random_element() / R.random_element()
        Traceback (most recent call last):
        ...
        pyexactreal.cppyy_exactreal.NotRepresentableError: ...

    Comparisons should work without limitations. The underlying exact-real
    compares ball-arithmetic representation, going to higher precisions as
    necessary. Due to the nature of the numbers that are supported, this
    process is always guaranteed to terminate::

        sage: g > g
        False
        sage: g < 1
        True
        sage: g*g < g
        True
        sage: g + o == o + g
        True

    """
    from .exact_reals import ExactReals as ER
    return ER(base, *args, **kwargs)
