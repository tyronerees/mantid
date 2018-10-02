#ifndef MANTID_WORKFLOWALGORITHMS_MUONGROUPASYMMETRYCALCULATOR_H_
#define MANTID_WORKFLOWALGORITHMS_MUONGROUPASYMMETRYCALCULATOR_H_

#include "MantidWorkflowAlgorithms/MuonGroupCalculator.h"

namespace Mantid {
namespace WorkflowAlgorithms {

/** MuonGroupAsymmetryCalculator : Calculates asymmetry between given group
  (specified via GroupIndex) and Muon exponential decay

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
class DLLExport MuonGroupAsymmetryCalculator : public MuonGroupCalculator {
public:
  MuonGroupAsymmetryCalculator(const API::WorkspaceGroup_sptr inputWS,
                               const std::vector<int> summedPeriods,
                               const std::vector<int> subtractedPeriods,
                               const int groupIndex, const double start = 0.0,
                               const double end = 30.0,
                               const std::string wsName = "");
  /// Performs group asymmetry calculation
  API::MatrixWorkspace_sptr calculate() const override;

private:
  /// Removes exponential decay from the workspace
  API::MatrixWorkspace_sptr removeExpDecay(const API::Workspace_sptr &inputWS,
                                           const int index) const;
  API::MatrixWorkspace_sptr
  estimateAsymmetry(const API::Workspace_sptr &inputWS, const int index) const;
};
double getStoredNorm();
} // namespace WorkflowAlgorithms
} // namespace Mantid

#endif /* MANTID_WORKFLOWALGORITHMS_MUONGROUPASYMMETRYCALCULATOR_H_ */
