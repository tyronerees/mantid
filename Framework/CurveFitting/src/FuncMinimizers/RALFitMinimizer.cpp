//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidCurveFitting/FuncMinimizers/RALFitMinimizer.h"

#include "MantidAPI/CostFunctionFactory.h"
#include "MantidAPI/FuncMinimizerFactory.h"

#include "MantidKernel/Logger.h"

namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {
  /*namespace{
  /// static logger object
  Kernel::Logger g_log("RALFitMinimizer");
  }*/

  DECLARE_FUNCMINIMIZER(RALFitMinimizer,RALFit)

  void RALFitMinimizer::initialize(API::ICostFunction_sptr costFunction,
				   size_t){
    // nothing here
  }

  bool RALFitMinimizer::iterate(size_t) {
    return true;
  }

  double RALFitMinimizer::costFunctionVal() {
    return 0.0;
  }
  
} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid
