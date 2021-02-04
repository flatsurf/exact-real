r"""
Python wrappers for exact-real powered by cppyy.

Note that pyexactreal offers a better interface to work with exact-real. Use
`pyexactreal.exactreal` if you need to directly manipulate the underlying C++
objects::

>>> from pyexactreal import exactreal
>>> exactreal.Module[exactreal.IntegerRing].make([])
ℤ-Module()

"""
# ********************************************************************
#  This file is part of exact-real.
#
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
# ********************************************************************

import cppyy
import gmpxxyy

from cppyythonizations.pickling.cereal import enable_cereal
from cppyythonizations.printing import enable_pretty_printing

class NotRepresentableError(ArithmeticError):
    r"""
    Raised when the result of an operation can not be written in this ring.

    EXAMPLES::

        >>> from pyexactreal import ZZModule, RealNumber
        >>> M = ZZModule(RealNumber.random(), RealNumber.random())
        >>> M.gen(0) / M.gen(1)
        Traceback (most recent call last):
        ...
        pyexactreal.cppyy_exactreal.NotRepresentableError: result is not representable in this parent

    """

class PrecisionError(ArithmeticError):
    r"""
    Raised when the result of an operation can not be decided in ball arithmetic.

    EXAMPLES::

        >>> from pyexactreal import exactreal
        >>> import cppyy
        >>> a = exactreal.Arb(cppyy.gbl.mpq_class(1, 3), 64); a
        [0.333333 +/- 3.34e-7]
        >>> a == 1
        False
        >>> a == a
        Traceback (most recent call last):
        ...
        pyexactreal.cppyy_exactreal.PrecisionError: ...

    """

# Importing cysignals after cppyy gives us proper stack traces on segfaults
# whereas cppyy otherwise only reports "segmentation violation" (which is
# probably what cling provides.)
import os
if os.environ.get('PYEXACTREAL_CYSIGNALS', True):
    try:
        import cysignals
    except ModuleNotFoundError:
        pass

def enable_arithmetic(proxy, name):
    r"""
    Help cppyy discover arithmetic provided by boost::operators.

    TESTS::

        >>> from pyexactreal import ZZModule, RealNumber
        >>> M = ZZModule(RealNumber.rational(1))
        >>> x = M.gen(0)
        >>> x + x
        2
        >>> x - x
        0
        >>> x * x
        1
        >>> x / x
        1

    """
    for op in ['add', 'sub', 'mul', 'truediv']:
        python_op = "__%s__" % (op,)
        python_rop = "__r%s__" % (op,)

        def unwrap_binary_optional(x):
            if not hasattr(x, 'has_value'):
                return x
            if not x.has_value():
                # e.g., when a division failed because x/y does not live in the coefficient ring
                raise NotRepresentableError("result is not representable in this parent")
            return x.value()

        implementation = getattr(cppyy.gbl.exactreal.cppyy, op)
        def binary(lhs, rhs, implementation=implementation):
            return unwrap_binary_optional(implementation[type(lhs), type(rhs)](lhs, rhs))
        def rbinary(rhs, lhs, implementation=implementation):
            return unwrap_binary_optional(implementation[type(lhs), type(rhs)](lhs, rhs))

        setattr(proxy, python_op, binary)
        setattr(proxy, python_rop, rbinary)

    setattr(proxy, "__neg__", lambda self: cppyy.gbl.exactreal.cppyy.neg(self))

arithmetic_predicate = lambda proxy, name: name in ["Arb", "Arf"] or \
        name.startswith("Element<") or \
        name.startswith("ArbExpr<") or \
        name.startswith("ArfExpr<")

def add_pythonization(pythonization, namespace, predicate=lambda *args: True):
    r"""
    Conditionally enable a pythonization on a namespace.
    """
    cppyy.py.add_pythonization(lambda proxy, name: predicate(proxy, name) and pythonization(proxy, name), namespace)

add_pythonization(enable_arithmetic, "exactreal", arithmetic_predicate)
add_pythonization(enable_arithmetic, "exactreal::yap", arithmetic_predicate)

