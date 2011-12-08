#ifndef MANTID_ALGORITHMS_WEIGHTINGSTRATEGYTEST_H_
#define MANTID_ALGORITHMS_WEIGHTINGSTRATEGYTEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidKernel/Timer.h"
#include "MantidKernel/System.h"
#include <iostream>
#include <iomanip>

#include "MantidAlgorithms/WeightingStrategy.h"

using namespace Mantid;
using namespace Mantid::Kernel;
using namespace Mantid::Algorithms;

class WeightingStrategyTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static WeightingStrategyTest *createSuite() { return new WeightingStrategyTest(); }
  static void destroySuite( WeightingStrategyTest *suite ) { delete suite; }


  void testNullWeightingStrategyAtRadiusThrows()
  {
    NullWeighting strategy;
    V3D distance;
    TSM_ASSERT_THROWS("NullWeighting should always throw in usage", strategy.weightAt(distance), std::runtime_error);
  }

  void testNullWeightingStrategyRectangularThrows()
  {
    NullWeighting strategy;
    int adjX = 0;
    int adjY = 0;
    int ix = 0;
    int iy = 0;
    TSM_ASSERT_THROWS("NullWeighting should always throw in usage", strategy.weightAt(adjX, adjY, ix, iy), std::runtime_error);
  }

  void testFlatWeightingStrategyAtRadius()
  {
    FlatWeighting strategy;
    V3D distanceA(0,0,0);
    V3D distanceB(10, 10, 10);
    TSM_ASSERT_EQUALS("FlatWeighting Should be distance insensitive", 1, strategy.weightAt(distanceA));
    TSM_ASSERT_EQUALS("FlatWeighting Should be distance insensitive", 1, strategy.weightAt(distanceB));
  }

  void testFlatWeightingStrategyRectangular()
  {
    FlatWeighting strategy;
    int adjX = 0;
    int adjY = 0;
    int ix = 0;
    int iy = 0;
    TSM_ASSERT_EQUALS("FlatWeighting Should be 1", 1, strategy.weightAt(adjX, ix, adjY, iy));
  }

  void testLinearWeightingAtRadius()
  {
    double cutOff = 2;
    LinearWeighting strategy(cutOff);

    V3D distanceAtOrigin(0,0,0);
    TSM_ASSERT_EQUALS("LinearWeighting should give full weighting at origin", 1, strategy.weightAt(distanceAtOrigin));
    V3D distanceAtMidPoint(1,0,0);
    TSM_ASSERT_EQUALS("LinearWeighting should give 0.5 weighting at 1/2 radius", 0.5, strategy.weightAt(distanceAtMidPoint));
    V3D distanceAtEdge(cutOff,0,0); //2
    TSM_ASSERT_EQUALS("LinearWeighting should give zero weighting at cutoff", 0, strategy.weightAt(distanceAtEdge));
  }

  void testLinearWeightingRectangular()
  {
    double cutOff = 0; //Doesn't matter what the cut off is.
    LinearWeighting strategy(cutOff);

    int adjX = 2;
    int adjY = 2; 

    int ix = 2; int iy = 2;
    TSM_ASSERT_EQUALS("Top-Right not calculated properly", 0, strategy.weightAt(adjX, ix, adjY, iy));
    ix = -2; iy = 2;
    TSM_ASSERT_EQUALS("Top-Left not calculated properly", 0, strategy.weightAt(adjX, ix, adjY, iy));
    ix = 2; iy = -2;
    TSM_ASSERT_EQUALS("Bottom-Right not calculated properly", 0, strategy.weightAt(adjX, ix, adjY, iy));
    ix = -2; iy = -2;
    TSM_ASSERT_EQUALS("Bottom-Left not calculated properly", 0, strategy.weightAt(adjX, ix, adjY, iy));
    ix = 0; iy = 0;
    TSM_ASSERT_EQUALS("Center not calculated properly", 1, strategy.weightAt(adjX, ix, adjY, iy));
    ix = 1; iy = 1;
    TSM_ASSERT_EQUALS("Half radius not calculated properly", 0.5, strategy.weightAt(adjX, ix, adjY, iy));
  }

  void testParabolicWeightingThrows()
  {
    ParabolicWeighting strategy;
    V3D distance;
    TSM_ASSERT_THROWS("Should not be able to use the ParabolicWeighting like this.", strategy.weightAt(distance), std::runtime_error);
  }

  void testParabolicWeightingRectangular()
  {
    ParabolicWeighting strategy;

    int adjX = 2;
    int adjY = 2; 

    int ix = 2; int iy = 2;
    TSM_ASSERT_EQUALS("Top-Right not calculated properly", 1, strategy.weightAt(adjX, ix, adjY, iy));
    ix = -2; iy = 2;
    TSM_ASSERT_EQUALS("Top-Left not calculated properly", 1, strategy.weightAt(adjX, ix, adjY, iy));
    ix = 2; iy = -2;
    TSM_ASSERT_EQUALS("Bottom-Right not calculated properly", 1, strategy.weightAt(adjX, ix, adjY, iy));
    ix = -2; iy = -2;
    TSM_ASSERT_EQUALS("Bottom-Left not calculated properly", 1, strategy.weightAt(adjX, ix, adjY, iy));
    ix = 0; iy = 0;
    TSM_ASSERT_EQUALS("Center not calculated properly", 5, strategy.weightAt(adjX, ix, adjY, iy));
  }

  void testGaussian1D()
  {
    GaussianWeighting1D weighting(4, 0.5);

    double expectedY[] = {0.1080,
      0.2590,
      0.4839,
      0.7041,
      0.7979,
      0.7041,
      0.4839,
      0.2590,
      0.1080
    };

    int count = 0;
    for(double i = -4; i <= 4; i+=1)
    {
      V3D distance(i, 0, 0);
      double y = weighting.weightAt(distance);
      double yExpected = expectedY[count];
      TS_ASSERT_DELTA(yExpected, y, 0.0001);
      count++;
    }
  }

  void testGaussian1DRectangular()
  {
    GaussianWeighting1D weighting(1,0.5);

    double expectedY[] = {0.1080,
      0.2590,
      0.4839,
      0.7041,
      0.7979,
      0.7041,
      0.4839,
      0.2590,
      0.1080
    };
    double adjX = 4;
    double adjY = 4;
    double fixedPoint = 0;
    int count = 0;
    for(double i = -4; i <= 4; i+=1)
    {
      double yExpected = expectedY[count];
      double y1 = weighting.weightAt(adjX, i, fixedPoint, fixedPoint); 
      double y2 = weighting.weightAt(fixedPoint, fixedPoint, adjY, i);
      TS_ASSERT_DELTA(yExpected, y1, 0.0001);
      TS_ASSERT_DELTA(yExpected, y2, 0.0001);
      TS_ASSERT_EQUALS(y1, y2);
      count++;
    }
  }


};


#endif /* MANTID_ALGORITHMS_WEIGHTINGSTRATEGYTEST_H_ */
