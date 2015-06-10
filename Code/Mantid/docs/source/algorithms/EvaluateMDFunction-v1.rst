<<<<<<< HEAD

=======
>>>>>>> origin/master
.. algorithm::

.. summary::

.. alias::

.. properties::

Description
-----------

<<<<<<< HEAD
TODO: Enter a full rst-markup description of your algorithm here. 
=======
The algorithm will use the axes of the input workspace to evaluate a functions on them 
and store the result in the output workspace.

This is a plot of the evaluated function from the usage example below.

.. figure:: /images/Function3D.png
   :alt: Function3D.png
>>>>>>> origin/master


Usage
-----
<<<<<<< HEAD
..  Try not to use files in your examples, 
    but if you cannot avoid it then the (small) files must be added to 
    autotestdata\UsageData and the following tag unindented
    .. include:: ../usagedata-note.txt

**Example - EvaluateMDFunction**

.. testcode:: EvaluateMDFunctionExample

   # Create a host workspace
   ws = CreateWorkspace(DataX=range(0,3), DataY=(0,2))
   or
   ws = CreateSampleWorkspace()

   wsOut = EvaluateMDFunction()

   # Print the result
   print "The output workspace has %i spectra" % wsOut.getNumberHistograms()

Output:

.. testoutput:: EvaluateMDFunctionExample 

  The output workspace has ?? spectra

.. categories::

=======

.. testcode::

    # Create an empty 3D histo workspace.
    n = 50 * 50 * 50
    ws=CreateMDHistoWorkspace(Dimensionality=3, Extents='-1,1,-1,1, -1,1',\
        SignalInput = [0.0] * n, ErrorInput = [1.0] * n,\
        NumberOfBins='50,50,50',Names='Dim1,Dim2,Dim3',Units='MomentumTransfer,MomentumTransfer,MomentumTransfer')

    # Define a function
    function = 'name=UserFunctionMD,Formula=1.0/(1.0 + 100*(0.5 - x^2 - y^2 -z^2)^2)'

    # Evaluate the function on the created workspace
    out = EvaluateMDFunction(ws,function)

    # Check the result workspace
    print out.getNumDims()
    print out.getXDimension().getName()
    print out.getYDimension().getName()
    print out.getZDimension().getName()
    
    
Output
######

.. testoutput::

  3
  Dim1
  Dim2
  Dim3

.. categories::
>>>>>>> origin/master
