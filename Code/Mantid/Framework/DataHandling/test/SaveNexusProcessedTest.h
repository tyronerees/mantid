#ifndef SAVENEXUSPROCESSEDTEST_H_
#define SAVENEXUSPROCESSEDTEST_H_

// These includes seem to make the difference between initialization of the
// workspace names (workspace2D/1D etc), instrument classes and not for this test case.
#include "MantidDataObjects/WorkspaceSingleValue.h"
#include "MantidDataHandling/LoadInstrument.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/TableRow.h"
#include "MantidAPI/ScopedWorkspace.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataHandling/LoadEventPreNexus.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidDataHandling/SaveNexusProcessed.h"
#include "MantidDataHandling/LoadMuonNexus.h"
#include "MantidDataHandling/LoadNexus.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidDataHandling/LoadRaw3.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"
#include <Poco/File.h>
#include <Poco/Path.h>

#include <nexus/NeXusFile.hpp>

#include <fstream>
#include <cxxtest/TestSuite.h>


using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::DataHandling;
using namespace Mantid::DataObjects;
using namespace Mantid::NeXus;

class SaveNexusProcessedTest : public CxxTest::TestSuite
{
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static SaveNexusProcessedTest *createSuite() { return new SaveNexusProcessedTest(); }
  static void destroySuite( SaveNexusProcessedTest *suite ) { delete suite; }

  SaveNexusProcessedTest()
  {
    // clearfiles - make true for SVN as dont want to leave on build server.
    // Unless the file "KEEP_NXS_FILES" exists, then clear up nxs files
    Poco::File file("KEEP_NXS_FILES");
    clearfiles = !file.exists();
  }

  void setUp()
  {

  }

  void tearDown()
  {
  }


  void testInit()
  {
    SaveNexusProcessed algToBeTested;
    TS_ASSERT_THROWS_NOTHING(algToBeTested.initialize());
    TS_ASSERT( algToBeTested.isInitialized() );
  }


  void testExec()
  {

    SaveNexusProcessed algToBeTested;
    if ( !algToBeTested.isInitialized() ) algToBeTested.initialize();

    // Should fail because mandatory parameter has not been set
    TS_ASSERT_THROWS(algToBeTested.execute(),std::runtime_error);


    // create dummy 2D-workspace
    Workspace2D_sptr localWorkspace2D = boost::dynamic_pointer_cast<Workspace2D>
    (WorkspaceFactory::Instance().create("Workspace2D",1,10,10));
    localWorkspace2D->getAxis(0)->unit() = UnitFactory::Instance().create("TOF");
    double d = 0.0;
    for(int i = 0; i<10; ++i,d+=0.1)
    {
      localWorkspace2D->dataX(0)[i] = d;
      localWorkspace2D->dataY(0)[i] = d;
      localWorkspace2D->dataE(0)[i] = d;
    }

    AnalysisDataService::Instance().addOrReplace("testSpace", localWorkspace2D);

    // Now set it...
    // specify name of file to save workspace to
    algToBeTested.setPropertyValue("InputWorkspace", "testSpace");
    outputFile = "SaveNexusProcessedTest_testExec.nxs";
    //entryName = "test";
    dataName = "spectra";
    title = "A simple workspace saved in Processed Nexus format";
    TS_ASSERT_THROWS_NOTHING(algToBeTested.setPropertyValue("Filename", outputFile));
    outputFile = algToBeTested.getPropertyValue("Filename");
    //algToBeTested.setPropertyValue("EntryName", entryName);
    algToBeTested.setPropertyValue("Title", title);
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();

    std::string result;
    TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("Filename") );
    TS_ASSERT( ! result.compare(outputFile));
    //TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("EntryName") )
    //TS_ASSERT( ! result.compare(entryName));

    // changed so that 1D workspaces are no longer written.
    TS_ASSERT_THROWS_NOTHING(algToBeTested.execute());
    TS_ASSERT( algToBeTested.isExecuted() );

