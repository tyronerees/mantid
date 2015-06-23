#ifndef PoldiDetectorAdapter_H
#define PoldiDetectorAdapter_H

#include "MantidSINQ/DllConfig.h"

#include "MantidGeometry/Instrument.h"
#include "MantidGeometry/Instrument/DetectorGroup.h"

#include "MantidKernel/V3D.h"

#include <utility>

namespace Mantid {
namespace Poldi {

/** PoldiDetectorAdapter :
 *
  Abstract representation of detector, used for POLDI related calculations.
 Calculation
  methods are the repsonsibility of concrete implementations.

    @author Michael Wedel, Paul Scherrer Institut - SINQ
    @date 07/02/2014

    Copyright © 2014 PSI-MSS

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

class MANTID_SINQ_DLL PoldiDetectorAdapter {
public:
  PoldiDetectorAdapter(const Geometry::IComponent_const_sptr &detector);
  virtual ~PoldiDetectorAdapter() {}

  virtual double efficiency() const;

  virtual double twoTheta(size_t elementIndex) const;
  virtual double distanceFromSample(size_t elementIndex) const;

  virtual size_t elementCount() const;
  virtual size_t allElementCount() const;

  virtual size_t centralElement() const;


  virtual const std::vector<size_t> &availableElements() const;

  virtual std::pair<double, double> qLimits(double lambdaMin,
                                            double lambdaMax) const;

protected:
  std::vector<size_t> getNonMaskedDetectorIds(
      const std::vector<Geometry::IDetector_const_sptr> &detectors) const;

  Geometry::DetectorGroup_const_sptr m_detector;
  std::vector<Geometry::IDetector_const_sptr> m_detectors;
  std::vector<size_t> m_availableDetectors;
  double m_efficiency;

  Kernel::V3D m_samplePosition;
  Kernel::V3D m_beamDirection;
};

typedef boost::shared_ptr<PoldiDetectorAdapter> PoldiDetectorAdapter_sptr;
}
}
#endif // PoldiDetectorAdapter_H
