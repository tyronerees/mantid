.. algorithm::

.. summary::

.. relatedalgorithms::

.. properties::

Description
-----------

The algorithm LoadPSIMuonBin will read in a .bin file from the PSI
facility, from one of the various machines that use that format.
The file name can be absolute or relative path and should have the
extension .bin. The file should only be loaded if the first two bytes
are "1N" representing it as the format that this will load.

Errors
######

For errors we use the Poisson distribution.

Child Algorithms used
#####################

The ChildAlgorithms used by LoadPSIMuonBin are:

* :ref:`algm-AddSampleLog-v1` - It adds to the Sample Log of the workspace

.. categories::

.. sourcelink::