    if(clearfiles) Poco::File(outputFile).remove();

    AnalysisDataService::Instance().remove("testSpace");


  }



  void testExecOnLoadraw()
  {
    SaveNexusProcessed algToBeTested;
    std::string inputFile = "LOQ48127.raw";
    TS_ASSERT_THROWS_NOTHING( loader.initialize());
    TS_ASSERT( loader.isInitialized() );
    loader.setPropertyValue("Filename", inputFile);

    outputSpace = "outer4";
    loader.setPropertyValue("OutputWorkspace", outputSpace);

    TS_ASSERT_THROWS_NOTHING(loader.execute());
    TS_ASSERT( loader.isExecuted() );

    //
    // get workspace
    //
    Workspace_sptr output;
    TS_ASSERT_THROWS_NOTHING(output = AnalysisDataService::Instance().retrieve(outputSpace));
    Workspace2D_sptr output2D = boost::dynamic_pointer_cast<Workspace2D>(output);
    //
    if ( !algToBeTested.isInitialized() ) algToBeTested.initialize();

    algToBeTested.setPropertyValue("InputWorkspace", outputSpace);
    // specify name of file to save workspace to
    outputFile = "SaveNexusProcessedTest_testExecOnLoadraw.nxs";
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();
    //entryName = "entry4";
    dataName = "spectra";
    title = "A save of a workspace from Loadraw file";
    algToBeTested.setPropertyValue("Filename", outputFile);

    //algToBeTested.setPropertyValue("EntryName", entryName);
    algToBeTested.setPropertyValue("Title", title);
    algToBeTested.setPropertyValue("Append", "0");
    outputFile = algToBeTested.getPropertyValue("Filename");
    std::string result;
    TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("Filename") );
    TS_ASSERT( ! result.compare(outputFile));
    //TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("EntryName") );
    //TS_ASSERT( ! result.compare(entryName));

    TS_ASSERT_THROWS_NOTHING(algToBeTested.execute());
    TS_ASSERT( algToBeTested.isExecuted() );

    if(clearfiles) remove(outputFile.c_str());
    TS_ASSERT_THROWS_NOTHING(AnalysisDataService::Instance().remove(outputSpace));

  }


  void testExecOnMuon()
  {
    SaveNexusProcessed algToBeTested;

    LoadNexus nxLoad;
    std::string outputSpace,inputFile;
    nxLoad.initialize();
    // Now set required filename and output workspace name
    inputFile = "emu00006473.nxs";
    nxLoad.setPropertyValue("Filename", inputFile);
    outputSpace="outer";
    nxLoad.setPropertyValue("OutputWorkspace", outputSpace);
    //
    // Test execute to read file and populate workspace
    //
    TS_ASSERT_THROWS_NOTHING(nxLoad.execute());
    TS_ASSERT( nxLoad.isExecuted() );
    //
    // get workspace
    //
    Workspace_sptr output;
    TS_ASSERT_THROWS_NOTHING(output = AnalysisDataService::Instance().retrieve(outputSpace));
    Workspace2D_sptr output2D = boost::dynamic_pointer_cast<Workspace2D>(output);
    // this would make all X's separate
    // output2D->dataX(22)[3]=0.55;
    //
    if ( !algToBeTested.isInitialized() ) algToBeTested.initialize();

    algToBeTested.setPropertyValue("InputWorkspace", outputSpace);
    // specify name of file to save workspace to
    outputFile = "SaveNexusProcessedTest_testExecOnMuon.nxs";
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();
    //entryName = "entry4";
    dataName = "spectra";
    title = "A save of a 2D workspace from Muon file";
    algToBeTested.setPropertyValue("Filename", outputFile);
    outputFile = algToBeTested.getPropertyValue("Filename");
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();

    //algToBeTested.setPropertyValue("EntryName", entryName);
    algToBeTested.setPropertyValue("Title", title);
    algToBeTested.setPropertyValue("Append", "0");

    std::string result;
    TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("Filename") );
    TS_ASSERT( ! result.compare(outputFile));
    //TS_ASSERT_THROWS_NOTHING( result = algToBeTested.getPropertyValue("EntryName") );
    //TS_ASSERT( ! result.compare(entryName));

    TS_ASSERT_THROWS_NOTHING(algToBeTested.execute());
    TS_ASSERT( algToBeTested.isExecuted() );

    // Nice idea, but confusing (seg-faulted) if algorithm doesn't clean its state
    // In reality out algorithms are only call once
    //    // try writing data again
    //    TS_ASSERT_THROWS_NOTHING(algToBeTested.execute());
    //    TS_ASSERT( algToBeTested.isExecuted() );
    if(clearfiles) Poco::File(outputFile).remove();
    TS_ASSERT_THROWS_NOTHING(AnalysisDataService::Instance().remove(outputSpace));
  }

  /**
   *
   * @param filename_root :: base of the file to save
   * @param type :: event type to create
   * @param outputFile[out] :: returns the output file
   * @param makeDifferentTypes :: mix event types
   * @param clearfiles :: clear files after saving
   * @param PreserveEvents :: save as event list
   * @param CompressNexus :: compress
   * @return
   */
  static EventWorkspace_sptr do_testExec_EventWorkspaces(std::string filename_root, EventType type,
      std::string & outputFile,  bool makeDifferentTypes, bool clearfiles,
      bool PreserveEvents=true, bool CompressNexus=false)
  {
    std::vector< std::vector<int> > groups(5);
    groups[0].push_back(10);
    groups[0].push_back(11);
    groups[0].push_back(12);
    groups[1].push_back(20);
    groups[2].push_back(30);
    groups[2].push_back(31);
    groups[3].push_back(40);
    groups[4].push_back(50);

    EventWorkspace_sptr WS = WorkspaceCreationHelper::CreateGroupedEventWorkspace(groups, 100, 1.0);
    WS->getEventList(3).clear(false);
    // Switch the event type
    if (makeDifferentTypes)
    {
      WS->getEventList(0).switchTo(TOF);
      WS->getEventList(1).switchTo(WEIGHTED);
      WS->getEventList(2).switchTo(WEIGHTED_NOTIME);
      WS->getEventList(4).switchTo(WEIGHTED);
    }
    else
    {
      for (size_t wi=0; wi < WS->getNumberHistograms(); wi++)
        WS->getEventList(wi).switchTo(type);
    }

    SaveNexusProcessed alg;
    alg.initialize();

    // Now set it...
    alg.setProperty("InputWorkspace", boost::dynamic_pointer_cast<Workspace>(WS));

    // specify name of file to save workspace to
    std::ostringstream mess;
    mess << filename_root << static_cast<int>(type) << ".nxs";
    outputFile = mess.str();
    std::string dataName = "spectra";
    std::string title = "A simple workspace saved in Processed Nexus format";

    alg.setPropertyValue("Filename", outputFile);
    outputFile = alg.getPropertyValue("Filename");
    alg.setPropertyValue("Title", title);
    alg.setProperty("PreserveEvents", PreserveEvents);
    alg.setProperty("CompressNexus", CompressNexus);

    // Clear the existing file, if any
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();
    alg.execute();
    TS_ASSERT( alg.isExecuted() );

    TS_ASSERT( Poco::File(outputFile).exists() );

    if(clearfiles) Poco::File(outputFile).remove();

    return WS;
  }


  void testExec_EventWorkspace_TofEvent()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_", TOF, outputFile, false, clearfiles);
  }

  void testExec_EventWorkspace_WeightedEvent()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_", WEIGHTED, outputFile, false, clearfiles);
  }

  void testExec_EventWorkspace_WeightedEventNoTime()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_", WEIGHTED_NOTIME, outputFile, false, clearfiles);
  }

  void testExec_EventWorkspace_DifferentTypes()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_DifferentTypes_", WEIGHTED_NOTIME, outputFile, true, clearfiles);
  }

  void testExec_EventWorkspace_DontPreserveEvents()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_EventTo2D", TOF, outputFile, false, clearfiles, false /* DONT preserve events */);
  }
  void testExec_EventWorkspace_CompressNexus()
  {
    std::string outputFile;
    do_testExec_EventWorkspaces("SaveNexusProcessed_EventTo2D", TOF, outputFile, false, clearfiles, true /* DONT preserve events */, true /* Compress */);
  }

  void testExecSaveLabel()
  {
    SaveNexusProcessed alg;
    if ( !alg.isInitialized() ) alg.initialize();

    // create dummy 2D-workspace
    Workspace2D_sptr localWorkspace2D = boost::dynamic_pointer_cast<Workspace2D>
      (WorkspaceFactory::Instance().create("Workspace2D",1,10,10));

    //set units to be a label
    localWorkspace2D->getAxis(0)->unit() = UnitFactory::Instance().create("Label");
    auto label = boost::dynamic_pointer_cast<Mantid::Kernel::Units::Label>(localWorkspace2D->getAxis(0)->unit());
    label->setLabel("Temperature","K");

    double d = 0.0;
    for(int i = 0; i<10; ++i,d+=0.1)
    {
      localWorkspace2D->dataX(0)[i] = d;
      localWorkspace2D->dataY(0)[i] = d;
      localWorkspace2D->dataE(0)[i] = d;
    }

    AnalysisDataService::Instance().addOrReplace("testSpace", localWorkspace2D);

    // Now set it...
    // specify name of file to save workspace to
    alg.setPropertyValue("InputWorkspace", "testSpace");
    outputFile = "SaveNexusProcessedTest_testExec.nxs";
    //entryName = "test";
    dataName = "spectra";
    title = "A simple workspace saved in Processed Nexus format";
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Filename", outputFile));
    outputFile = alg.getPropertyValue("Filename");
    //alg.setPropertyValue("EntryName", entryName);
    alg.setPropertyValue("Title", title);
    if( Poco::File(outputFile).exists() ) Poco::File(outputFile).remove();

    std::string result;
    TS_ASSERT_THROWS_NOTHING( result = alg.getPropertyValue("Filename") );
    TS_ASSERT( ! result.compare(outputFile));

    // changed so that 1D workspaces are no longer written.
    TS_ASSERT_THROWS_NOTHING(alg.execute());
    TS_ASSERT( alg.isExecuted() );

    if(clearfiles) Poco::File(outputFile).remove();

    AnalysisDataService::Instance().remove("testSpace");
  }

  void testSaveTableVectorColumn()
  {
    std::string outputFileName = "SaveNexusProcessedTest_testSaveTableVectorColumn.nxs";

    // Create a table which we will save
    ITableWorkspace_sptr table = WorkspaceFactory::Instance().createTable();
    table->addColumn("vector_int", "IntVectorColumn");
    table->addColumn("vector_double", "DoubleVectorColumn");

    std::vector<double> d1, d2, d3;
    d1.push_back(0.5);
    d2.push_back(1.0); d2.push_back(2.5);
    d3.push_back(4.0);

    // Add some rows of different sizes
    TableRow row1 = table->appendRow(); row1 << Strings::parseRange("1")<< d1;
    TableRow row2 = table->appendRow(); row2 << Strings::parseRange("2,3") << d2;
    TableRow row3 = table->appendRow(); row3 << Strings::parseRange("4,5,6,7") << d3;

    ScopedWorkspace inputWsEntry(table);

    SaveNexusProcessed alg;
    alg.initialize();
    alg.setPropertyValue("InputWorkspace", inputWsEntry.name());
    alg.setPropertyValue("Filename", outputFileName);

    TS_ASSERT_THROWS_NOTHING( alg.execute() );
    TS_ASSERT( alg.isExecuted() );

    if ( ! alg.isExecuted() )
      return; // Nothing to check

    // Get full output file path
    outputFileName = alg.getPropertyValue("Filename");

    try
    {
      NeXus::File savedNexus(outputFileName);

      savedNexus.openGroup("mantid_workspace_1", "NXentry");
      savedNexus.openGroup("table_workspace", "NXdata");

      // -- Checking int column -----

      savedNexus.openData("column_1");

      NeXus::Info columnInfo1 = savedNexus.getInfo();
      TS_ASSERT_EQUALS( columnInfo1.dims.size(), 2 );
      TS_ASSERT_EQUALS( columnInfo1.dims[0], 3 );
      TS_ASSERT_EQUALS( columnInfo1.dims[1], 4 );
      TS_ASSERT_EQUALS( columnInfo1.type, NX_INT32 );

      std::vector<int> data1;
      savedNexus.getData<int>(data1);

      TS_ASSERT_EQUALS( data1.size(), 12 );
      TS_ASSERT_EQUALS( data1[0], 1 );
      TS_ASSERT_EQUALS( data1[3], 0 );
      TS_ASSERT_EQUALS( data1[5], 3 );
      TS_ASSERT_EQUALS( data1[8], 4 );
      TS_ASSERT_EQUALS( data1[11], 7 );

      std::vector<NeXus::AttrInfo> attrInfos1 = savedNexus.getAttrInfos();
      TS_ASSERT_EQUALS( attrInfos1.size(), 3 );

      if ( attrInfos1.size() == 3 )
      {
        TS_ASSERT_EQUALS( attrInfos1[0].name, "units");
        TS_ASSERT_EQUALS( attrInfos1[1].name, "interpret_as");
        TS_ASSERT_EQUALS( attrInfos1[2].name, "name");

        TS_ASSERT_EQUALS( savedNexus.getStrAttr(attrInfos1[1]), "A vector of int" );
        TS_ASSERT_EQUALS( savedNexus.getStrAttr(attrInfos1[2]), "IntVectorColumn" );
      }

      // -- Checking double column -----

      savedNexus.openData("column_2");

      NeXus::Info columnInfo2 = savedNexus.getInfo();
      TS_ASSERT_EQUALS( columnInfo2.dims.size(), 2 );
      TS_ASSERT_EQUALS( columnInfo2.dims[0], 3 );
      TS_ASSERT_EQUALS( columnInfo2.dims[1], 2 );
      TS_ASSERT_EQUALS( columnInfo2.type, NX_FLOAT64 );

      std::vector<double> data2;
      savedNexus.getData<double>(data2);

      TS_ASSERT_EQUALS( data2.size(), 6 );
      TS_ASSERT_EQUALS( data2[0], 0.5 );
      TS_ASSERT_EQUALS( data2[3], 2.5 );
      TS_ASSERT_EQUALS( data2[5], 0.0 );

      std::vector<NeXus::AttrInfo> attrInfos2 = savedNexus.getAttrInfos();
      TS_ASSERT_EQUALS( attrInfos2.size(), 3 );

      if ( attrInfos2.size() == 3 )
      {
        TS_ASSERT_EQUALS( attrInfos2[0].name, "units");
        TS_ASSERT_EQUALS( attrInfos2[1].name, "interpret_as");
        TS_ASSERT_EQUALS( attrInfos2[2].name, "name");

        TS_ASSERT_EQUALS( savedNexus.getStrAttr(attrInfos2[1]), "A vector of double" );
        TS_ASSERT_EQUALS( savedNexus.getStrAttr(attrInfos2[2]), "DoubleVectorColumn" );
      }
    }
    catch(std::exception& e)
    {
      TS_FAIL( e.what() );
    }

    Poco::File(outputFileName).remove();
  }

private:
  std::string outputFile;
  std::string entryName;
  std::string dataName;
  std::string title;
  Workspace2D myworkspace;

  Mantid::DataHandling::LoadRaw3 loader;
  std::string inputFile;
  std::string outputSpace;
  bool clearfiles;

};
#endif /*SAVENEXUSPROCESSEDTEST_H_*/
