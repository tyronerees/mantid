#ifndef MANTID_CURVEFITTING_RALFITMINIMIZER_H_
#define MANTID_CURVEFITTING_RALFITMINIMIZER_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/IFuncMinimizer.h"
#include "MantidAPI/IFunction.h"
#include "MantidCurveFitting/CostFunctions/CostFuncLeastSquares.h"
#include "MantidCurveFitting/FortranDefs.h"
#include "MantidCurveFitting/GSLVector.h"
#include "MantidCurveFitting/GSLJacobian.h"
#include "MantidCurveFitting/GSLFunctions.h"
#include "ral_nlls.h"

namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {
/** A base class for RALFit minimizers.
*/
  
class DLLExport RALFitMinimizer : public API::IFuncMinimizer {
public:
  /// constructor and destructor
  RALFitMinimizer();
    
  //  // Name of the minimizer
  std::string name() const override { return "RALFit"; }
  
   /// Initialize minimizer, i.e. pass a function to minimize.
  void initialize(API::ICostFunction_sptr function,
                  size_t maxIterations = 0) override;
    
  /// Do one iteration.
  bool iterate(size_t) override;
  
  /// Return current value of the cost function
  double costFunctionVal() override;

  /// finalize
  void finalize() override;
  /// sizes
  int n;
  int m;

  static RALFitMinimizer& getInstance()
  {
    static RALFitMinimizer instance;
    return instance;
  }

private:

  int hasConverged();

  //  evalF (as in TrustRegionMinimizer.h)
  friend int evalF(const int n,
		   const int m,
		   void const* params,
		   const DoubleFortranVector &x,
		   DoubleFortranVector &f);

  // f(x)
  DoubleFortranVector m_f;
  /// The Jacobian
  mutable JacobianImpl1 m_J;
  void * m_workspace;
  
  /// Stored cost function
  boost::shared_ptr<CostFunctions::CostFuncLeastSquares> m_leastSquares;
  /// Stored to access IFunction interface in iterate()
  API::IFunction_sptr m_function;
  /// Fitting data weights
  DoubleFortranVector m_weights;
  /// Fitting parameters
  DoubleFortranVector m_x;
  /// Stored data
  GSL_FitData *m_data;
  
  
  //  double m_xx[n];
 
  
  
  //  int hasConverged();

  int evalTEST() const;

  // RALFit stuff...
  struct ral_nlls_options m_options;
  struct ral_nlls_inform m_inform;

  
  void *ral_nlls_iterate;
  
}; // end of RALFitMinimizer class

  /*  int ralfit_f(const int n,
	       const int m,
	       void const* params,
	       const double *x,
	       double *f);
  */
  //  int ralfit_J();
  //  int ralfit_H();
  
  int evalF_classless (const int n,
		       const int m,
		       void const* params,
		       const DoubleFortranVector &x,
		       DoubleFortranVector &f);
  
} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid


int test_RALFit(const int n,
		const int m,
		void *classdata,
		void (*eval_r)(const int n,
			       const int m,
			       void const* params,
			       const double &x,
			       double &f));

/// Evaluate the fitting function and calculate the residuals.
/*
int evalF(const int n,
	  const int m,
	  const void *params,
	  const double *x,
	  double *f);
*/
/// Evaluate the Jacobian
/*int evalJ(const int n,
	  const int m,
	  const void *params,
	  const double *x,
	  double *J);
/// Evaluate the Hessian
int evalHf(const int n,
	   const int m,
	   const void *params,
	   const double *x,
	   const double *f,
	   double *Hf);
*/

#endif /*MANTID_CURVEFITTING_RALFITMINIMIZER_H_*/
