#ifndef MANTID_MDEVENTS_MDPRODUCTTEST_H_
#define MANTID_MDEVENTS_MDPRODUCTTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidMDEvents/MDProduct.h"

using Mantid::MDEvents::MDProduct;
using namespace Mantid::API;

class MDProductTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static MDProductTest *createSuite() { return new MDProductTest(); }
  static void destroySuite( MDProductTest *suite ) { delete suite; }


  void test_Something()
  {
    TSM_ASSERT( "You forgot to write a test!", 0);
  }


};


#endif /* MANTID_MDEVENTS_MDPRODUCTTEST_H_ */