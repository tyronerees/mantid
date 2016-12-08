#ifndef MANTID_CURVEFITTING_RALFITMINIMIZER_H_
#define MANTID_CURVEFITTING_RALFITMINIMIZER_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/IFuncMinimizer.h"
#include "MantidAPI/IFunction.h"
#include "MantidCurveFitting/CostFunctions/CostFuncLeastSquares.h"
//#include "MantidCurveFitting/CostFunctions/CostFuncLeastSquares.h"
//#include "MantidCurveFitting/FortranDefs.h"
//#include "MantidCurveFitting/GSLJacobian.h"
//#include "MantidCurveFitting/RalNlls/Workspaces.h"
#include "MantidCurveFitting/GSLVector.h"
#include "MantidCurveFitting/GSLMatrix.h"

namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {
/** A base class for RALFit minimizers.
*/
  
class DLLExport RALFitMinimizer : public API::IFuncMinimizer {
public:
  /// constructor and destructor
  RALFitMinimizer();
  
  /*  void initialize(API::ICostFunction_sptr costFunction,
                  size_t maxIterations = 0) override;

  
		  std::string name() const override { return "RALFit"; }*/
  
  //  // Name of the minimizer
  std::string name() const override { return "RALFit"; }
  
   /// Initialize minimizer, i.e. pass a function to minimize.
  void initialize(API::ICostFunction_sptr function,
                  size_t maxIterations = 0) override;
  /// Initialize minimizer, i.e. pass a function to minimize.
  //  void initialize() override;
  
  /// Do one iteration.
  bool iterate(size_t) override;
  
  /// Return current value of the cost function
  double costFunctionVal() override;


private:
  //  int hasConverged();
  /*  /// Evaluate the fitting function and calculate the residuals.
  void evalF(const DoubleFortranVector &x, DoubleFortranVector &f) const;
  /// Evaluate the Jacobian
  void evalJ(const DoubleFortranVector &x, DoubleFortranMatrix &J) const;
  /// Evaluate the Hessian
  void evalHF(const DoubleFortranVector &x, const DoubleFortranVector &f,
              DoubleFortranMatrix &h) const;
  /// Find a correction vector to the parameters.
  virtual void
  calculateStep(const DoubleFortranMatrix &J, const DoubleFortranVector &f,
                const DoubleFortranMatrix &hf, const DoubleFortranVector &g,
                double Delta, DoubleFortranVector &d, double &normd,
                const NLLS::nlls_options &options, NLLS::nlls_inform &inform,
                NLLS::calculate_step_work &w) = 0;

  /// Stored cost function
  boost::shared_ptr<CostFunctions::CostFuncLeastSquares> m_leastSquares;
  /// Stored to access IFunction interface in iterate()
  API::IFunction_sptr m_function;
  /// Fitting data weights
  DoubleFortranVector m_weights;
  /// Fitting parameters
  DoubleFortranVector m_x;
  /// The Jacobian
  mutable JacobianImpl1 m_J;
  /// Options
  NLLS::nlls_options m_options;
  /// Information about the fitting
  NLLS::nlls_inform m_inform;
  /// Temporary and helper objects
  NLLS::NLLS_workspace m_workspace;*/
  
};

} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid

#endif /*MANTID_CURVEFITTING_RALFITMINIMIZER_H_*/
