#ifndef MANTID_CRYSTAL_CHECKSPACEGROUPTEST_H_
#define MANTID_CRYSTAL_CHECKSPACEGROUPTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidCrystal/CheckSpaceGroup.h"

#include "MantidDataObjects/PeaksWorkspace.h"
#include "MantidGeometry/Crystal/BraggScattererFactory.h"
#include "MantidGeometry/Crystal/CompositeBraggScatterer.h"
#include "MantidGeometry/Crystal/CrystalStructure.h"
#include "MantidGeometry/Crystal/SpaceGroupFactory.h"

using Mantid::Crystal::CheckSpaceGroup;
using namespace Mantid::API;
using namespace Mantid::DataObjects;
using namespace Mantid::Geometry;
using namespace Mantid::Kernel;

class CheckSpaceGroupTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static CheckSpaceGroupTest *createSuite() {
    return new CheckSpaceGroupTest();
  }
  static void destroySuite(CheckSpaceGroupTest *suite) { delete suite; }

  void testMonoclinic() {
    checkSpaceGroup("C 1 2/m 1", "P 1 2/m 1", "C 1 2/c 1",
                    UnitCell(2.5, 3.5, 4.5, 90, 103, 90));
  }

  void testOrthorhombic() {
    checkSpaceGroup("C c c m", "P m m m", "C c c e", UnitCell(2.5, 3.5, 4.5));
  }

  void testTetragonal() {
    checkSpaceGroup("I 41/a m d", "P 4/m m m", "I 41/a c d",
                    UnitCell(2.5, 2.5, 4.5));
  }

  void testCubic() {
    checkSpaceGroup("F d -3 m", "P m -3 m", "F d -3 c",
                    UnitCell(2.5, 2.5, 2.5));
  }

private:
  /* This method constructs a crystal structure using the supplied cell and
   * the first space group and a scatterer on a general position. It calculates
   * structure factors for all reflections that would be allowed assuming no
   * translations, so some of them are 0 ("not observed").
   *
   * The algorithm is run with the same space group that was used to calculate
   * structure factors, which should result in 0 absence violations and 0
   * additional absences. Running it with the space group without translations
   * should give extra absences, while using a spage group with extra
   * translations should result in absence violations.
   */
  void checkSpaceGroup(const std::string &spaceGroup,
                       const std::string &spaceGroupNoTranslations,
                       const std::string &spaceGroupExtraTranslations,
                       const UnitCell &cell) {
    SpaceGroup_const_sptr sg =
        SpaceGroupFactory::Instance().createSpaceGroup(spaceGroup);
    SpaceGroup_const_sptr sgNoTranlsations =
        SpaceGroupFactory::Instance().createSpaceGroup(
            spaceGroupNoTranslations);

    CompositeBraggScatterer_sptr atoms =
        boost::make_shared<CompositeBraggScatterer>();
    atoms->addScatterer(BraggScattererFactory::Instance().createScatterer(
        "IsotropicAtomBraggScatterer",
        "Element=Fe;Position=[0.232,0.5312,0.654];U=0.005"));

    CrystalStructure_sptr cs =
        boost::make_shared<CrystalStructure>(cell, sg, atoms);

    IPeaksWorkspace_sptr peaks = getPeaksWorkspace(cs, sgNoTranlsations, 0.7);

    CheckSpaceGroup alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT(alg.isInitialized());

    /* Using the correct space group does not give any additional absences
     * or absence violations.
     */
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", peaks));
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("SpaceGroup", spaceGroup));

    TS_ASSERT_THROWS_NOTHING(alg.execute());

    int absenceViolations = alg.getProperty("SystematicAbsenceViolations");
    int additionalAbsences = alg.getProperty("AdditionalAbsences");

    TSM_ASSERT_EQUALS("Space group '" + spaceGroup +
                          "': Number of absence violations is not 0.",
                      absenceViolations, 0);
    TSM_ASSERT_EQUALS("Space group '" + spaceGroup +
                          "': Number of additional absences is not 0.",
                      additionalAbsences, 0);

    /* When the space group without translations is used, there are additional
     * absences, but since no translations means no systematic absences, there
     * can't be any violation.
     */
    alg.setProperty("InputWorkspace", peaks);
    alg.setProperty("SpaceGroup", spaceGroupNoTranslations);
    alg.execute();

    absenceViolations = alg.getProperty("SystematicAbsenceViolations");
    additionalAbsences = alg.getProperty("AdditionalAbsences");

    TSM_ASSERT_EQUALS("Space group '" + spaceGroup + "', no translations '" +
                          spaceGroupNoTranslations +
                          "': Number of absence violations is not 0.",
                      absenceViolations, 0);
    TSM_ASSERT_LESS_THAN(
        "Space group '" + spaceGroup + "', no translations '" +
            spaceGroupNoTranslations +
            "': Number of additional absences is 0, but should be larger.",
        0, additionalAbsences);

    /* Using a space group with extra translations means that it has more
     * systematic absences and the structure factors violate some of them.
     */
    alg.setProperty("InputWorkspace", peaks);
    alg.setProperty("SpaceGroup", spaceGroupExtraTranslations);
    alg.execute();

    absenceViolations = alg.getProperty("SystematicAbsenceViolations");
    additionalAbsences = alg.getProperty("AdditionalAbsences");

    TSM_ASSERT_LESS_THAN(
        "Space group '" + spaceGroup + "', extra translations '" +
            spaceGroupExtraTranslations +
            "': Number of absence violations is 0, but should be larger.",
        0, absenceViolations);

    TSM_ASSERT_EQUALS("Space group '" + spaceGroup + "', extra translations '" +
                          spaceGroupExtraTranslations +
                          "': Number of additional absences is not 0.",
                      additionalAbsences, 0);
  }

  IPeaksWorkspace_sptr
  getPeaksWorkspace(CrystalStructure_sptr &cs,
                    const SpaceGroup_const_sptr &superGroup, double dMin) {
    SpaceGroup_const_sptr oldGroup = cs->spaceGroup();
    cs->setSpaceGroup(superGroup);

    std::vector<V3D> hkls = cs->getHKLs(dMin, 100.0);
    cs->setSpaceGroup(oldGroup);

    std::vector<double> fSquared = cs->getFSquared(hkls);

    PeaksWorkspace_sptr peaks = boost::make_shared<PeaksWorkspace>();
    for (size_t i = 0; i < hkls.size(); ++i) {
      Peak p;
      p.setHKL(hkls[i]);
      p.setIntensity(fSquared[i]);
      p.setSigmaIntensity(sqrt(fabs(fSquared[i])));

      peaks->addPeak(p);
    }

    return peaks;
  }
};

#endif /* MANTID_CRYSTAL_CHECKSPACEGROUPTEST_H_ */
