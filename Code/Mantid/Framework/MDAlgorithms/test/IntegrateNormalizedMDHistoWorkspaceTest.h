#ifndef MANTID_MDEVENTS_ConvertMDHistoToMDEventWorkspaceTEST_H_
#define MANTID_MDEVENTS_ConvertMDHistoToMDEventWorkspaceTEST_H_

#include "MantidMDAlgorithms/ConvertMDHistoToMDEventWorkspace.h"

#include <cxxtest/TestSuite.h>
#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidDataObjects/MDHistoWorkspace.h"
#include "MantidTestHelpers/MDEventsTestHelper.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"

#include <boost/lexical_cast.hpp>

using namespace Mantid;
using namespace Mantid::API;
using namespace Mantid::DataObjects;
using namespace Mantid::Kernel;
using namespace Mantid::MDAlgorithms;


class ConvertMDHistoToMDEventWorkspaceTest : public CxxTest::TestSuite
{
public:



  /**
    * Test conversion of a MD workspace to a 2D MDEventWorkspace.
    *
    */
  void test_convert()
  {
    // create an MD histo workspace
    size_t ndims = 3;
    // property values for CreateMDHistoWorkspace
    std::vector<size_t> numberOfBins(ndims);
    std::vector<std::string> names(ndims);
    // property values for SliceMDHisto
    std::vector<coord_t> start(ndims);
    std::vector<coord_t> end(ndims);
    for(size_t i = 0; i < ndims; ++i)
    {
      names[i] = "x_" + boost::lexical_cast<std::string>(i);
      numberOfBins[i] = 11;
      start[i] = -5;
      end[i] = 5;
    }
    signal_t signal(1.f), error(1.f);
    IMDHistoWorkspace_sptr slice = MDEventsTestHelper::makeFakeMDHistoWorkspaceGeneral(ndims, signal,
        error, &numberOfBins.front(), &start.front(), &end.front(), names);

    // test ConvertMDHistoToMDEventWorkspace
    auto alg = AlgorithmManager::Instance().create("ConvertMDHistoToMDEventWorkspace");
    alg->initialize();
    alg->setRethrows(true);
    alg->setChild(true);
    alg->setProperty("InputWorkspace", slice);
    alg->setPropertyValue("OutputWorkspace", "_2"); // Not really required for child algorithm


    try
    {
      alg->execute();
    }
    catch(std::exception& e)
    {
      TS_FAIL(e.what());
    }


    IMDEventWorkspace_sptr outWS = alg->getProperty("OutputWorkspace");
    TS_ASSERT( outWS );

    TS_ASSERT_EQUALS(3, outWS->getNumDims());

    TS_ASSERT_EQUALS(1331, outWS->getNPoints());
    TS_ASSERT_EQUALS("MDLeanEvent", outWS->getEventTypeName());


  }




};


#endif /* MANTID_MDEVENTS_ConvertMDHistoToMDEventWorkspaceTEST_H_ */
