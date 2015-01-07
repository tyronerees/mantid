#ifndef MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_
#define MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidDataHandling/LoadHFIRPDD.h"
#include "MantidDataHandling/LoadNexusProcessed.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidDataHandling/LoadInstrument.h"

using Mantid::DataHandling::LoadHFIRPDD;
using Mantid::DataHandling::LoadNexusProcessed;
using Mantid::DataHandling::LoadInstrument;

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

  /** Test loading HB2A's IDF file
   * @brief test_HB2BIDF
   */
  void test_HB2AIDF() {
    MatrixWorkspace_sptr dataws =
        WorkspaceFactory::Instance().create("Workspace2D", 44, 2, 1);
    AnalysisDataService::Instance().addOrReplace("EmptyWS", dataws);

    LoadInstrument loader;
    loader.initialize();

    loader.setProperty("InstrumentName", "HB2A");
    loader.setProperty("Workspace", dataws);

    loader.execute();
    TS_ASSERT(loader.isExecuted());

    MatrixWorkspace_sptr outws = boost::dynamic_pointer_cast<MatrixWorkspace>(
        AnalysisDataService::Instance().retrieve("EmptyWS"));
    TS_ASSERT(outws);

    TS_ASSERT_EQUALS(outws->getInstrument()->getName(), "HB2A");
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
    loader.setProperty("Instrument", "HB2A");
    loader.setProperty("RunStart", "2012-08-13T11:57:33");
    loader.setPropertyValue("OutputWorkspace", "HB2A_MD");

    loader.execute();
    TS_ASSERT(loader.isExecuted());

    TS_ASSERT_EQUALS(1, 29121);

    IMDEventWorkspace_sptr mdws =
        boost::dynamic_pointer_cast<IMDEventWorkspace>(
            AnalysisDataService::Instance().retrieve("HB2A_MD"));
    TS_ASSERT(mdws);
  }


};


#endif /* MANTID_DATAHANDLING_LOADHFIRPDDTEST_H_ */
