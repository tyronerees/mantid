#ifndef MANTID_GEOMETRY_CRYSTALSTRUCTURETEST_H_
#define MANTID_GEOMETRY_CRYSTALSTRUCTURETEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidGeometry/Crystal/CrystalStructure.h"
#include "MantidGeometry/Crystal/PointGroupFactory.h"
#include "MantidGeometry/Crystal/SpaceGroupFactory.h"

#include "MantidGeometry/Crystal/CompositeScatterer.h"
#include "MantidGeometry/Crystal/IsotropicAtomScatterer.h"

using namespace Mantid::Geometry;
using namespace Mantid::Kernel;

class CrystalStructureTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static CrystalStructureTest *createSuite() { return new CrystalStructureTest(); }
  static void destroySuite( CrystalStructureTest *suite ) { delete suite; }

  CrystalStructureTest() :
      m_CsCl(4.126, 4.126, 4.126),
      m_pg(PointGroupFactory::Instance().createPointGroup("m-3m")),
      m_centering(new ReflectionConditionPrimitive)
  {
      m_spaceGroup = SpaceGroupFactory::Instance().createSpaceGroup("I m -3 m");

      m_scatterers = CompositeScatterer::create();
      m_scatterers->addScatterer(IsotropicAtomScatterer::create("Si", V3D(0.25, 0.25, 0.25)));
  }


  void testConstructionDefault()
  {
      // Only cell is really required, for the others there's a default value
      CrystalStructure structure(m_CsCl);

      TS_ASSERT_EQUALS(structure.cell().a(), m_CsCl.a());
      TS_ASSERT(boost::dynamic_pointer_cast<PointGroupLaue1>(structure.pointGroup()));
      TS_ASSERT(boost::dynamic_pointer_cast<ReflectionConditionPrimitive>(structure.centering()));
      TS_ASSERT_THROWS_NOTHING(structure.crystalSystem());
      TS_ASSERT_EQUALS(structure.crystalSystem(), PointGroup::Triclinic);

      CrystalStructure structurePg(m_CsCl, m_pg);
      TS_ASSERT_EQUALS(structurePg.pointGroup(), m_pg);
      TS_ASSERT(boost::dynamic_pointer_cast<ReflectionConditionPrimitive>(structurePg.centering()));
      TS_ASSERT_EQUALS(structurePg.crystalSystem(), m_pg->crystalSystem());

      CrystalStructure structureAll(m_CsCl, m_pg, m_centering);
      TS_ASSERT_EQUALS(structureAll.centering(), m_centering);
  }

  void testConstructionSpaceGroup()
  {
      TS_ASSERT_THROWS_NOTHING(CrystalStructure structure(m_CsCl, m_spaceGroup, m_scatterers));
      CrystalStructure structure(m_CsCl, m_spaceGroup, m_scatterers);
      TS_ASSERT_EQUALS(structure.cell().getG(), m_CsCl.getG());
      TS_ASSERT_EQUALS(structure.spaceGroup(), m_spaceGroup);
      TS_ASSERT_EQUALS(structure.getScatterers()->nScatterers(), m_scatterers->nScatterers());
  }

  void testSetSpaceGroup()
  {
      CrystalStructure structure(m_CsCl, m_pg, m_centering);

      // Space group is null
      TS_ASSERT(!structure.spaceGroup());
      TS_ASSERT(!structure.getScatterers()->getSpaceGroup());

      TS_ASSERT_THROWS_NOTHING(structure.setSpaceGroup(m_spaceGroup));

      // Not null anymore
      TS_ASSERT(structure.spaceGroup())
      // Space group on scatterer collection should be the same
      TS_ASSERT_EQUALS(structure.getScatterers()->getSpaceGroup()->hmSymbol(), "I m -3 m");

      // pointers are different
      TS_ASSERT_DIFFERS(structure.pointGroup(), m_pg);
      // symbol is the same
      TS_ASSERT_EQUALS(structure.pointGroup()->getSymbol(), "m-3m");

      // pointers are different
      TS_ASSERT_DIFFERS(structure.centering(), m_centering);
      // symbols as well
      TS_ASSERT_DIFFERS(structure.centering()->getSymbol(), m_centering->getSymbol());
      TS_ASSERT_EQUALS(structure.centering()->getSymbol(), "I");
  }

  void testCellGetSet()
  {
      CrystalStructure structure(m_CsCl);
      TS_ASSERT_EQUALS(structure.cell().a(), m_CsCl.a());

      UnitCell Si(5.43, 5.43, 5.43);
      structure.setCell(Si);

      TS_ASSERT_EQUALS(structure.cell().a(), Si.a());
  }

  void testPointGroupGetSet()
  {
      CrystalStructure structure(m_CsCl, m_pg);
      TS_ASSERT_EQUALS(structure.pointGroup(), m_pg);
      TS_ASSERT_EQUALS(structure.crystalSystem(), m_pg->crystalSystem());

      PointGroup_sptr newPg = boost::make_shared<PointGroupLaue3>();
      structure.setPointGroup(newPg);

      TS_ASSERT_EQUALS(structure.pointGroup(), newPg);
      TS_ASSERT_EQUALS(structure.crystalSystem(), newPg->crystalSystem());

      // setting a space group makes setting a point group impossible
      structure.setSpaceGroup(m_spaceGroup);
      TS_ASSERT_DIFFERS(structure.crystalSystem(), newPg->crystalSystem());

      TS_ASSERT_THROWS(structure.setPointGroup(newPg), std::runtime_error);
  }

  void testCenteringGetSet()
  {
      CrystalStructure structure(m_CsCl, m_pg, m_centering);
      TS_ASSERT_EQUALS(structure.centering(), m_centering);

      ReflectionCondition_sptr newCentering = boost::make_shared<ReflectionConditionAFaceCentred>();
      structure.setCentering(newCentering);

      TS_ASSERT_EQUALS(structure.centering(), newCentering);

      // setting a space group makes setting a centering impossible
      structure.setSpaceGroup(m_spaceGroup);
      TS_ASSERT_DIFFERS(structure.centering()->getSymbol(), newCentering->getSymbol());

      TS_ASSERT_THROWS(structure.setCentering(newCentering), std::runtime_error);
  }

  void testSufficientStateForHKLGeneration()
  {
      TestableCrystalStructure structure;

      // Default is "UseCentering"
      ReflectionCondition_sptr nullCentering;
      structure.setCentering(nullCentering);
      TS_ASSERT(!structure.isStateSufficientForHKLGeneration(CrystalStructure::UseCentering));
      structure.setCentering(m_centering);
      TS_ASSERT(structure.isStateSufficientForHKLGeneration(CrystalStructure::UseCentering));

      // Structure factor requires at least one scatterer - otherwise all hkl are "forbidden"
      TS_ASSERT(!structure.isStateSufficientForHKLGeneration(CrystalStructure::UseStructureFactor));
      structure.setScatterers(m_scatterers);
      TS_ASSERT(structure.isStateSufficientForHKLGeneration(CrystalStructure::UseStructureFactor));

      // centering does not matter for this
      structure.setCentering(nullCentering);
      TS_ASSERT(structure.isStateSufficientForHKLGeneration(CrystalStructure::UseStructureFactor));

  }

  void testSufficientStateForUniqueHKLGeneration()
  {
      TestableCrystalStructure structure;

      // Default is "UseCentering"
      ReflectionCondition_sptr nullCentering;
      PointGroup_sptr nullPointGroup;

      structure.setCentering(nullCentering);
      structure.setPointGroup(nullPointGroup);

      // Does not work with null point group
      TS_ASSERT(!structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseCentering));

      // not even when centering is set
      structure.setCentering(m_centering);
      TS_ASSERT(!structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseCentering));

      // now it's okay
      structure.setPointGroup(m_pg);
      TS_ASSERT(structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseCentering));

      // Structure factor requires at least one scatterer - otherwise all hkl are "forbidden"
      TS_ASSERT(!structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseStructureFactor));
      structure.setScatterers(m_scatterers);
      TS_ASSERT(structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseStructureFactor));

      // point group is required anyway
      structure.setPointGroup(nullPointGroup);
      TS_ASSERT(!structure.isStateSufficientForUniqueHKLGeneration(CrystalStructure::UseStructureFactor));
   }

  void testThrowIfRangeUnacceptable()
  {
      TestableCrystalStructure structure;

      TS_ASSERT_THROWS(structure.throwIfRangeUnacceptable(0.0, 1.0), std::invalid_argument);
      TS_ASSERT_THROWS(structure.throwIfRangeUnacceptable(-10.0, 1.0), std::invalid_argument);
      TS_ASSERT_THROWS(structure.throwIfRangeUnacceptable(1.0, 0.0), std::invalid_argument);
      TS_ASSERT_THROWS(structure.throwIfRangeUnacceptable(1.0, -1.0), std::invalid_argument);
      TS_ASSERT_THROWS(structure.throwIfRangeUnacceptable(2.0, 1.0), std::invalid_argument);

      TS_ASSERT_THROWS_NOTHING(structure.throwIfRangeUnacceptable(1.0, 2.0))
  }

  void testGetUniqueHKLsHappyCase()
  {
      double dMin = 0.55;
      double dMax = 4.0;

      CrystalStructure structure(m_CsCl, m_pg, m_centering);

      TS_ASSERT_THROWS_NOTHING(structure.getUniqueHKLs(dMin, dMax));

      std::vector<V3D> peaks = structure.getUniqueHKLs(dMin, dMax);

      TS_ASSERT_EQUALS(peaks.size(), 68);
      TS_ASSERT_EQUALS(peaks[0], V3D(1, 1, 0));
      TS_ASSERT_EQUALS(peaks[11], V3D(3, 2, 0));
      TS_ASSERT_EQUALS(peaks[67], V3D(7, 2, 1));

      // make d-value list and check that peaks are within limits
      std::vector<double> peaksD = structure.getDValues(peaks);

      std::sort(peaksD.begin(), peaksD.end());

      TS_ASSERT_LESS_THAN_EQUALS(dMin, peaksD.front());
      TS_ASSERT_LESS_THAN_EQUALS(peaksD.back(), dMax);
  }

  void testGetHKLsHappyCase()
  {
      double dMin = 0.55;
      double dMax = 4.0;

      // make a structure with P-1
      CrystalStructure structure(m_CsCl, PointGroupFactory::Instance().createPointGroup("-1"));

      std::vector<V3D> unique = structure.getUniqueHKLs(dMin, dMax);
      std::vector<V3D> peaks = structure.getHKLs(dMin, dMax);

      // Because of symmetry -1, each reflection has multiplicity 2.
      TS_ASSERT_EQUALS(peaks.size(), 2 * unique.size());
  }

  void testGetDValues()
  {
      std::vector<V3D> hkls;
      hkls.push_back(V3D(1, 0, 0));
      hkls.push_back(V3D(0, 1, 0));
      hkls.push_back(V3D(0, 0, 1));

      UnitCell ortho(2.0, 3.0, 5.0);
      CrystalStructure structure(ortho);

      std::vector<double> dValues = structure.getDValues(hkls);

      TS_ASSERT_EQUALS(dValues.size(), hkls.size());
      TS_ASSERT_EQUALS(dValues[0], 2.0);
      TS_ASSERT_EQUALS(dValues[1], 3.0);
      TS_ASSERT_EQUALS(dValues[2], 5.0);
  }

private:
    UnitCell m_CsCl;
    PointGroup_sptr m_pg;
    ReflectionCondition_sptr m_centering;

    SpaceGroup_const_sptr m_spaceGroup;
    CompositeScatterer_sptr m_scatterers;

    class TestableCrystalStructure : public CrystalStructure
    {
        friend class CrystalStructureTest;
    public:
        TestableCrystalStructure() : CrystalStructure(UnitCell()) { }
        ~TestableCrystalStructure() { }
    };
};


#endif /* MANTID_GEOMETRY_CRYSTALSTRUCTURETEST_H_ */
