.. _Plugin:

Plugin
======

What is it?
-----------

Mantid is designed to be extensible by anyone who can write a simple
library in C++. There are several areas that have been built so that new
version and instances can be added just by copying a library with the
new code into the plugin directory before starting mantid. This
eliminates the need to recompile the main Mantid code or any user
interfaces derived from it when you want to extend it by, for example,
adding a new algorithm.

What is a plugin?
-----------------

A plugin is a library of one or more classes that include the
functionality that you need. Within the outputs of the Mantid project
Several of the libraries we deliver are created as plugins. Examples
are:

-  MantidAlgorithms - Contains the general :ref:`algorithms <Algorithm>`
-  MantidDataHandling - Contains the basic data loading and saving
   :ref:`algorithms <Algorithm>`
-  MantidNexus - Contains the :ref:`algorithms <Algorithm>` for handling
   nexus files
-  MantidDataObjects - Contains the definitions of the standard
   :ref:`workspaces <Workspace>`
   
Any library that includes classes that extend one of the defined interfaces, and the correct declaration
will be loaded at runtime  if located in the Plugins directory.

How can you extend Mantid?
--------------------------

The following areas have been designed to be easily extensible through
using plugins. Those with links contain more details in case you wish to create
one of your own.

Algorithms

-  :ref:`Algorithm <Algorithm>`
-  USER_ALGs
-  Unit

GUI extensions

-  Custom Algorithm Dialogs
-  DECLARE_SUBWINDOW

Fitting / Function Optimization

-  Fitting functions
-  Fitting constraints
-  Fitting function minimizers
-  Fitting Cost Functions
-  DECLARE_DOMAINCREATOR
-  DECLARE_IMPLICIT_FUNCTION_PARAMETER_PARSER
-  DECLARE_IMPLICIT_FUNCTION_PARSER
-  DECLARE_TRANSFORMSCALE

Workspaces & workspace extensions

-  :ref:`Workspace <Workspace>`
-  Instrument Parameter Types
-  Tableworkspace columns
-  DECLARE_VECTORCOLUMN (related to above?)
-  DECLARE_SINGLE_VALUE_PARAMETER
-  DECLARE_VECTOR_PARAMETER
-  DECLARE_WIDGET_PLUGIN

Facility extensions

-  Live listener implementations
-  Script Repository implementations
-  Metadata Catalog implementations (such as ICAT)

Unknown 