def enable_yap(proxy, name):
    r"""
    Make yap templating machinery accessible from cppyy.

    TESTS::

        >>> from pyexactreal import exactreal
        >>> a = exactreal.Arb(1)
        >>> b = a + a; b
        expr<+>
            term<exactreal::Arb ... &>[=1.00000]
            term<exactreal::Arb ... &>[=1.00000]
        >>> b(64)
        2.00000
        >>> b += b
        >>> b += a
        >>> b(64)
        5.00000
        >>> a += b
        >>> a(64)
        6.00000

    """
    yap = ["Arb", "Arf"]

    if name in yap:
        for op in ['add', 'sub', 'mul', 'truediv']:
            python_op = "__%s__" % (op,)
            python_rop = "__r%s__" % (op,)

            implementation = getattr(proxy, python_op)
            def binary(lhs, rhs, implementation=implementation):
                return Yap.wrap(lhs)._op(implementation, rhs)
            def rbinary(rhs, lhs, implementation=implementation):
                return Yap.wrap(lhs)._op(implementation, rhs)

            setattr(proxy, python_op, binary)
            setattr(proxy, python_rop, rbinary)

cppyy.py.add_pythonization(enable_yap, "exactreal")

def enable_optional(proxy, name):
    if name in  ["Arb"] or name.startswith("ArbExpr<"):
        def unwrap_logical_optional(x):
            if not hasattr(x, 'has_value'):
                return x
            if not x.has_value():
                raise PrecisionError("precision insufficient to decide this relation or not well defined for arguments")
            return x.value()

        for op in ["lt", "le", "eq", "ne", "gt", "ge"]:
            python_op = "__%s__"%(op,)
            hidden_op = "_cppyy_%s"%(op,)
            setattr(proxy, hidden_op, getattr(proxy, python_op))
            def relation(lhs, rhs, op=hidden_op):
                return unwrap_logical_optional(getattr(lhs, op)(rhs))
            setattr(proxy, python_op, relation)

cppyy.py.add_pythonization(enable_optional, "exactreal")
cppyy.py.add_pythonization(enable_optional, "exactreal::yap")

cppyy.py.add_pythonization(enable_pretty_printing, "exactreal")
cppyy.py.add_pythonization(enable_pretty_printing, "eantic")

cppyy.py.add_pythonization(lambda proxy, name: enable_cereal(proxy, name, ["exact-real/cereal.hpp"]), "exactreal")

# Set EXTRA_CLING_ARGS="-I /usr/include" or wherever exact-real/cppyy.hpp can
# be resolved if the following line fails to find the header file.
cppyy.add_include_path(os.path.join(os.path.dirname(__file__), 'include'))

cppyy.include("exact-real/cppyy.hpp")
cppyy.include("e-antic/renfxx.h")

from cppyy.gbl import exactreal

