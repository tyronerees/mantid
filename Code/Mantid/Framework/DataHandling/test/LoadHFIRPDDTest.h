#ifndef MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_
#define MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidDataHandling/LoadHFIRPDD.h"
#include "MantidDataHandling/LoadNexusProcessed.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/ITableWorkspace.h"

using Mantid::DataHandling::LoadHFIRPDD;
using Mantid::DataHandling::LoadNexusProcessed;

using namespace Mantid::API;
using namespace Mantid::DataObjects;

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
    // This is the solution of stage 1. Stage 2 will be different
    LoadNexusProcessed nxsloader;
    nxsloader.initialize();
    nxsloader.setProperty("Filename", "HB2A_exp0231_tabledata.nxs");
    nxsloader.setProperty("OutputWorkspace", "DataTable");
    nxsloader.execute();
    ITableWorkspace_sptr datatablews =
        boost::dynamic_pointer_cast<ITableWorkspace>(
            AnalysisDataService::Instance().retrieve("DataTable"));
    TS_ASSERT(datatablews);

    nxsloader.setProperty("Filename", "HB2A_exp0231_log.nxs");
    nxsloader.setProperty("OutputWorkspace", "LogParentWS");
    nxsloader.execute();
    MatrixWorkspace_sptr parentlogws =
        boost::dynamic_pointer_cast<MatrixWorkspace>(
            AnalysisDataService::Instance().retrieve("LogParentWS"));
    TS_ASSERT(parentlogws);

    LoadHFIRPDD loader;
    loader.initialize();

    // loader.setPropertyValue("Filename", "HB2A_exp0231_scan0001.dat");
    loader.setProperty("InputWorkspace", datatablews);
    loader.setProperty("ParentWorkspace", parentlogws);
    loader.setPropertyValue("OutputWorkspace", "HB2A_MD");

    loader.execute();
  }


};


#endif /* MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_ */
