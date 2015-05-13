#ifndef MANTID_MDALGORITHMS_ConvertMDHistoToMDEventWorkspace_H_
#define MANTID_MDALGORITHMS_ConvertMDHistoToMDEventWorkspace_H_

#include "MantidAPI/Algorithm.h"
#include "MantidDataObjects/MDEventWorkspace.h"
#include <deque>

namespace Mantid {
namespace MDAlgorithms {

/** ConvertMDHistoToMDEventWorkspace : Loads a file containing dimensionality and data for
  an MDEventWorkspace. Handles either full mdevents for mdleanevents as input
  data types.

  @date 2012-07-11

  Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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
class DLLExport ConvertMDHistoToMDEventWorkspace : public API::Algorithm {
public:
  ConvertMDHistoToMDEventWorkspace();
  virtual ~ConvertMDHistoToMDEventWorkspace();

  virtual const std::string name() const;
  /// Summary of algorithms purpose
  virtual const std::string summary() const {
    return "Reads an ASCII file containing MDEvent data and constructs an "
           "MDEventWorkspace.";
  }

  virtual int version() const;
  virtual const std::string category() const;

private:

  /// Actual number of dimensions specified
  size_t m_nDimensions;
  /// Actual number of md events provided.
  size_t m_nDataObjects;
  /// call back to add event data
  template <typename MDE, size_t nd>
  void addEventsData(typename DataObjects::MDEventWorkspace<MDE, nd>::sptr outWs);
  DataObjects::MDHistoWorkspace_sptr ws;

  void init();
  void exec();
};

} // namespace MDAlgorithms
} // namespace Mantid

#endif /* MANTID_MDALGORITHMS_ConvertMDHistoToMDEventWorkspace_H_ */
