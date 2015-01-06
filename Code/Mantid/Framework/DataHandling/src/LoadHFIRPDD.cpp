#include "MantidDataHandling/LoadHFIRPDD.h"

#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/FileProperty.h"
#include "MantidGeometry/IDetector.h"
#include "MantidKernel/TimeSeriesProperty.h"

#include <boost/algorithm/string/predicate.hpp>
#include <Poco/TemporaryFile.h>

namespace Mantid
{
namespace DataHandling
{

  using namespace Mantid::API;
  using namespace Mantid::Kernel;
  using namespace Mantid::DataObjects;

  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  LoadHFIRPDD::LoadHFIRPDD() : m_instrumentName(""), m_numSpec(0)
  {
  }

  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  LoadHFIRPDD::~LoadHFIRPDD()
  {
  }


  //----------------------------------------------------------------------------------------------
  /** Init
   */
  void LoadHFIRPDD::init()
  {
    declareProperty(new WorkspaceProperty<TableWorkspace>("InputWorkspace", "",
                                                          Direction::Input),
                    "Input table workspace for data.");

    declareProperty(new WorkspaceProperty<MatrixWorkspace>(
                        "ParentWorkspace", "", Direction::Input),
                    "Input matrix workspace serving as parent workspace "
                    "containing sample logs.");

    /* These are for stage 2
    std::vector<std::string> exts;
    exts.push_back(".dat");
    exts.push_back(".txt");
    declareProperty(
        new API::FileProperty("Filename", "", API::FileProperty::Load, exts),
        "The input filename of the stored data");
        */

    declareProperty(new WorkspaceProperty<IMDEventWorkspace>(
                        "OutputWorkspace", "", Direction::Output),
                    "Name to use for the output workspace.");


  }

  //----------------------------------------------------------------------------------------------
  /** Exec
   */
  void LoadHFIRPDD::exec()
  {
    /** Stage 2
    // Process inputs
    std::string spiceFileName = getProperty("Filename");

    // Load SPICE data
    m_dataTableWS = loadSpiceData(spiceFileName);
    */
    m_dataTableWS = getProperty("InputWrokspace");
    MatrixWorkspace_const_sptr parentWS = getProperty("ParentWorkspace");

    // Convert table workspace to a list of 2D workspaces
    std::vector<MatrixWorkspace_sptr> vec_ws2d =
        convertToWorkspaces(m_dataTableWS, parentWS);

    // Convert to MD workspaces
    IMDEventWorkspace_sptr m_mdEventWS = convertToMDEventWS(vec_ws2d);

    // Set property
    setProperty("OutputWorkspace", m_mdEventWS);

  }

  //----------------------------------------------------------------------------------------------
  /** Load data by call
   */
  TableWorkspace_sptr LoadHFIRPDD::loadSpiceData(const std::string &spicefilename)
  {
    const std::string tempoutws = "_tempoutdatatablews";
    const std::string tempinfows = "_tempinfomatrixws";

    IAlgorithm_sptr loader = this->createChildAlgorithm("LoadSPICEAscii", 0, 5, true);

    loader->initialize();
    loader->setProperty("Filename", spicefilename);
    loader->setPropertyValue("OutputWorkspace", tempoutws);
    loader->setPropertyValue("RunInfoWorkspace", tempinfows);
    loader->executeAsChildAlg();

    TableWorkspace_sptr tempdatatablews = loader->getProperty("OutputWorkspace");
    if (tempdatatablews)
      g_log.notice() << "[DB] data table contains " << tempdatatablews->rowCount() << " lines." << "\n";
    else
      g_log.notice("No table workspace is returned.");

    return tempdatatablews;
  }

