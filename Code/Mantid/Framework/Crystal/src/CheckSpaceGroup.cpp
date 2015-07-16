#include "MantidCrystal/CheckSpaceGroup.h"
#include "MantidAPI/IPeaksWorkspace.h"
#include "MantidGeometry/Crystal/SpaceGroupFactory.h"
#include "MantidKernel/ListValidator.h"

namespace Mantid {
namespace Crystal {

DECLARE_ALGORITHM(CheckSpaceGroup)

using namespace API;
using namespace Geometry;
using namespace Kernel;

CheckSpaceGroup::CheckSpaceGroup() {}

bool CheckSpaceGroup::isPeakObserved(const IPeak &peak,
                                     double obsThreshold) const {
  return peak.getIntensity() / peak.getSigmaIntensity() >= obsThreshold;
}

bool
CheckSpaceGroup::isPeakAllowed(const IPeak &peak,
                               const SpaceGroup_const_sptr &spaceGroup) const {
  V3D hkl = peak.getHKL();
  hkl.round();

  return spaceGroup->isAllowedReflection(hkl);
}

void CheckSpaceGroup::init() {
  declareProperty(
      new WorkspaceProperty<IPeaksWorkspace>("InputWorkspace", "",
                                            Direction::Input),
      "A workspace with indexed and integrated single crystal peaks.");

  std::vector<std::string> spaceGroups =
      SpaceGroupFactory::Instance().subscribedSpaceGroupSymbols();
  declareProperty("SpaceGroup", spaceGroups.front(),
                  boost::make_shared<StringListValidator>(spaceGroups),
                  "Space group to check peaks against.");

  declareProperty("SigmaMultiples", 3.0, "A peak is considered observed if its "
                                         "intensity is larger than "
                                         "SigmaMultiples times its error.");

  declareProperty("SystematicAbsenceViolations", 0,
                  "Number of peaks that should be absent according to space "
                  "group symmetry but are observed.",
                  Direction::Output);

  declareProperty("AdditionalAbsences", 0, "Number of peaks that should be "
                                           "present according to space group "
                                           "symmetry but are not observed.",
                  Direction::Output);
}

void CheckSpaceGroup::exec() {
  IPeaksWorkspace_sptr peaks = getProperty("InputWorkspace");
  double obsThreshold = getProperty("SigmaMultiples");
  SpaceGroup_const_sptr sg =
      SpaceGroupFactory::Instance().createSpaceGroup(getProperty("SpaceGroup"));

  int peakCount = peaks->getNumberPeaks();

  std::vector<IPeak *> absenceViolations;
  absenceViolations.reserve(static_cast<size_t>(peakCount));

  std::vector<IPeak *> additionalAbsences;
  additionalAbsences.reserve(static_cast<size_t>(peakCount));

  for (int i = 0; i < peakCount; ++i) {
    IPeak *peak = peaks->getPeakPtr(i);

    bool isObserved = isPeakObserved(*peak, obsThreshold);
    bool isAllowed = isPeakAllowed(*peak, sg);

    if (isObserved && !isAllowed) {
      absenceViolations.push_back(peak);
    } else if (!isObserved && isAllowed) {
      additionalAbsences.push_back(peak);
    }
  }

  setProperty("SystematicAbsenceViolations",
              static_cast<int>(absenceViolations.size()));
  setProperty("AdditionalAbsences",
              static_cast<int>(additionalAbsences.size()));
}

} // namespace Crystal
} // namespace Mantid
