#ifndef CONVERTTABLETOMATRIXWORKSPACETEST_H_
#define CONVERTTABLETOMATRIXWORKSPACETEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAlgorithms/ConvertTableToMatrixWorkspace.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/TableRow.h"
#include "MantidKernel/Unit.h"

using namespace Mantid;
using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::Algorithms;

class ConvertTableToMatrixWorkspaceTest : public CxxTest::TestSuite
{
public:
  void testName()
  {
    TS_ASSERT_EQUALS( m_converter->name(), "ConvertTableToMatrixWorkspace" )
  }

  void testVersion()
  {
    TS_ASSERT_EQUALS( m_converter->version(), 1 )
  }

  void testInit()
  {
    TS_ASSERT_THROWS_NOTHING( m_converter->initialize() )
    TS_ASSERT( m_converter->isInitialized() )
  }


  void testExec()
  {
    
    ITableWorkspace_sptr tws = WorkspaceFactory::Instance().createTable();
    tws->addColumn("int","A");
    tws->addColumn("double","B");
    tws->addColumn("double","C");

    size_t n = 10;
    for (size_t i = 0; i < n; ++i)
    {
      TableRow row = tws->appendRow();
      int x = int(i);
      double y = x * 1.1;
      double e = sqrt(y);
      row << x << y << e;
    }


    TS_ASSERT_THROWS_NOTHING( m_converter->setProperty("InputWorkspace",tws) );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("OutputWorkspace","out") );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("ColumnX","A") );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("ColumnY","B") );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("ColumnE","C") );

    TS_ASSERT( m_converter->execute() );

    MatrixWorkspace_sptr mws = boost::dynamic_pointer_cast<MatrixWorkspace>(
    API::AnalysisDataService::Instance().retrieve("out"));

    TS_ASSERT( mws );
    TS_ASSERT_EQUALS( mws->getNumberHistograms() , 1);
    TS_ASSERT( !mws->isHistogramData() );
    TS_ASSERT_EQUALS( mws->blocksize(), tws->rowCount() );

    const Mantid::MantidVec& X = mws->readX(0);
    const Mantid::MantidVec& Y = mws->readY(0);
    const Mantid::MantidVec& E = mws->readE(0);

    for(size_t i = 0; i < tws->rowCount(); ++i)
    {
      TableRow row = tws->getRow(i);
      int x;
      double y,e;
      row >> x >> y >> e;
      TS_ASSERT_EQUALS( double(x), X[i] );
      TS_ASSERT_EQUALS( y, Y[i] );
      TS_ASSERT_EQUALS( e, E[i] );
    }

    boost::shared_ptr<Units::Label> label = boost::dynamic_pointer_cast<Units::Label>(mws->getAxis(0)->unit());
    TS_ASSERT(label);
    TS_ASSERT_EQUALS(label->caption(), "A");
    TS_ASSERT_EQUALS(mws->YUnitLabel(), "B");

    API::AnalysisDataService::Instance().remove("out");
  }

  void test_Default_ColumnE()
  {
    size_t n = 10;
    for (size_t i = 0; i < n; ++i)
    {
      TableRow row = m_tws1->appendRow();
      double x = double(i);
      double y = x * 1.1;
      row << x << y;
    }

    TS_ASSERT( m_converter->execute() );

    MatrixWorkspace_sptr mws = AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>("out");

    TS_ASSERT( mws );
    TS_ASSERT_EQUALS( mws->getNumberHistograms() , 1);
    TS_ASSERT( !mws->isHistogramData() );
    TS_ASSERT_EQUALS( mws->blocksize(), m_tws1->rowCount() );

    const Mantid::MantidVec& X = mws->readX(0);
    const Mantid::MantidVec& Y = mws->readY(0);
    const Mantid::MantidVec& E = mws->readE(0);

    for(size_t i = 0; i < m_tws1->rowCount(); ++i)
    {
      TableRow row = m_tws1->getRow(i);
      double x,y;
      row >> x >> y;
      TS_ASSERT_EQUALS( double(x), X[i] );
      TS_ASSERT_EQUALS( y, Y[i] );
      TS_ASSERT_EQUALS( 0.0, E[i] );
    }

    API::AnalysisDataService::Instance().remove("out");
  }

  void test_fail_on_empty_table()
  {
    TS_ASSERT_THROWS( m_converter->execute(), std::runtime_error );
  }

  void test_allowed_values_change_when_workspace_changes()
  {
    ConvertTableToMatrixWorkspace convert;
    convert->setRethrows(true);
    convert->initialize();

    TS_ASSERT_THROWS_NOTHING( convert->setProperty("InputWorkspace", m_tws1) );
    auto columnXAllowedValues = convert->getProperty("ColumnX").allowedValues();
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "A") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "B") != columnXAllowedValues.end());
    TS_ASSERT_EQUALS(columnXAllowedValues.size(), 2);
    auto columnEAllowedValues = convert->getProperty("ColumnE").allowedValues();
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "A") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "B") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "") != columnXAllowedValues.end());
    TS_ASSERT_EQUALS(columnXAllowedValues.size(), 2);

    TS_ASSERT_THROWS_NOTHING( convert->setProperty("InputWorkspace", m_tws2) );
    auto columnXAllowedValues = convert->getProperty("ColumnX").allowedValues();
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "X") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "Y") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "E") != columnXAllowedValues.end());
    TS_ASSERT_EQUALS(columnXAllowedValues.size(), 3);
    auto columnEAllowedValues = convert->getProperty("ColumnE").allowedValues();
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "X") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "Y") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "E") != columnXAllowedValues.end());
    TS_ASSERT(std::find(columnXAllowedValues.begin(), columnXAllowedValues.end(), "") != columnXAllowedValues.end());
    TS_ASSERT_EQUALS(columnXAllowedValues.size(), 4);
  }

  void setUp()
  {
    m_tws1 = WorkspaceFactory::Instance().createTable();
    m_tws1->addColumn("double","A");
    m_tws1->addColumn("double","B");

    m_converter = boost::make_shared<Mantid::Algorithms::ConvertTableToMatrixWorkspace>();
    m_converter->setRethrows(true);
    m_converter->initialize();
    TS_ASSERT_THROWS_NOTHING( m_converter->setProperty("InputWorkspace",m_tws1) );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("OutputWorkspace","out") );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("ColumnX","A") );
    TS_ASSERT_THROWS_NOTHING( m_converter->setPropertyValue("ColumnY","B") );
    
    m_tws2 = WorkspaceFactory::Instance().createTable();
    m_tws2->addColumn("double","X");
    m_tws2->addColumn("double","Y");
    m_tws2->addColumn("double","E");
  }

private:
    IAlgorithm_sptr m_converter;
    ITableWorkspace_sptr m_tws1;
    ITableWorkspace_sptr m_tws2;
};

#endif /*CONVERTTABLETOMATRIXWORKSPACETEST_H_*/