  //----------------------------------------------------------------------------------------------
  /** Convert runs/pts from table workspace to a list of workspace 2D
   */
  std::vector<MatrixWorkspace_sptr>
  LoadHFIRPDD::convertToWorkspaces(DataObjects::TableWorkspace_sptr tablews,
                                   API::MatrixWorkspace_const_sptr parentws) {
    // For HB2A m_numSpec is 44
    // MatrixWorkspace_sptr parentws = createParentWorkspace(m_numSpec);

    // Get table workspace's column information
    size_t ipt, irotangle, itime;
    std::vector<std::pair<size_t, size_t> > anodelist;
    readTableInfo(tablews, ipt, irotangle, itime, anodelist);

    // Load data
    size_t numws = tablews->rowCount();
    std::vector<MatrixWorkspace_sptr> vecws(numws);
    double duration = 0;
    Kernel::DateAndTime runstart =
        parentws->run().getProperty("run_start")->value();
    for (size_t i = 0; i < numws; ++i) {
      vecws[i] = loadRunToMatrixWS(tablews, i, parentws, runstart, irotangle,
                                   itime, anodelist, duration);
      runstart += static_cast<int64_t>(duration * 1.0E9);
    }

    return vecws;
  }

  //-------------------------------------------------------------------
  //---------------------------
  /** Load one run of data to a new workspace
   * @brief LoadHFIRPDD::loadRunToMatrixWS
   * @param tablews :: input workspace
   * @param irow :: the row in workspace to load
   * @param parentws :: parent workspace with preset log
   * @param runstart :: run star time
   * @param irotangle :: column index of rotation angle
   * @param itime :: column index of duration
   * @param anodelist :: list of anodes
   * @param duration :: output of duration
   * @return
   */
  MatrixWorkspace_sptr LoadHFIRPDD::loadRunToMatrixWS(
      DataObjects::TableWorkspace_sptr tablews, size_t irow,
      MatrixWorkspace_const_sptr parentws, Kernel::DateAndTime runstart,
      size_t irotangle, size_t itime,
      const std::vector<std::pair<size_t, size_t> > anodelist,
      double &duration) {
    // New workspace from parent workspace
    MatrixWorkspace_sptr tempws = WorkspaceFactory::Instance().create(parentws, m_numSpec, 2, 1);

    // Set up angle and time
    double twotheta = tablews->cell<double>(irow, irotangle);
    TimeSeriesProperty<double> *prop2theta =
        new TimeSeriesProperty<double>("rotangle");

    prop2theta->addValue(runstart, twotheta);
    tempws->mutableRun().addProperty(prop2theta);

    TimeSeriesProperty<std::string> *proprunstart =
        new TimeSeriesProperty<std::string>("run_start");
    proprunstart->addValue(runstart, runstart.toISO8601String());
    tempws->mutableRun().addProperty(proprunstart);

    // Load instrument
    IAlgorithm_sptr instloader = this->createChildAlgorithm("LoadInstrument");
    instloader->initialize();
    instloader->setProperty("Instrument", m_instrumentName);

    // Import data
    for (size_t i = 0; i < m_numSpec; ++i)
    {
      Geometry::IDetector_const_sptr tmpdet = tempws->getDetector(i);
      tempws->dataX(i)[0] = tmpdet->getPos().X();
      tempws->dataX(i)[0] = tmpdet->getPos().X()+0.01;
      tempws->dataY(i)[0] = tablews->cell<double>(irow, anodelist[i].second);
      tempws->dataE(i)[0] = 1;
    }

    // Return duration
    duration = tablews->cell<double>(irow, itime);

    return tempws;
  }

  //----------------------------------------------------------------------------------------------
  /** Read table workspace's column information
   */
  void LoadHFIRPDD::readTableInfo(TableWorkspace_const_sptr tablews, size_t& ipt, size_t& irotangle, size_t& itime,
                                  std::vector<std::pair<size_t, size_t> >& anodelist)
  {
    // Init
    bool bfPt = false;
    bool bfRotAngle = false;
    bool bfTime = false;

    ipt = -1;
    irotangle = -1;
    itime = -1;

    const std::vector<std::string> & colnames = tablews->getColumnNames();

    for (size_t icol = 0; icol < colnames.size(); ++icol)
    {
      const std::string &colname = colnames[icol];

      if ( !bfPt && colname.compare("Pt.") == 0)
      {
        // Pt.
        ipt = icol;
        bfPt = true;
      }
      else if ( !bfRotAngle && colname.compare("2theta") == 0)
      {
        // 2theta_zero
        irotangle = icol;
        bfRotAngle = true;
      }
      else if (!bfTime && colname.compare("time") == 0)
      {
        // time
        itime = icol;
        bfTime = true;
      }
      else if ( boost::starts_with(colname, "anode") )
      {
        // anode
        std::vector<std::string> terms;
        boost::split(terms, colname, boost::is_any_of("anode"));
        size_t anodeid = static_cast<size_t>(atoi(terms.back().c_str()));
        anodelist.push_back( std::make_pair(anodeid, icol) );
      }
      else
      {
        // regular log
        // FIXME - Should add to sample log
        continue;
      }

    } // ENDFOR

    // Sort out anode id index list;
    std::sort(anodelist.begin(), anodelist.end());

    return;
  }


