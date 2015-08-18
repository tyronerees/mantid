#include "MantidKernel/StringContainsValidator.h"
#include "MantidPythonInterface/kernel/IsNone.h"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/default_call_policies.hpp>
#include <boost/python/overloads.hpp>

using Mantid::Kernel::StringContainsValidator;
using Mantid::Kernel::IValidator;
using namespace boost::python;

/// A macro for generating exports for each type
#define EXPORT_STRINGCONTAINSVALIDATOR(ElementType, prefix)                    \
  class_<StringContains<ElementType>, bases<IValidator>, boost::noncopyable>(  \
      #prefix "StringContains")
}

void export_StringContainsValidator() {}
