#ifndef MANTID_KERNEL_MDUNITTEST_H_
#define MANTID_KERNEL_MDUNITTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/MDUnit.h"
#include "MantidKernel/UnitLabelTypes.h"

using namespace Mantid::Kernel;

class MDUnitTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static MDUnitTest *createSuite() { return new MDUnitTest(); }
  static void destroySuite( MDUnitTest *suite ) { delete suite; }


  void test_RLU_getUnitLabel(){
      ReciprocalLatticeUnit unit;
      TS_ASSERT_EQUALS(Units::Symbol::RLU, unit.getUnitLabel());
  }

  void test_RLU_canConvertTo_does_not_convert_to_just_anything(){
      ReciprocalLatticeUnit unit;
      LabelUnit other("MeV");
      TSM_ASSERT("Conversion forbidden", !unit.canConvertTo(other));
      TS_ASSERT_DIFFERS(unit, other);
  }

  void test_RLU_canConvertTo_InverseAngstroms(){
      ReciprocalLatticeUnit unit;
      InverseAngstromsUnit other;
      TSM_ASSERT("Simple conversion possible", unit.canConvertTo(other));
      TS_ASSERT(unit == other);
  }

  void test_InverseAngstroms_getUnitLabel(){
      InverseAngstromsUnit unit;
      TS_ASSERT_EQUALS(Units::Symbol::InverseAngstrom, unit.getUnitLabel());
  }

  void test_InverseAngstroms_canConvertTo_does_not_convert_to_just_anything(){
      InverseAngstromsUnit unit;
      LabelUnit other("MeV");
      TSM_ASSERT("Conversion forbidden", !unit.canConvertTo(other));
      TS_ASSERT_DIFFERS(unit, other);
  }

  void test_InverseAnstroms_canConvertTo_RLU(){
      ReciprocalLatticeUnit unit;
      InverseAngstromsUnit other;
      TSM_ASSERT("Simple conversion possible", unit.canConvertTo(other));
      TS_ASSERT(unit == other);
  }

  void test_labelUnit_getUnitLabel(){
      // Negative test
      LabelUnit tUnit("DegC");
      TSM_ASSERT_DIFFERS("Not same unit label", UnitLabel("SomethingElse"), tUnit.getUnitLabel());

      // Positive test
      TSM_ASSERT_EQUALS("Same unit label", UnitLabel("DegC"), tUnit.getUnitLabel());
  }

  void test_LabelUnit_canConvert_to_same(){
      LabelUnit a("Bar");
      LabelUnit b("Bar");
      TS_ASSERT(a.canConvertTo(b));
      TS_ASSERT(a == b);
  }

  void test_LabelUnit_canConvert_to_other(){
      LabelUnit a("DegC");
      LabelUnit b("Bar");
      TS_ASSERT(!a.canConvertTo(b));
      TS_ASSERT_DIFFERS(a, b);
  }




};


#endif /* MANTID_KERNEL_MDUNITTEST_H_ */
