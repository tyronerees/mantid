#include "MantidCurveFitting/MDProduct.h"
#include "MantidAPI/IMDIterator.h"
#include "MantidAPI/FunctionDomainMD.h"
#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/IMDHistoWorkspace.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidGeometry/MDGeometry/IMDDimension.h"

namespace Mantid
{
namespace CurveFitting
{

using namespace API;

DECLARE_FUNCTION(MDProduct);

//----------------------------------------------------------------------------------------------
/** Constructor
  */
MDProduct::MDProduct()
{
}
    
//----------------------------------------------------------------------------------------------
/** Destructor
  */
MDProduct::~MDProduct()
{
}

namespace // anonymous
{ 

size_t calcStride(const IMDHistoWorkspace& workspace, size_t dim)
{
  size_t stride = 1;
  for(size_t i = 0; i < dim; ++i)
  {
    auto dimension = workspace.getDimension(i);
    stride *= dimension->getNBins();
  }
  return stride;
}

}
  
void MDProduct::setWorkspace(boost::shared_ptr<const API::Workspace> ws)
{
  IFunctionMD::setWorkspace( ws );

  IMDHistoWorkspace_const_sptr mdws = boost::dynamic_pointer_cast<const IMDHistoWorkspace>( ws );
  if ( !mdws ) throw std::invalid_argument("Expected IMDHistoWorkspace");

  m_nonIntegDims = mdws->getNonIntegratedDimensions();
  size_t nd = m_nonIntegDims.size();

  m_functionCache.resize( nd );
  m_dimIndex.resize( nd );
  for(size_t i = 0; i < nd; ++i)
  {
    auto dim = m_nonIntegDims[i];
    m_dimIndex[i] = mdws->getDimensionIndexById( dim->getDimensionId() );
    size_t n = dim->getNBins();
    double binWidh = dim->getBinWidth();
    double x0 = dim->getMinimum() + binWidh / 2;
    std::vector<double> &y = m_functionCache[i];
    y.resize(n);
    std::vector<double> x(n);
    for(size_t j = 0; j < n; ++j)
    {
      x[j] = x0 + j * binWidh;
    }
    FunctionDomain1DView domain( x.data(), x.size() );
    FunctionValues values( domain );
    getFunction( i )->function( domain, values );
    y.assign( values.getPointerToCalculated(0), values.getPointerToCalculated(0) + n );
  }

  size_t ND = mdws->getNumDims();
  m_strides.resize( ND );
  for(size_t i = 0; i < ND; ++i)
  {
    m_strides[i] = calcStride( *mdws, i );
  }

}

void MDProduct::function(const API::FunctionDomain& domain,API::FunctionValues& values)const
{
    size_t n = m_nonIntegDims.size();
    if ( n != 2 )
    {
      throw std::invalid_argument("Function works in 2 dimensions only.");
    }

    if ( nFunctions() != n )
    {
      throw std::runtime_error("MDProduct has different dimensionality than domain.");
    }

    const API::FunctionDomainMD* dmd = dynamic_cast<const API::FunctionDomainMD*>(&domain);
    if (!dmd)
    {
      throw std::invalid_argument("Unexpected domain in IFunctionMD");
    }

    std::vector<size_t> mdIndex(m_strides.size());
    dmd->reset();
    size_t ivalue = 0;
    for(const API::IMDIterator* r = dmd->getNextIterator(); r != NULL; r = dmd->getNextIterator(),++ivalue)
    {
      this->reportProgress("Evaluating function for box " + boost::lexical_cast<std::string>(ivalue+1));
      double v = 1.0;
      //if ( r->getError() == 0.0 )
      //{
      //  v = 0.0;
      //}
      //else
      {
        calcMDIndex( r->getLinearIndex(), mdIndex );
        size_t ifun = 0;
        for(auto idim = m_dimIndex.begin(); idim != m_dimIndex.end(); ++idim, ++ifun)
        {
          size_t i = mdIndex[ *idim ];
          v *= m_functionCache[ifun][i];
        }
      }
      values.setCalculated(ivalue,v);
    };
}

void MDProduct::calcMDIndex(size_t linearIndex, std::vector<size_t>& mdIndex) const
{
  size_t index = linearIndex;
  auto mdit = mdIndex.rbegin();
  for(auto stride = m_strides.rbegin(); stride != m_strides.rend(); ++stride, ++mdit)
  {
    *mdit = index / *stride;
    index %= *stride;
  }
}

} // namespace CurveFitting
} // namespace Mantid