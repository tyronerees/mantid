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

/// Returns true if the signal/noise ratio of the peak is above the threshold.
bool CheckSpaceGroup::isPeakObserved(const IPeak &peak,
                                     double obsThreshold) const {
  return peak.getIntensity() >= peak.getSigmaIntensity() * obsThreshold;
}

/// Returns true if the rounded HKL is allowed in the supplied space group.
bool
CheckSpaceGroup::isPeakAllowed(const IPeak &peak,
                               const SpaceGroup_const_sptr &spaceGroup) const {
  V3D hkl = peak.getHKL();
  hkl.round();

  return spaceGroup->isAllowedReflection(hkl);
}

/// Initialization
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

  declareProperty("ObservedThreshold", 3.0, "A peak is considered observed if "
                                            "its I >= ObservedThreshold * "
                                            "sigma(I).");

  declareProperty(new WorkspaceProperty<IPeaksWorkspace>(
                      "AbsenceViolationsWorkspace", "", Direction::Output),
                  "Number of peaks that should be absent according to space "
                  "group symmetry but are observed.");

  declareProperty(
      new WorkspaceProperty<IPeaksWorkspace>("AdditionalAbsencesWorkspace", "",
                                             Direction::Output),
      "Number of peaks that should be present according to space group "
      "symmetry but are not observed.");
}

void CheckSpaceGroup::exec() {
  IPeaksWorkspace_sptr peaks = getProperty("InputWorkspace");
  int peakCount = peaks->getNumberPeaks();

  SpaceGroup_const_sptr sg =
      SpaceGroupFactory::Instance().createSpaceGroup(getProperty("SpaceGroup"));
  double obsThreshold = getProperty("ObservedThreshold");

  IPeaksWorkspace_sptr absenceViolations =
      WorkspaceFactory::Instance().createPeaks();
  absenceViolations->copyExperimentInfoFrom(peaks.get());

  IPeaksWorkspace_sptr additionalAbsences =
      WorkspaceFactory::Instance().createPeaks();
  additionalAbsences->copyExperimentInfoFrom(peaks.get());

  for (int i = 0; i < peakCount; ++i) {
    IPeak *peak = peaks->getPeakPtr(i);

    bool isObserved = isPeakObserved(*peak, obsThreshold);
    bool isAllowed = isPeakAllowed(*peak, sg);

    if (isObserved && !isAllowed) {
      absenceViolations->addPeak(*peak);
    } else if (!isObserved && isAllowed) {
      additionalAbsences->addPeak(*peak);
    }
  }

  setProperty("AbsenceViolationsWorkspace", absenceViolations);
  setProperty("AdditionalAbsencesWorkspace", additionalAbsences);
}

} // namespace Crystal
} // namespace Mantid
