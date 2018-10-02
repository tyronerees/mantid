=======================
Python Coding Standards
=======================

.. contents:: Contents
   :local:

Style
^^^^^

- Unless otherwise specified, follow `PEP 8
  <https://www.python.org/dev/peps/pep-0008>`_; this means using
  `snake_case <https://en.wikipedia.org/wiki/Snake_case>`_
- Use `flake8 <http://flake8.pycqa.org/en/latest>`_ to check
  for problems in this area. Remember that PEP 8 is only a guide, so
  respect the style of the surrounding code as a primary goal
- Always use four spaces for indentation
- For docstrings please follow `Docstring Conventions PEP 257
  <https://www.python.org/dev/peps/pep-0257>`_ and document code to
  aid `sphinx
  <https://pythonhosted.org/an_example_pypi_project/sphinx.html#full-code-example>`_

None checks
-----------  

Prefer ``if obj is not None:`` over ``if obj:``. The latter invokes
``object.__nonzero__`` whereas the former simply compares that obj
references the same object as ``None``.

Imports
-------

Imports should be grouped in the following order:

1. stdlib
2. third party libraries
3. local modules

Each group should be alphabetically sorted and separate by a newline, e.g.

.. code-block:: python

    import sys

    from qtpy.QtWidgets import QMainWindow

    from mypackage.subpkg import MyClass

Python/Qt
^^^^^^^^^

Use the `qtpy <https://pypi.python.org/pypi/QtPy>`_ module to hide the
differences between PyQt versions.  When working with signals, use the
new style. For naming new custom signals, use the ``sig_`` prefix:

.. code-block:: python

    from qtpy.QtCore import Signal
    ...

    class MyWidget(...):
        """Funky new widget"""    

        # Signals
        sig_run_a_thing_happened = Signal(str, str, str, bool, bool)
