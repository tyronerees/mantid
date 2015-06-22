#include "MantidSINQ/PoldiUtilities/PoldiDetectorFactory.h"

#include "MantidSINQ/PoldiUtilities/PoldiHeliumDetector.h"

namespace Mantid {
namespace Poldi {
using namespace boost::gregorian;

PoldiDetectorFactory::PoldiDetectorFactory()
    : m_newDetectorDate(from_string(std::string("2016/01/01"))) {}

PoldiDetectorAdapter *
PoldiDetectorFactory::createDetector(std::string detectorType) {
  UNUSED_ARG(detectorType);

  return new PoldiHeliumDetector();
}

PoldiDetectorAdapter *
PoldiDetectorFactory::createDetector(date experimentDate) {
  if (experimentDate < m_newDetectorDate) {
    return new PoldiHeliumDetector();
  }

  return 0;
}

} // namespace Poldi
} // namespace Mantid
