#ifndef MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_
#define MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidDataHandling/LoadHFIRPDD.h"

using Mantid::DataHandling::LoadHFIRPDD;
using namespace Mantid::API;

class LoadHFIRPDDTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static LoadHFIRPDDTest *createSuite() { return new LoadHFIRPDDTest(); }
  static void destroySuite( LoadHFIRPDDTest *suite ) { delete suite; }


  void test_Init()
  {
    LoadHFIRPDD loader;
    loader.initialize();
    TS_ASSERT(loader.isInitialized());
  }

  void test_LoadHB2AData()
  {
    LoadHFIRPDD loader;
    loader.initialize();

    loader.setPropertyValue("Filename", "HB2A_exp0231_scan0001.dat");
    loader.setPropertyValue("OutputWorkspace", "HB2A_MD");

    loader.execute();
  }


};


#endif /* MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_ */
