#ifndef MANTID_API_WORKSPACEGROUP_FWD_H_
#define MANTID_API_WORKSPACEGROUP_FWD_H_

#include <boost/shared_ptr.hpp>
#include <memory>

namespace Mantid {
namespace API {
/**
  This file provides forward declarations for Mantid::API::WorkspaceGroup

  Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>.
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/

/// forward declare of Mantid::API::WorkspaceGroup
class WorkspaceGroup;
/// shared pointer to Mantid::API::WorkspaceGroup
using WorkspaceGroup_sptr = boost::shared_ptr<WorkspaceGroup>;
/// shared pointer to Mantid::API::WorkspaceGroup, pointer to const version
using WorkspaceGroup_const_sptr = boost::shared_ptr<const WorkspaceGroup>;
/// unique pointer to Mantid::API::WorkspaceGroup
using WorkspaceGroup_uptr = std::unique_ptr<WorkspaceGroup>;
/// unique pointer to Mantid::API::WorkspaceGroup (const version)
using WorkspaceGroup_const_uptr = std::unique_ptr<const WorkspaceGroup>;

} // namespace API
} // namespace Mantid

#endif // MANTID_API_WORKSPACEGROUP_FWD_H_