class Yap(object):
    r"""
    Wrap a boost::yap expression with added lifelines to make them compatible
    with Python memory management.

    TESTS::

        >>> from pyexactreal import exactreal
        >>> a = exactreal.Arb(1)
        >>> a += a
        >>> a += a
        >>> a += a
        >>> a += a
        >>> a
        expr<+>
            expr<+> const &
                expr<+> const &
                    expr<+> const &
                    ...
        >>> a(64)
        16.0000

    """
    def __init__(self, value, lifelines=[]):
        self.value = value
        self._lifelines = lifelines

    @classmethod
    def wrap(cls, operand):
        if not isinstance(operand, Yap):
            return Yap(operand)
        return operand

    def _op(self, op, *rhs):
        r"""
        TESTS:

        There seems to be a bug in cppyy as of mid-2019 where the following
        addition would collect the ``a``. That's why we introduced the (likely
        excessive) amount of lifelines. It should be enough to hold a lifeline
        to ``self`` and ``rhs`` (so we recursively hold their values and all
        their lifelines.) But if we do not hold the operands explicitly, they
        get collected during the addition::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> b(10)
            2.00000

        """
        operands = [self.value] + [Yap.wrap(x).value for x in rhs]
        lifelines = [operands, self, rhs]
        ret = Yap(op(*operands), lifelines)
        return ret

    def __add__(self, rhs):
        r"""
        Addition for Yap expressions

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> (b + a)(64) == (a + b)(64)
            True
            >>> (b + b)(64)
            4.00000
            
        """
        return self._op(lambda x, y: x + y, rhs)

    def __sub__(self, rhs):
        r"""
        Multiplication for Yap expressions

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> (b - a)(64) != (a - b)(64)
            True
            >>> (b - b)(64)
            0
            
        """
        return self._op(lambda x, y: x - y, rhs)

    def __mul__(self, rhs):
        r"""
        Multiplication for Yap expressions

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> (b * a)(64) == (a * b)(64)
            True
            >>> (b * b)(64)
            4.00000
            
        """
        return self._op(lambda x, y: x * y, rhs)

    def __truediv__(self, rhs):
        r"""
        Division for Yap expressions

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> (b / a)(64) != (a / b)(64)
            True
            >>> (b / b)(64)
            1.00000
            
        """
        return self._op(lambda x, y: x / y, rhs)

    def __neg__(self):
        r"""
        Negatives of Yap expressions

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> b = a + a
            >>> (-b)(64)
            -2.00000
        
        """
        return self._op(lambda x: -x)

    def __call__(self, *args):
        r"""
        Evaluate this Yap expression.

        EXAMPLES::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> (a + a)(64)
            2.00000

        """
        return cppyy.gbl.exactreal.cppyy.eval[type(self.value)](self.value, *args)

    def __repr__(self):
        r"""
        Return a printable representation of this Yap expression.

        EXAMPLES:

        We use Yap's own debug printer. It's quite verbose::

            >>> from pyexactreal import exactreal
            >>> a = exactreal.Arb(1)
            >>> a + a
            expr<+>
                term<exactreal::Arb ... &>[=1.00000]
                term<exactreal::Arb ... &>[=1.00000]

        """
        import cppyy
        cppyy.include("boost/yap/print.hpp")
        os = cppyy.gbl.std.stringstream()
        try:
            cppyy.gbl.boost.yap.print(os, self.value)
        except:
            return repr(self.value)
        return os.str().strip()

def makeModule(traits, gens, ring=None):
    r"""
    Helper to create a module over a ``traits`` ``ring`` generated by ``gens``.

    EXAMPLES::

        >>> from pyexactreal import exactreal, RealNumber
        >>> import pyexactreal.cppyy_exactreal
        >>> pyexactreal.cppyy_exactreal.makeModule(exactreal.IntegerRing, [RealNumber.random()], ring=None)
        ℤ-Module(ℝ(...))

    """
    vector = cppyy.gbl.std.vector['std::shared_ptr<const exactreal::RealNumber>']
    basis = vector(gens)
    make = exactreal.Module[traits].make
    if ring is None:
        ring = traits()
    return make(basis, ring)

def ZZModule(*gens):
    r"""
    Return the `\Z`-module generated by ``gens``.

    EXAMPLES::

        >>> from pyexactreal import ZZModule, RealNumber
        >>> ZZModule(RealNumber.rational(1), RealNumber.random())
        ℤ-Module(1, ℝ(...))

    """
    return makeModule(exactreal.IntegerRing, gens)

def QQModule(*gens):
    r"""
    Return the `\Q`-module generated by ``gens``.

    EXAMPLES::

        >>> from pyexactreal import ZZModule, RealNumber
        >>> QQModule(RealNumber.rational(1), RealNumber.random())
        ℚ-Module(1, ℝ(...))

    """
    return makeModule(exactreal.RationalField, gens)

def NumberFieldModule(K, *gens):
    r"""
    Return the ``K``-module generated by ``gens``.

    EXAMPLES::

        >>> from pyexactreal import ZZModule, RealNumber, NumberField
        >>> K = NumberField("x^2 - 2", "x", "1.4 +/- 1")
        >>> NumberFieldModule(K, RealNumber.rational(1), RealNumber.random())
        K-Module(1, ℝ(...))

    """
    return makeModule(exactreal.NumberField, gens, K)
