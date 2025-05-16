Tour of the SageMath Interface
==============================

We want to construct a module over the field of rational real numbers. Unlike in the C++ and pure Python case, we do not have to fix our generators, but just the base ring:

..
    We cannot use ``sage:`` syntax in the examples below because then the
    state, i.e., the variables, would not be carried over from one code block
    to the next and sage -t fails on this file. We could use a Jupyter notebook
    to work around but instead we just use byexample on this Python file
    instead. The only downside is that we have to write y*y instead of y^2
    below.

.. testcode::
    :hide:

    >>> from sage.all import *

.. code-block:: python

    >>> from pyexactreal import ExactReals  # random output due to deprecation warnings  # byexample: +pass
    >>> R = ExactReals(QQ)
    >>> R
    Real Numbers as (Rational Field)-Module

We pick two generators, one a random real in the interval (0, 1) and one a rational::

    >>> x = R(1)
    >>> y = R.random_element()

We can now perform some arithmetic in the module they generate::

    >>> 2*x
    2

    >>> y + x - y == x
    True

Note that we can also multiply elements. The result then typically lives in a larger submodule of the reals::

    >>> y * y
    ℝ(<...>)^2

Divisions are supported when the result can be determined exactly::

    >>> y * y / y == y
    True

Otherwise, only floor division is possible::

    >>> x // y >= 1
    True

Consult the :doc:`module reference <sage_exact_reals>` for further details.
