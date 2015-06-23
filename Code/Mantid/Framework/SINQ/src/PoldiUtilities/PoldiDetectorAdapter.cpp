#include "MantidSINQ/PoldiUtilities/PoldiDetectorAdapter.h"
#include <boost/make_shared.hpp>

namespace Mantid {
namespace Poldi {

using namespace Geometry;
using namespace Kernel;

PoldiDetectorAdapter::PoldiDetectorAdapter(
    const IComponent_const_sptr &detector) {
  ICompAssembly_const_sptr detectorComponent =
      boost::dynamic_pointer_cast<const ICompAssembly>(detector);

  std::vector<IComponent_const_sptr> detectorsRawComponents;
  detectorComponent->getChildren(detectorsRawComponents, false);

  std::vector<IDetector_const_sptr> detectors;
  detectors.reserve(detectorsRawComponents.size());
  for (size_t i = 0; i < detectorsRawComponents.size(); ++i) {
    IDetector_const_sptr detPtr =
        boost::dynamic_pointer_cast<const IDetector>(detectorsRawComponents[i]);
    detectors.push_back(detPtr);
  }

  m_detector = boost::make_shared<const DetectorGroup>(detectors);
  m_detectors = m_detector->getDetectors();
  m_availableDetectors = getNonMaskedDetectorIds(m_detectors);
  m_efficiency = detector->getNumberParameter("efficiency").front();

  m_samplePosition = V3D(0, 0, 0);
  m_beamDirection = V3D(0, 0, 1);
}

double PoldiDetectorAdapter::efficiency() const { return m_efficiency; }

double PoldiDetectorAdapter::twoTheta(size_t elementIndex) const {
  return m_detectors[elementIndex]
      ->getTwoTheta(m_samplePosition, m_beamDirection);
}

double PoldiDetectorAdapter::distanceFromSample(size_t elementIndex) const {
  return m_detectors[elementIndex]->getPos().norm() * 1000.0;
}

size_t PoldiDetectorAdapter::elementCount() const {
  return m_availableDetectors.size();
}

size_t PoldiDetectorAdapter::allElementCount() const {
  return m_detectors.size();
}

size_t PoldiDetectorAdapter::centralElement() const {
  return (m_detectors.size() - 1) / 2;
}

const std::vector<size_t> &PoldiDetectorAdapter::availableElements() const {
  return m_availableDetectors;
}

std::pair<double, double>
PoldiDetectorAdapter::qLimits(double lambdaMin, double lambdaMax) const {
  return std::make_pair(4.0 * M_PI / lambdaMax * sin(twoTheta(0) / 2.0),
                        4.0 * M_PI / lambdaMin *
                            sin(twoTheta(m_detectors.size() - 1) / 2.0));
}

std::vector<size_t> PoldiDetectorAdapter::getNonMaskedDetectorIds(
    const std::vector<IDetector_const_sptr> &detectors) const {
  std::vector<size_t> ids;
  ids.reserve(detectors.size());

  for (size_t i = 0; i < detectors.size(); ++i) {
    if (!detectors[i]->isMasked()) {
      ids.push_back(i);
    }
  }

  return ids;
}
}
}
