#include "MantidKernel/StringContainsValidator.h"

#include <boost/python/class.hpp>
#include <boost/python/register_ptr_to_python.hpp>

using Mantid::Kernel::StringContainsValidator;
using Mantid::Kernel::IValidator;
using namespace boost::python;

void export_StringContainsValidator() {
  register_ptr_to_python<boost::shared_ptr<StringContainsValidator>>();

  class_<StringContainsValidator, bases<IValidator>, boost::noncopyable>(
      "StringContainsValidator");
}
