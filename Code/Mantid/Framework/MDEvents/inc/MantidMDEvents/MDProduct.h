#ifndef MANTID_MDEVENTS_MDPRODUCT_H_
#define MANTID_MDEVENTS_MDPRODUCT_H_

#include "MantidKernel/System.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidAPI/IFunctionMD.h"

namespace Mantid
{
namespace MDEvents
{

  /** MDProduct : TODO: DESCRIPTION
    
    Copyright &copy; 2014 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://github.com/mantidproject/mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
  */
  class DLLExport MDProduct: public API::IFunctionMD, public API::CompositeFunction
  {
  public:
    MDProduct();
    virtual ~MDProduct();
    std::string name() const {return "MDProduct";}
    void setWorkspace(boost::shared_ptr<const API::Workspace> ws);
    void function(const API::FunctionDomain& domain,API::FunctionValues& values)const;
    void functionDeriv(const API::FunctionDomain& domain, API::Jacobian& jacobian){calNumericalDeriv(domain,jacobian);}
  private:
    virtual double functionMD(const API::IMDIterator& r) const {return 0.0;}
    void calcMDIndex(size_t linearIndex, std::vector<size_t>& mdIndex) const;
    std::vector< std::vector<double> > m_functionCache;
    Mantid::Geometry::VecIMDDimension_const_sptr m_nonIntegDims;
    std::vector<size_t> m_dimIndex;
    std::vector<size_t> m_strides;
    
  };


} // namespace MDEvents
} // namespace Mantid

#endif  /* MANTID_MDEVENTS_MDPRODUCT_H_ */