  //----------------------------------------------------------------------------------------------
  /**
   * @brief LoadHFIRPDD::createParentWorkspace
   * @param numspec
   * @return
   */
  API::MatrixWorkspace_sptr LoadHFIRPDD::createParentWorkspace(size_t numspec)
  {
    // TODO - This method might be deleted

    MatrixWorkspace_sptr tempws =
        WorkspaceFactory::Instance().create("Workspace2D", numspec, 2, 1);

    // FIXME - Need unit

    // TODO - Load property from

    return tempws;
  }

  //----------------------------------------------------------------------------------------------
  /** Convert to MD Event workspace
   */
  IMDEventWorkspace_sptr LoadHFIRPDD::convertToMDEventWS(const std::vector<MatrixWorkspace_sptr> vec_ws2d)
  {
    // Write the lsit of workspacs to a file to be loaded to an MD workspace
    Poco::TemporaryFile tmpFile;
    std::string tempFileName = tmpFile.path();
    g_log.debug() << "Dumping WSs in a temp file: " << tempFileName << std::endl;

    std::ofstream myfile;
    myfile.open(tempFileName.c_str());
    myfile << "DIMENSIONS" << std::endl;
    myfile << "x X m 100" << std::endl;
    myfile << "y Y m 100" << std::endl;
    myfile << "z Z m 100" << std::endl;
    myfile << "# Signal, Error, DetectorId, RunId, coord1, coord2, ... to end of "
              "coords" << std::endl;
    myfile << "MDEVENTS" << std::endl;

    if (vec_ws2d.size() > 0) {
      Progress progress(this, 0, 1, vec_ws2d.size());
      for (auto it = vec_ws2d.begin(); it < vec_ws2d.end(); ++it) {
        std::size_t pos = std::distance(vec_ws2d.begin(), it);
        API::MatrixWorkspace_sptr thisWorkspace = *it;

        std::size_t nHist = thisWorkspace->getNumberHistograms();
        for (std::size_t i = 0; i < nHist; ++i) {
          Geometry::IDetector_const_sptr det = thisWorkspace->getDetector(i);
          const MantidVec &signal = thisWorkspace->readY(i);
          const MantidVec &error = thisWorkspace->readE(i);
          myfile << signal[0] << " ";
          myfile << error[0] << " ";
          myfile << det->getID() << " ";
          myfile << pos << " ";
          Kernel::V3D detPos = det->getPos();
          myfile << detPos.X() << " ";
          myfile << detPos.Y() << " ";
          myfile << detPos.Z() << " ";
          myfile << std::endl;
        }
        progress.report("Creating MD WS");
      }
      myfile.close();
    }

    // Import to MD Workspace
    IAlgorithm_sptr importMDEWS = createChildAlgorithm("ImportMDEventWorkspace");
    // Now execute the Child Algorithm.
    try {
      importMDEWS->setPropertyValue("Filename", tempFileName);
      importMDEWS->setProperty("OutputWorkspace", "Test");
      importMDEWS->executeAsChildAlg();
    }
    catch (std::exception &exc) {
      throw std::runtime_error(
            std::string("Error running ImportMDEventWorkspace: ") + exc.what());
    }
    IMDEventWorkspace_sptr workspace =
        importMDEWS->getProperty("OutputWorkspace");
    if (!workspace)
      throw(std::runtime_error("Can not retrieve results of child algorithm "
                               "ImportMDEventWorkspace"));

    return workspace;

  }



} // namespace DataHandling
} // namespace Mantid