-  DECLARE_BRAGGSCATTERER
-  DECLARE_POINTGROUP
-  DECLARE_GENERATED_SPACE_GROUP
-  DECLARE_TABULATED_SPACE_GROUP
-  DECLARE_SYMMETRY_OPERATION
-  DECLARE_TEST_VALIDATOR
-  DECLARE_FOREGROUNDMODEL
-  DECLARE_MDRESOLUTIONCONVOLUTION
-  DECLARE_3D_VECTOR_PARAMETER
-  DECLARE_MD_TRANSF
-  DECLARE_MD_TRANSFID



  MDResolutionConvolution.h(139):#define DECLARE_MDRESOLUTIONCONVOLUTION(classname, alias)
  Vector3DParameter.h(151):#define DECLARE_3D_VECTOR_PARAMETER(classname, type_)
  MDTransfFactory.h(18):#define DECLARE_MD_TRANSF(classname)
  MDTransfFactory.h(29):#define DECLARE_MD_TRANSFID(classname, regID)
  AlgorithmDialog.h(10):#define DECLARE_DIALOG(classname)
  UserSubWindow.h(10):#define DECLARE_SUBWINDOW(classname)
  CatalogFactory.h(13):#define DECLARE_CATALOG(classname)
  ConstraintFactory.h(94):#define DECLARE_CONSTRAINT(classname)
  DeclareUserAlg.h(4):#DECLARE_FOREGROUNDMODELdefine DECLARE_USER_ALG(x)
  FuncMinimizerFactory.h(81):#define DECLARE_FUNCMINIMIZER(classname, username)
  FunctionFactory.h(171):#define DECLARE_FUNCTION(classname)
  IArchiveSearch.h(16):#define DECLARE_ARCHIVESEARCH(classname, facility) 
  ICostFunction.h(82):#define DECLARE_COSTFUNCTION(classname, username)
  IDomainCreator.h(161):#define DECLARE_DOMAINCREATOR(classname)
  ImplicitFunctionParameterParser.h(10):#define DECLARE_IMPLICIT_FUNCTION_PARAMETER_PARSER(classname)
  ImplicitFunctionParser.h(10):#define DECLARE_IMPLICIT_FUNCTION_PARSER(classname)
  ITransformScale.h(61):#define DECLARE_TRANSFORMSCALE(classname)
  LiveListenerFactory.h(12):#define DECLARE_LISTENER(classname)
  ScriptRepositoryFactory.h(87):#define DECLARE_SCRIPTREPOSITORY(classname)
  SingleValueParameter.h(158):#define DECLARE_SINGLE_VALUE_PARAMETER(classname, type_)
  VectorParameter.h(245):#define DECLARE_VECTOR_PARAMETER(classname, type_)
  WorkspaceFactory.h(9):#define DECLARE_WORKSPACE(classname)
  TableColumn.h(369):#define DECLARE_TABLECOLUMN(DataType, TypeName)
  VectorColumn.h(171):#define DECLARE_VECTORCOLUMN(Type, TypeName)
  PluginCollectionInterface.h(74):#define DECLARE_WIDGET_PLUGIN(PluginClass, WidgetClass, ToolTip)
  BraggScattererFactory.h(99):#define DECLARE_BRAGGSCATTERER(classname)
  Instrument\Parameter.h(7):#define DECLARE_PARAMETER(classname, classtype)
  Crystal\PointGroupFactory.h(109):#define DECLARE_POINTGROUP(classname)
  Crystal\SpaceGroupFactory.h(209):#define DECLARE_GENERATED_SPACE_GROUP(number, hmSymbol, generators)
  Crystal\SpaceGroupFactory.h(218):#define DECLARE_TABULATED_SPACE_GROUP(number, hmSymbol, symmetryOperations)
  Crystal\SymmetryOperationFactory.h(94):#define DECLARE_SYMMETRY_OPERATION(operation, name)
  UnitFactory.h(12):#define DECLARE_UNIT(classname)
  TypedValidatorTest.h(10):#define DECLARE_TEST_VALIDATOR(ClassName, HeldType)
  ForegroundModel.h(145):#define DECLARE_FOREGROUNDMODEL(classname)
  MDResolutionConvolution.h(139):#define DECLARE_MDRESOLUTIONCONVOLUTION(classname, alias)
  Vector3DParameter.h(151):#define DECLARE_3D_VECTOR_PARAMETER(classname, type_)
  MDTransfFactory.h(18):#define DECLARE_MD_TRANSF(classname)
  MDTransfFactory.h(29):#define DECLARE_MD_TRANSFID(classname, regID)
  AlgorithmDialog.h(10):#define DECLARE_DIALOG(classname)
  UserSubWindow.h(10):#define DECLARE_SUBWINDOW(classname)


How do you create a plugin?
---------------------------

There is nothing special about the library you build in order for it to
be used as a plugin, as long as it contains one or more algorithms,
workspaces or units (they can be mixed) they will automatically be
registered and available for use.

How does it work?
-----------------

Each of the extensible units within Mantid shares a base class that all
further objects of that type inherit from. For example all algorithms
must inherit from the Algorithm base class. This allows all uses of
those objects to work through the interface of the base class, and the
user (or other code) does not need to know what the algorithm actually
is, just that it is an algorithm.

In addition each of the extensible units has a macro that adds some code
that automatically registers the class with the appropriate :ref:`dynamic
factory <Dynamic Factory>`. This code executes immediately when the
library is loaded and is what makes you new objects available for use.
All of these macros start DECLARE and, for example, the one for
algorithms is:

-  ``DECLARE_ALGORITHM(classname)`` (or ``namespace::classname`` if the
   declaration is not enclosed in the algorithm's namespace)



.. categories:: Concepts