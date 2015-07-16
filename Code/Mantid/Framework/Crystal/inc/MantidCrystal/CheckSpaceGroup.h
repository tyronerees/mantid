#ifndef MANTID_CRYSTAL_CHECKSPACEGROUP_H_
#define MANTID_CRYSTAL_CHECKSPACEGROUP_H_

#include "MantidKernel/System.h"
#include "MantidAPI/Algorithm.h"
#include "MantidGeometry/Crystal/IPeak.h"
#include "MantidGeometry/Crystal/SpaceGroup.h"

namespace Mantid {
namespace Crystal {

/**
  @class CheckSpaceGroup

  This algorithm checks whether the reflections in a PeaksWorkspace are
  compatible with a given space group. This is done by determining whether a
  peak is experimentally observed or not using the following criterion:

  \f[
    I > n \cdot \sigma(I)
  \f]

  Here, n is a factor that can be specified by the user and defaults to 3. After
  this determination, the algorithm checks whether the reflection is allowed
  according to space group symmetry.

  If the reflection is observed, but not allowed by space group symmetry, it's a
  "systematic absence violation". If, inversly, the reflection is allowed but
  not observed, it's an "additional absence", which may indicate the presence
  of an atom on a position in the unit cell with high symmetry.

  By default, the algorithm counts the number of absence violations and
  additional absences and returns them in their respective output properties.
  Optionally, the peaks in question can be made available in their own
  PeaksWorkspaces so that they may be inspected more closely.

    @author Michael Wedel
    @date 14/07/2015

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

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport CheckSpaceGroup : public API::Algorithm {
public:
  CheckSpaceGroup();
  ~CheckSpaceGroup() {}

  const std::string name() const { return "CheckSpaceGroup"; }
  const std::string summary() const {
    return "Check if the peaks are compatible with reflection conditions of a "
           "space group.";
  }

  int version() const { return 1; }

  const std::string category() const { return "Crystal"; }

private:
  bool isPeakObserved(const Geometry::IPeak &peak, double obsThreshold) const;
  bool isPeakAllowed(const Geometry::IPeak &peak,
                     const Geometry::SpaceGroup_const_sptr &spaceGroup) const;

  void init();
  void exec();
};

} // namespace Crystal
} // namespace Mantid

#endif /* MANTID_CRYSTAL_CHECKSPACEGROUP_H_ */
