#include "MantidDataHandling/LoadHFIRPDD.h"

#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/FileProperty.h"

#include <boost/algorithm/string/predicate.hpp>

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
  LoadHFIRPDD::LoadHFIRPDD()
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
    std::vector<std::string> exts;
    exts.push_back(".gsa");
    exts.push_back(".gss");
    exts.push_back(".gda");
    exts.push_back(".txt");
    declareProperty(
        new API::FileProperty("Filename", "", API::FileProperty::Load, exts),
        "The input filename of the stored data");

    declareProperty(new WorkspaceProperty<IMDEventWorkspace>(
                        "OutputWorkspace", "", Direction::Output),
                    "Name to use for the output workspace.");


  }

  //----------------------------------------------------------------------------------------------
  /** Exec
   */
  void LoadHFIRPDD::exec()
  {
    // Process inputs
    std::string spiceFileName = getProperty("Filename");

    // Load SPICE data
    m_dataTableWS = loadSpiceData(spiceFileName);

    // Convert table workspace to a list of 2D workspaces
    vec_ws2d = convertToWorkspaces(m_dataTableWS);

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
  std::vector<MatrixWorkspace_sptr> LoadHFIRPDD::convertToWorkspaces(DataObjects::TableWorkspace_sptr tablews)
  {
    // For HB2A m_numSpec is 44
    MatrixWorkspace_sptr parentws = createParentWorkspace(m_numSpec);

    // Get table workspace's column information
    readTableInfo(tablews);

    // Load data
    size_t numws = tablews->rowCount();
    vector<MatrixWorkspace_sptr> vecws(numws);
    for (size_t i = 0; i < numws; ++i)
      vecws[i] = loadRunToMatrixWS(tablews, i);




  }

  //----------------------------------------------------------------------------------------------
  /** Read table workspace's column information
   */
  void LoadHFIRPDD::readTableInfo(TableWorkspace_const_sptr tablews)
  {
    // Init
    bool bfPt = false;
    bool bfRotAngle = false;

    size_t ipt = -1;
    size_t irotangle = -1;

    const std::vector<std::string> & colnames = tablews.getColumnNames();

    vector<size_t> anodelist;

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
      else if ( boost::starts_with("anode") )
      {
        // anode
        size_t anodeid = int(colname.split("anode")[-1]);
        anodelist.push_back( std::make_pair<size_t, size_t>(anodeid, icol) );
      }
      else
      {
        // regular log
        // FIXME - Should add to sample log
        continue;
      }

    } // ENDFOR

    // Sort out anode id index list;
    anodelist = sorted(anodelist);

    return  (ipt, irotangle, anodelist);
  }


  //----------------------------------------------------------------------------------------------

  API::MatrixWorkspace_sptr LoadHFIRPDD::createParentWorkspace(size_t numspec)
  {
    MatrixWorkspace_sptr tempws =
        WorkspaceFactory::Instance().create("Workspace2D", numspec, 2, 1);

    // FIXME - Need unit

    // TODO - Load property from

    return tempws;
  }



} // namespace DataHandling
} // namespace Mantid
