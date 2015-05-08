#ifndef MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILETEST_H_
#define MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILETEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidDataHandling/ExportSampleLogsToCSVFile.h"

using Mantid::DataHandling::ExportSampleLogsToCSVFile;
using namespace Mantid::API;

class ExportSampleLogsToCSVFileTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static ExportSampleLogsToCSVFileTest *createSuite() { return new ExportSampleLogsToCSVFileTest(); }
  static void destroySuite( ExportSampleLogsToCSVFileTest *suite ) { delete suite; }


  void test_Something()
  {
    TSM_ASSERT( "You forgot to write a test!", 0);
  }


};


#endif /* MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILETEST_H_ */