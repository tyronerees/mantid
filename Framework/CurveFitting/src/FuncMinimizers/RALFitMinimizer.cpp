
//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidCurveFitting/FuncMinimizers/RALFitMinimizer.h"
#include "MantidAPI/ICostFunction.h"
#include "MantidAPI/IConstraint.h"

#include "MantidAPI/CostFunctionFactory.h"
#include "MantidAPI/FuncMinimizerFactory.h"
#include "MantidCurveFitting/GSLFunctions.h"
#include "MantidCurveFitting/Jacobian.h"

#include "MantidKernel/Logger.h"



namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {
  /*namespace{
  /// static logger object
  Kernel::Logger g_log("RALFitMinimizer");
  }*/

  // clang-format off
  DECLARE_FUNCMINIMIZER(RALFitMinimizer,RALFit)
  // clang-format on
  
  /*  /// Evaluate the fitting function and calculate the residuals.
  /// @param n :: The number of parameters to be fitted
  /// @param m :: The length of the data
  /// @param param :: any parameters to be passed in?
  /// @param x :: The fitting parameters as a fortran 1d array.
  /// @param f :: The output fortran vector with the weighted residuals.
  int TrustRegionMinimizer::evalF(const int n,
	    const int m,
	    const void *params,
	    const DoubleFortranVector *x,
	    DoubleFortranVector *f) {
    UNUSED_ARG(n);


    boost::shared_ptr<CostFunctions::CostFuncLeastSquares> m_leastSquares;
    m_leastSquares->Mantid::CurveFitting::FuncMinimizers::setParameters(*x);
    auto &domain = *m_leastSquares->getDomain();
    auto &values = *m_leastSquares->getValues();
    m_function->function(domain, values);
    if (f->len() != m) {  // changed . for -> (check, and below!)
      f->allocate(m); 
    }
    for (int i = 0; i < m; ++i) {
      f->set(i, (values.getCalculated(i) - values.getFitData(i))) ;
	     // values.getFitWeight(i)); remove the weights...
    }

    return 0;
  }
  */



  
  //Evaluate the Jacobian
  /// @param n :: The number of parameters to be fitted
  /// @param m :: The length of the data
  /// @param param :: any parameters to be passed in?
  /// @param x :: The fitting parameters as a fortran 1d array.
  /// @param J :: The output fortran matrix with the weighted Jacobian.
  /*  int evalJ(const int n,
			     const int m,
			     const void *params,
			     const double *x,
			     double *J) {
    UNUSED_ARG(params); 

    // / *
    m_leastSquares->setParameters(*x);
    auto &domain = *m_leastSquares->getDomain();
    //    auto &values = *m_leastSquares->getValues();
    if (J->len1() != m || J->len2() != n) {
      J->allocate(m, n);
    }
    m_J.setJ(J->gsl());
    m_function->functionDeriv(domain, m_J); // what is this?!?
    for (int i = 1; i <= m; ++i) {
      double w = values.getFitWeight(i - 1);
      for (int j = 1; j <= n; ++j) {
	J(i, j) *= w;
      }
    }
  // * /
    return 0;
  }
  */

  //Evaluate the Hessian
  /// @param n :: The number of parameters to be fitted
  /// @param m :: The length of the data
  /// @param param :: any parameters to be passed in?
  /// @param x :: The fitting parameters as a fortran 1d array.
  /// @param f :: The fortran vector of residuals
  /// @param Hf :: The output fortran matrix with the weighted Jacobian.
  int ralfit_Hf(const int n,
		const int m,
		const void *params,
		const double *x,
		const double *f,
		double *Hf) {
    UNUSED_ARG(n);
    UNUSED_ARG(m);
    UNUSED_ARG(params);
    UNUSED_ARG(x);
    UNUSED_ARG(f);
    //    if (Hf->len1() !=n || Hf->len2() != n) {
    //         Hf->allocate(n,n);
    //    }
    // and set to zero for now...
    //    Hf->zero();
  
    return 0;
  }
  

  int ralfit_f(const int n,
	       const int m,
	       const void * params,
	       const double *x,
	       double *f){

    // inspired by gsl_f

    printf("In ralfit_f!\n");
    for (int i = 0; i < m; i++){
      f[i] = std::numeric_limits<double>::quiet_NaN();
    }
    
    const struct GSL_FitData *p = reinterpret_cast<const struct GSL_FitData *>(params);

    auto values = boost::dynamic_pointer_cast<API::FunctionValues>(p->costFunction->getValues());
    if (!values) {
      throw std::invalid_argument("FunctionValues expected");
    }
    p->function->function(*p->costFunction->getDomain(), *values);

    for (size_t i = 0; i < m; i++) {
      f[i] = (values->getCalculated(i) - values->getFitData(i));
	// * values->getFitWeight(i);  // Weights not needed -- handled by RALFit
    } 

    /* This seems to apply to bound constrained problems only....
    
    // update function parameters
    size_t ia = 0;
    for (size_t i = 0; i < p->function->nParams(); ++i) {
      if (p->function->isActive(i)) {
	p->function->setActiveParameter(i, x[ia]);
	++ia;
      }
    }
    p->function->applyTies();
    
    auto values = boost::dynamic_pointer_cast<API::FunctionValues>(p->costFunction->getValues());
    if (!values) {
      throw std::invalid_argument("FunctionValues expected");
    }
    p->function->function(*p->costFunction->getDomain(), *values);

    // Add penalty
    double penalty = 0.;
    for (size_t i = 0; i < p->function->nParams(); ++i) {
      API::IConstraint *c = p->function->getConstraint(i);
      if (c) {
	penalty += c->checkDeriv();
      }
    }


    // add penalty to first and last point and every 10th point in between
    if (penalty != 0.0) {
      values->addToCalculated(0, penalty);
      values->addToCalculated(n-1, penalty);
      
      for (size_t i = 9; i < n; i += 10) {
	values->addToCalculated(i, penalty);
      }
    }
    
    // function() return calculated data values. Need to convert this values into
    // calculated-observed devided by error values used by GSL


    printf("m = %d, p->n = %d\n",m,p->n);
    for (size_t i = 0; i < m; i++) {
      f[i] = (values->getCalculated(i) - values->getFitData(i));
	// * values->getFitWeight(i);  // Weights not needed -- handled by RALFit
    } 
    //

    */
  return 0;
    
  }


  int ralfit_J(const int n,
	       const int m,
	       const void * params,
	       const double *x,
	       double *J){


    const struct GSL_FitData *p = reinterpret_cast<const struct GSL_FitData *>(params);

    //    mutable JacobianImpl1 m_J;
    //    m_J.setJ(J)
    //    J_jac.setJ(J_jac);
    /*    p->J.setJ(J);

    // update function parameters
    size_t ia = 0;
    for (size_t i = 0; i < p->function->nParams(); ++i) {
      if (p->function->isActive(i)) {
	p->function->setActiveParameter(i, x->data[ia]);
	++ia;
      }
    }
  
    p->function->applyTies();
    */
    // calculate the Jacobian
    //    p->function->functionDeriv(*p->costFunction->getDomain(), J);

    // p->function->addPenaltyDeriv(&p->J);
    // add penalty
    //    size_t n = p->costFunction->getValues()->size() - 1;
    /*    size_t ia = 0;
    for (size_t i = 0; i < n; ++i) {
      if (!p->function->isActive(i))
	continue;
      
      API::IConstraint *c = p->function->getConstraint(i);
      if (c) {
	double penalty = c->checkDeriv2();
	if (penalty != 0.0) {
	  double deriv = p->J.get(0, ia);
	  p->J.set(0, ia, deriv + penalty);
	  deriv = p->J.get(n, ia);
	  p->J.set(n, ia, deriv + penalty);
	  
	  for (size_t j = 9; j < n; j += 10) {
	    deriv = p->J.get(j, ia);
	    p->J.set(j, ia, deriv + penalty);
	  }
	}
      } // if (c)
      ++ia;
    }
    */
  // functionDeriv() return derivatives of calculated data values. Need to
  // convert this values into
  // derivatives of calculated-observed divided by error values used by GSL
  auto values = boost::dynamic_pointer_cast<API::FunctionValues>(
      p->costFunction->getValues());
  if (!values) {
    throw std::invalid_argument("FunctionValues expected");
  }
  for (size_t iY = 0; iY < m; iY++)
    for (size_t iP = 0; iP < n; iP++) {
      J[iP*m + iY] *= values->getFitWeight(iY);
      //      J->data[iY * p->p + iP] *= values->getFitWeight(iY);
      // std::cerr << iY << ' ' << iP << ' ' << J->data[iY*p->p + iP] <<
      // '\n';
    }

  return 0;
  }

  RALFitMinimizer::RALFitMinimizer() : m_function(){
    declareProperty("InitialRadius", 100.0,
		    "Initial radius of the trust region");
  }
  
  void RALFitMinimizer::initialize(API::ICostFunction_sptr costFunction,
				   size_t maxIterations){
    m_leastSquares =
      boost::dynamic_pointer_cast<CostFunctions::CostFuncLeastSquares>(costFunction);
    
    if (m_leastSquares) {
      m_data = new GSL_FitData(m_leastSquares);
    } else {
      throw std::runtime_error("RALFit can only be used with Least "
			       "squares cost function.");
    }

    m_function = m_leastSquares->getFittingFunction();
    auto &values = *m_leastSquares->getValues();
    n = static_cast<int>(m_leastSquares->nParams());
    m = static_cast<int>(values.size());
    if (n > m) {
      throw std::runtime_error("More parameters than data.");
    }
        
    // initialize options values
    struct ral_nlls_options m_options;
    ral_nlls_default_options(&m_options);
    m_options.initial_radius  = getProperty("InitialRadius");
    m_options.maxit = static_cast<int>(maxIterations);
    printf("options.model = %d\n",m_options.model);
    
    // initialize the workspace
    ral_nlls_init_workspace(&m_workspace);

    // initialize the vector that will hold the solution
    m_x.allocate(n);
    m_leastSquares->getParameters(m_x);

    // allocate space for the weights.
    m_weights.allocate(n);

    //    double *m_xx = (double *) malloc(n*sizeof(double));
    for (int i = 0; i < n; i++){
      //      m_xx[i] = m_x[i];
      // TODO!!
      // do not leave this like this
      // (at least, not without thinking about it and checking
      //  that all is ok...)
      printf("x[%d] = %f\n",i,m_x[i]);
    }
    
    printf("n = %d, m = %d\n", n,m);

      
    /* 
     // This code is in TrustRegionMinimizer, but I'm not sure what
     // it does....
     int j = 0;	 
     for (size_t i = 0; i < m_function->nParams(); ++i) {
      if (m_function->isActive(i)) {
	m_J.m_index.push_back(j);
	j++;
      } else
	m_J.m_index.push_back(-1);
      }
    */

    //todo: allow the user to change these...
  }


  /* Define the functions that evaluate:
     + the function
     + the Jacobian
     + the Hessian (eventually)
  */

  /*  int test_RALFit(const int n,
		  const int m,
		  void *classdata,
		  void (*eval_r)(const int n,
				 const int m,
				 void const* params,
				 const double &x,
				 double &f)) {

    UNUSED_ARG(n);
    UNUSED_ARG(m);
    UNUSED_ARG(eval_r);
    printf("I'm in test_RALFit");
    
    
    return 0;
  }
  */


  int evalF_classless(int n,
		      int m,
		      void const* params,
		      const DoubleFortranVector &x,
		      DoubleFortranVector &f)  {

    UNUSED_ARG(n);
    UNUSED_ARG(params);
    
    

    /*
    m_leastSquares->setParameters(x);
    auto &domain = *m_leastSquares->getDomain();
    auto &values = *m_leastSquares->getValues();
    m_function->function(domain, values);
    
    
    //    int m = static_cast<int>(values.size());
    if (f.len() != m) {
      f.allocate(m);
    }
    for (size_t i = 0; i < values.size(); ++i) {
      f.set(i, (values.getCalculated(i) - values.getFitData(i)) *
	    values.getFitWeight(i));
    }

    */
    return 0;
    
  }
  

  /*
    int evalF(int n,
	    int m,
	    void const* params,
	    const DoubleFortranVector &x,
	    DoubleFortranVector &f) {

    UNUSED_ARG(n);
    UNUSED_ARG(params);

    RALFitMinimizer* ralfit = (RALFitMinimizer*) params;
    
    m_leastSquares = ralfit->boost::dynamic_pointer_cast<CostFunctions::CostFuncLeastSquares>(
											      costFunction);//setParameters(x);
    auto &domain = *m_leastSquares->getDomain();
    auto &values = *m_leastSquares->getValues();
    m_function->function(domain, values);
    
    
    //    int m = static_cast<int>(values.size());
    if (f.len() != m) {
      f.allocate(m);
    }
    for (size_t i = 0; i < values.size(); ++i) {
      f.set(i, (values.getCalculated(i) - values.getFitData(i)) *
	    values.getFitWeight(i));
    }

    return 0;
    
  }

  */
  
  bool RALFitMinimizer::iterate(size_t) {

    printf("**************************\n");
    printf("n = %d\n", n);
    for (int i = 0; i < n; i++){
      printf("x[%d] = %f\n",i,m_x[i]);
    }
    printf("options.model = %d\n",m_options.model);

    RALFitMinimizer classinstance = getInstance();

    printf("n = %d, m = %d\n",n,m);
    double *m_xx = (double *) malloc(n*sizeof(double));
    for (int i = 0; i < n; i++){
      m_xx[i] = m_x[i];
      // TODO!!
      // do not leave this like this
      // (at least, not without thinking about it and checking
      //  that all is ok...)
      printf("x[%d] = %f\n",i,m_x[i]);
    }


    // Now let's check that I can call eval_f?
    double *m_f = (double *) malloc(m*sizeof(double));
    printf("Checking that I can call eval_f\n");
    ralfit_f(n,m,m_data,m_xx,m_f);
    printf("done!\n");

    printf("Checking that I can call eval_J\n");
    ralfit_J(n,m,m_data,m_xx,m_J);
    printf("done!\n");
    
    
    //    ral_nlls_test(n,m,m_xx,ralfit_f,m_data);
    
    /*  int status = ral_nlls_iterate( n, m,
				   m_x, // solution
				   m_workspace,
				   ralfit_f,
				   ralfit_J,
				   ralfit_Hf,
				   &classinstance,//params
				   &m_options,
				   &m_inform,
				   m_weights); // weights
    */

    //    int status =
    /*  ral_nlls_iterate( n, m,
		      m_x, // solution
		      m_workspace,
		      ralfit_f,
		      ralfit_J,
		      ralfit_Hf,
		      &classinstance,//params
		      &m_options,
		      &m_inform,
		      m_weights); // weights
    */

    double x_unwrapped[n];
    for (int i=0; i<n; i++){
      x_unwrapped[i] = m_x.get(i);
    }

    printf("options.model = %d\n",m_options.model);

    
    /*
nlls_solve(n,m,
	       x_unwrapped,
	       ralfit_f,
	       ralfit_J,
	       ralfit_Hf,
	       m_data,
	       &m_options,
	       &m_inform,
	       NULL);
    */
    
    ///	       m_weights);
    
    //    dummyIterate(evalF);
    
    
    if (m_inform.status < 0) return false;

    return true;

  }

  double RALFitMinimizer::costFunctionVal() {
    return 0.0;//inform.obj; // check that this is actually what I think it is...
  }

  void RALFitMinimizer::finalize() {

    // free the workspace
    void ral_nlls_free_workspace(void **m_workspace);
  }
  
} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid
