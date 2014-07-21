#include "MantidMDAlgorithms/SmoothMDHisto.h"
#include "MantidAPI/IMDHistoWorkspace.h"
#include "MantidAPI/FunctionProperty.h"
#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/Axis.h"
#include "MantidAPI/CompositeFunction.h"

namespace Mantid
{

using namespace API;

namespace MDAlgorithms
{

  using Mantid::Kernel::Direction;

  // Register the algorithm into the AlgorithmFactory
  DECLARE_ALGORITHM(SmoothMDHisto)
  


  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  SmoothMDHisto::SmoothMDHisto()
  {
  }
    
  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  SmoothMDHisto::~SmoothMDHisto()
  {
  }
  

  //----------------------------------------------------------------------------------------------

  
  /// Algorithm's version for identification. @see Algorithm::version
  int SmoothMDHisto::version() const { return 1;};
  
  /// Algorithm's category for identification. @see Algorithm::category
  const std::string SmoothMDHisto::category() const { return "MDAlgorithms";}

  /// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
  const std::string SmoothMDHisto::summary() const { return "Smooth MD histo workspace";};

  //----------------------------------------------------------------------------------------------
  /** Initialize the algorithm's properties.
   */
  void SmoothMDHisto::init()
  {
    declareProperty(new WorkspaceProperty<IMDHistoWorkspace>("InputWorkspace","",Direction::Input), "An input workspace.");
    //declareProperty(new FunctionProperty("Function"),"Parameters defining the fitting function and its initial values");
    declareProperty("NIterations",10,"NUmber of iterations");
    declareProperty(new WorkspaceProperty<Workspace>("DiffWorkspace","",Direction::Output), "An output workspace.");
    declareProperty(new WorkspaceProperty<Workspace>("SumWorkspace","",Direction::Output), "An output workspace.");
  }

  //----------------------------------------------------------------------------------------------

  namespace // anonymous
  {
    void findMaxAbsValuePos(MatrixWorkspace_sptr ws, size_t &maxRow, size_t &maxCol, double &signedSqrtMax)
    {
      size_t nrows = ws->getNumberHistograms();
      size_t ncols = ws->blocksize();
      double absMax = 0.0;
      double sign = 1.0;
      maxRow = nrows;
      maxCol = ncols;
      
      for(size_t row = 0; row < nrows; ++row)
      {
        auto &Y = ws->readY(row);
        auto &E = ws->readE(row);
        for(auto it = Y.begin(); it != Y.end(); ++it)
        {
          double d = fabs( *it );
          if ( d > absMax )
          {
            auto col = static_cast<size_t>(std::distance(Y.begin(),it));
            double error = E[col];
            //if ( error > 0.0 && d > error )
            {
              absMax = d;
              if ( *it >= 0.0 )
              {
                sign = 1.0;
              }
              else
              {
                sign = -1.0;
              }
              maxRow = row;
              maxCol = col;
            }
          }
        }
      }
      signedSqrtMax = sign * sqrt( absMax );
    }

    IFunction_sptr fit( MatrixWorkspace_sptr ws, size_t wi, double absMax)
    {
      IFunction_sptr fun1 = FunctionFactory::Instance().createFunction("BSpline");
      fun1->setAttributeValue("NBreak", 30);
      fun1->setAttributeValue("StartX", ws->readX(0).front() );
      fun1->setAttributeValue("EndX", ws->readX(0).back() );

      Mantid::MantidVec &E = ws->dataE( wi );
      Mantid::MantidVec old(E);

      E.assign(E.size(), 1.0);

      auto fit = AlgorithmManager::Instance().create("Fit");
      fit->initialize();
      //fit->setChild(true);
      fit->setProperty("Function", fun1);
      fit->setProperty("InputWorkspace", ws);
      fit->setProperty("WorkspaceIndex", static_cast<int>(wi));
      fit->setPropertyValue("Output", "stuff"+boost::lexical_cast<std::string>(ws->getNumberHistograms()));
      fit->execute();

      for(size_t i = 0; i < fun1->nParams(); ++i)
      {
        double param = fun1->getParameter(i);
        fun1->setParameter( i, param / absMax );
      }

      std::swap(E,old);

      return fun1;
    }

    IFunction_sptr fit1( MatrixWorkspace_sptr ws, size_t wi, double absMax)
    {
      const double binWidth = ws->readX(0)[1] - ws->readX(0)[0];
      double startX = ws->readX(0).front();// + binWidth / 2;
      double endX = ws->readX(0).back();// - binWidth / 2;
      IFunction_sptr fun1 = FunctionFactory::Instance().createFunction("BSpline");
      fun1->setAttributeValue("NBreak", static_cast<int>(ws->blocksize()) - 1);
      fun1->setAttributeValue("StartX", startX );
      fun1->setAttributeValue("EndX", endX );

      auto &Y = ws->readY(wi);
      std::cerr << Y.size() << ' ' << fun1->nParams() << std::endl;
      assert( Y.size() == fun1->nParams() );
      for(size_t i = 0; i < fun1->nParams(); ++i)
      {
        fun1->setParameter( i, Y[i] / absMax );
      }

      return fun1;
    }

    MatrixWorkspace_sptr getColumn( MatrixWorkspace_sptr ws, size_t col)
    {
      size_t nx = ws->getNumberHistograms();
      MatrixWorkspace_sptr ws1 = WorkspaceFactory::Instance().create("Workspace2D",1,nx,nx);
      auto axis = ws->getAxis(1);
      std::cerr << axis->getValue(0) << ' ' << axis->getValue(nx-1) << std::endl;
      auto &X = ws1->dataX(0);
      auto &Y = ws1->dataY(0);
      auto &E = ws1->dataE(0);
      for(size_t i = 0; i < nx; ++i)
      {
        X[i] = axis->getValue(i);
        Y[i] = ws->readY(i)[col];
        E[i] = ws->readE(i)[col];
      }
      return ws1;
    }

    std::pair<API::IMDHistoWorkspace_sptr,API::IMDHistoWorkspace_sptr>
    iterate(API::IMDHistoWorkspace_sptr input, API::IMDHistoWorkspace_sptr sum, API::IMDHistoWorkspace_sptr diff)
    {

      if ( !diff ) diff = input;

      auto cloner = AlgorithmManager::Instance().create("ConvertMDHistoToMatrixWorkspace");
      cloner->initialize();
      cloner->setChild(true);
      cloner->setProperty("InputWorkspace", diff);
      //cloner->setPropertyValue("Normalization","NumEventsNormalization");
      cloner->setPropertyValue("OutputWorkspace", "_");
      cloner->execute();

      MatrixWorkspace_sptr ws = cloner->getProperty("OutputWorkspace");

      size_t maxRow = 0;
      size_t maxCol = 0;
      double signedSqrtMax = 0.0;
      findMaxAbsValuePos( ws, maxRow, maxCol, signedSqrtMax );

      double x = ws->readX(maxRow)[maxCol];
      double y = ws->getAxis(1)->getValue(maxRow);

      std::cerr << "Max " << x << ' ' << y << ' ' << signedSqrtMax*fabs(signedSqrtMax) << std::endl;

      if ( maxRow >= ws->getNumberHistograms() || maxCol >= ws->blocksize() )
      {
        std::cerr << "STOP!!! " << std::endl;
        return std::make_pair( input, API::IMDHistoWorkspace_sptr() );
      }

      auto fun1 = fit1(ws,maxRow, signedSqrtMax);
      auto fun2 = fit1( getColumn(ws,maxCol), 0, fabs(signedSqrtMax) );

      IFunction_sptr fun = FunctionFactory::Instance().createFunction("MDProduct");
      auto prod = boost::dynamic_pointer_cast<CompositeFunction>(fun);
      prod->addFunction( fun1 );
      prod->addFunction( fun2 );

      auto evaluator = AlgorithmManager::Instance().create("EvaluateMDFunction");
      evaluator->initialize();
      evaluator->setChild(true);
      evaluator->setProperty("InputWorkspace", input);
      evaluator->setProperty("Function", fun);
      evaluator->setPropertyValue("OutputWorkspace", "eval");
      evaluator->execute();

      API::IMDHistoWorkspace_sptr out = evaluator->getProperty("OutputWorkspace");

      if ( !sum )
      {
        sum = out;
      }
      else
      {
        auto plus = AlgorithmManager::Instance().create("PlusMD");
        plus->initialize();
        plus->setChild(true);
        plus->setProperty("LHSWorkspace", sum);
        plus->setProperty("RHSWorkspace", out);
        plus->setPropertyValue("OutputWorkspace", "diff");
        plus->execute();

        API::IMDWorkspace_sptr isum = plus->getProperty("OutputWorkspace");
        sum = boost::dynamic_pointer_cast<IMDHistoWorkspace>( isum );
        if ( !sum ) throw std::runtime_error("Wrong workspace type found.");
      }

      auto minus = AlgorithmManager::Instance().create("MinusMD");
      minus->initialize();
      minus->setChild(true);
      minus->setProperty("LHSWorkspace", input);
      minus->setProperty("RHSWorkspace", sum);
      minus->setPropertyValue("OutputWorkspace", "diff");
      minus->execute();

      API::IMDWorkspace_sptr idiff = minus->getProperty("OutputWorkspace");
      diff = boost::dynamic_pointer_cast<IMDHistoWorkspace>( idiff );

      if ( !diff ) throw std::runtime_error("Wrong workspace type found.");

      return std::make_pair( diff, sum );
    }

  }

  //----------------------------------------------------------------------------------------------
  /** Execute the algorithm.
   */
  void SmoothMDHisto::exec()
  {
    API::IMDHistoWorkspace_sptr input = getProperty("InputWorkspace");
    int niter = getProperty("NIterations");

    auto res = iterate( input, IMDHistoWorkspace_sptr(), IMDHistoWorkspace_sptr() );
    for(int i = 1; i < niter; ++i)
    {
      res = iterate( input, res.second, res.first );
    }

    setProperty("DiffWorkspace",res.first);
    setProperty("SumWorkspace",res.second);
  }



} // namespace MDAlgorithms
} // namespace Mantid