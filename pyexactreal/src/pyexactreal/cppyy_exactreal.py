#*********************************************************************
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
#*********************************************************************

import cppyy

# Importing cysignals after cppyy gives us proper stack traces on segfaults
# whereas cppyy otherwise only reports "segmentation violation" (which is
# probably what cling provides.)
import os
if os.environ.get('PYEXACTREAL_CYSIGNALS', True):
    try:
        import cysignals
    except ModuleNotFoundError:
        pass

def make_iterable(proxy, name):
    if hasattr(proxy, 'begin') and hasattr(proxy, 'end'):
        if not hasattr(proxy, '__iter__'):
            def iter(self):
                i = self.begin()
                while i != self.end():
                    yield i.__deref__()
                    i.__preinc__()

            proxy.__iter__ = iter

cppyy.py.add_pythonization(make_iterable, "exactreal")

class Expression:
    r"""
    An unbound Arb/Arf expression, emulating the yap expression trees that the
    C++ interface would provide.
    """
    def __init__(self, lhs, rhs, op):
        self.lhs = lhs
        self.rhs = rhs
        self.op = op
    def __repr__(self):
        return "(%s + %s)"%(self.lhs, self.rhs)
    def __call__(self, *args):
        return cppyy.gbl.exactreal.binary(self.lhs, self.rhs, self.op, *args)

def enable_arithmetic(proxy, name):
    elements = ["Element<exactreal::IntegerRing>", "Element<exactreal::RationalField>", "Element<exactreal::NumberField>"]
    with_precision = ["Arb", "Arf"]
    if name in with_precision:
        for (n, op) in [('add', ord('+')), ('sub', ord('-')), ('mul', ord('*')), ('div', ord('/'))]:
            def create_expression(lhs, rhs, op = op): return Expression(lhs, rhs, op)
            def inplace(lhs, *args, **kwargs): raise NotImplementedError("inplace operators are not supported")
            setattr(proxy, "__%s__"%n, create_expression)
            setattr(proxy, "__i%s__"%n, inplace)
    if name in elements:
        for (n, op) in [('add', ord('+')), ('sub', ord('-')), ('mul', ord('*')), ('div', ord('/'))]:
            def cppname(x):
                # some types such as int do not have a __cppname__; there might
                # be a better way to get their cppname but this seems to work
                # fine for the types we're using at least.
                return type(x).__cppname__ if hasattr(type(x), '__cppname__') else type(x).__name__
            def binary(lhs, rhs, op = op):
                return cppyy.gbl.exactreal.boost_binary[cppname(lhs), cppname(rhs), op](lhs, rhs)
            def inplace(lhs, *args, **kwargs): raise NotImplementedError("inplace operators are not supported yet")
            setattr(proxy, "__%s__"%n, binary)
            setattr(proxy, "__r%s__"%n, binary)
            setattr(proxy, "__i%s__"%n, inplace)
    if name in elements + with_precision:
        # strangely, cppyy does not hook up the unary operator-, so we do that here manually
        setattr(proxy, "__neg__", lambda self: cppyy.gbl.exactreal.minus(self))
    if name == "Arf":
        # cppyy has trouble with our nested enum in Arf (I could not reproduce
        # it so it is unclear what is the reason for this) so we provide the
        # enum values explicitly on the Arf instance.
        proxy.NEAR = 4
        proxy.DOWN = 0
        proxy.UP = 1
        proxy.FLOOR = 2
        proxy.CEIL = 3

cppyy.py.add_pythonization(enable_arithmetic, "exactreal")

def pretty_print(proxy, name):
    proxy.__repr__ = proxy.__str__

cppyy.py.add_pythonization(pretty_print, "exactreal")
cppyy.py.add_pythonization(pretty_print, "eantic")

for path in os.environ.get('PYEXACTREAL_INCLUDE','').split(':'):
    if path: cppyy.add_include_path(path)

cppyy.include("exact-real/cppyy.hpp")
cppyy.include("e-antic/renfxx.h")

from cppyy.gbl import exactreal

def makeModule(traits, gens, ring=None):
    vector = cppyy.gbl.std.vector['std::shared_ptr<const exactreal::RealNumber>']
    basis = vector(gens)
    print(exactreal.Module)
    make = exactreal.Module[traits].make
    if ring is None:
        ring = traits()
    return make(basis, ring)

NumberFieldTraits = exactreal.NumberField
exactreal.ZZModule = lambda *gens: makeModule(exactreal.IntegerRing, gens)
exactreal.QQModule = lambda *gens: makeModule(exactreal.RationalField, gens)
exactreal.NumberFieldModule = lambda field, *gens: makeModule(NumberFieldTraits, gens, field)

from pyeantic import eantic
exactreal.NumberField = eantic.renf
exactreal.NumberFieldElement = eantic.renf_elem

