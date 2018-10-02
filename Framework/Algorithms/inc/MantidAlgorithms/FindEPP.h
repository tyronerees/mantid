#ifndef MANTID_ALGORITHMS_FINDEPP_H_
#define MANTID_ALGORITHMS_FINDEPP_H_

#include "MantidAPI/Algorithm.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAlgorithms/DllConfig.h"

namespace Mantid {
namespace Algorithms {

/** Performs Gaussian fits over each spectrum to find the Elastic Peak
 Position (EPP).

  Copyright &copy; 2017 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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
class MANTID_ALGORITHMS_DLL FindEPP : public API::Algorithm {
public:
  const std::string name() const override;
  int version() const override;
  const std::string category() const override;
  const std::string summary() const override;

private:
  void init() override;
  void exec() override;
  void fitGaussian(int64_t);
  void initWorkspace();

  Mantid::API::MatrixWorkspace_sptr m_inWS;
  Mantid::API::ITableWorkspace_sptr m_outWS;
  std::unique_ptr<Mantid::API::Progress> m_progress;
};

} // namespace Algorithms
} // namespace Mantid

#endif /* MANTID_ALGORITHMS_FINDEPP_H_ */
