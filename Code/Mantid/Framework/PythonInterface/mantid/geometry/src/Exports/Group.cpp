
#include "MantidGeometry/Crystal/Group.h"

#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/list.hpp>

using Mantid::Geometry::Group;
using Mantid::Geometry::SymmetryOperation;

using namespace boost::python;

namespace {
    std::vector<std::string> getSymmetryOperationStrings(Group &self) {
      const std::vector<SymmetryOperation> &symOps = self.getSymmetryOperations();

      std::vector<std::string> pythonSymOps;
      for (auto it = symOps.begin(); it != symOps.end(); ++it) {
        pythonSymOps.push_back((*it).identifier());
      }

      return pythonSymOps;
    }

    boost::python::list getSymmetryOperations(Group &self) {
      const std::vector<SymmetryOperation> &symOps = self.getSymmetryOperations();

      boost::python::list pythonSymOps;
      for (auto it = symOps.begin(); it != symOps.end(); ++it) {
        pythonSymOps.append(*it);
      }

      return pythonSymOps;
    }
}

// clang-format off
void export_Group()
// clang-format on
{
  enum_<Group::CoordinateSystem>("CoordinateSystem")
      .value("Orthogonal", Group::Orthogonal)
      .value("Hexagonal", Group::Hexagonal);

  class_<Group, boost::noncopyable>("Group", no_init)
      .def("getOrder", &Group::order, "Returns the order of the group.")
      .def("getCoordinateSystem", &Group::getCoordinateSystem, "Returns the type of coordinate system to distinguish groups with hexagonal system definition.")
      .def("getSymmetryOperations", &getSymmetryOperations, "Returns the symmetry operations contained in the group.")
      .def("getSymmetryOperationStrings", &getSymmetryOperationStrings, "Returns the x,y,z-strings for the contained symmetry operations.");
}
