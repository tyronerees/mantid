#include "MantidMDAlgorithms/ConvertMDHistoToMDEventWorkspace.h"

#include <iostream>
#include <fstream>

#include "MantidDataObjects/MDEventFactory.h"
#include "MantidDataObjects/MDEventInserter.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


namespace Mantid {
namespace MDAlgorithms {

using namespace API;
using namespace DataObjects;
using namespace Geometry;
using namespace Kernel;
// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(ConvertMDHistoToMDEventWorkspace)

//----------------------------------------------------------------------------------------------
/** Constructor
 */
ConvertMDHistoToMDEventWorkspace::ConvertMDHistoToMDEventWorkspace() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
ConvertMDHistoToMDEventWorkspace::~ConvertMDHistoToMDEventWorkspace() {}

//----------------------------------------------------------------------------------------------
/// Algorithm's name for identification. @see Algorithm::name
const std::string ConvertMDHistoToMDEventWorkspace::name() const {
  return "ConvertMDHistoToMDEventWorkspace";
}

/// Algorithm's version for identification. @see Algorithm::version
int ConvertMDHistoToMDEventWorkspace::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string ConvertMDHistoToMDEventWorkspace::category() const {
  return "MDAlgorithms";
}

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void ConvertMDHistoToMDEventWorkspace::init() {
  declareProperty(new WorkspaceProperty<API::IMDHistoWorkspace>(
                      "InputWorkspace", "", Direction::Input),
                  "An input IMDHistoWorkspace.");
  declareProperty(new WorkspaceProperty<API::IMDEventWorkspace>("OutputWorkspace", "",
                                                   Direction::Output),
                  "Name of the output MDEventWorkspace.");
}

/**
Extracts mdevent information from the histo data and directs the creation of new
DataObjects on the workspace.
@param ws: Workspace to add the events to.
*/
template <typename MDE, size_t nd>
void ConvertMDHistoToMDEventWorkspace::addEventsData(
    typename MDEventWorkspace<MDE, nd>::sptr outWs) {
  /// Creates a new instance of the MDEventInserter.
  MDEventInserter<typename MDEventWorkspace<MDE, nd>::sptr> inserter(outWs);
  std::vector<Mantid::coord_t> centers(nd);

  for (size_t i = 0; i < m_nDataObjects; ++i) {
    float signal = static_cast<float>(ws->getSignalAt(i));
    if (std::fabs(signal) < 1e-15) continue;
    float error = static_cast<float>(ws->getErrorAt(i));
    uint16_t run_no = 0;
    int32_t detector_no = 0;
    VMD boxCenter = ws->getCenter(i);
    for (size_t j = 0; j < m_nDimensions; ++j) {
      centers[j] = static_cast<float>(boxCenter[j]);
    }
    // Actually add the mdevent.
    inserter.insertMDEvent(signal, error * error, run_no, detector_no,
                           centers.data());
  }
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void ConvertMDHistoToMDEventWorkspace::exec() {
  IMDHistoWorkspace_sptr inWS = getProperty("InputWorkspace");

  ws = boost::dynamic_pointer_cast<MDHistoWorkspace>(inWS);
  if (!ws)
    throw std::runtime_error("InputWorkspace is not a MDHistoWorkspace");

  // Calculate the dimensionality
  m_nDimensions =  ws->getNumDims();
  // Calculate the actual number of columns in the MDEvent data.
  uint64_t numPoints = ws->getNPoints();
  m_nDataObjects = static_cast<size_t>(numPoints);

  // Create a target output workspace.
  IMDEventWorkspace_sptr outWs = MDEventFactory::CreateMDWorkspace(
      m_nDimensions, "MDLeanEvent");
  if (ws->getNumExperimentInfo() > 0) {
    ExperimentInfo_sptr ei = ws->getExperimentInfo(0);
    outWs->addExperimentInfo(ei);
  }
  outWs->setCoordinateSystem(ws->getSpecialCoordinateSystem());
  // Extract Dimensions and add to the output workspace.
  for (size_t i = 0; i < m_nDimensions; ++i) {
    IMDDimension_const_sptr dim = ws->getDimension(i);
    std::string id = dim->getDimensionId();
    std::string name = dim->getName();
    std::string units = dim->getUnits();
    size_t nbins = dim->getNBins();

    outWs->addDimension(MDHistoDimension_sptr(new MDHistoDimension(
        name, id, units, dim->getMinimum(),
        dim->getMaximum(), nbins)));
  }

  CALL_MDEVENT_FUNCTION(this->addEventsData, outWs)

  Mantid::API::Algorithm_sptr childAlg =
      createChildAlgorithm("SliceMD");
  if (!childAlg)
    throw(std::runtime_error(
        "Can not create child ChildAlgorithm to found min/max values"));

  childAlg->setProperty("InputWorkspace", outWs);
  childAlg->setProperty("OutputWorkspace", outWs);
  IMDDimension_const_sptr dim = ws->getDimension(0);
  std::string dimStr = dim->getName() + "," +
      boost::lexical_cast<std::string>(dim->getMinimum())+ "," +
      boost::lexical_cast<std::string>(dim->getMaximum()) + "," +
      boost::lexical_cast<std::string>(dim->getNBins());
  childAlg->setProperty("AlignedDim0", dimStr);
  dim = ws->getDimension(1);
  dimStr = dim->getName() + "," +
      boost::lexical_cast<std::string>(dim->getMinimum())+ "," +
      boost::lexical_cast<std::string>(dim->getMaximum()) + "," +
      boost::lexical_cast<std::string>(dim->getNBins());
  childAlg->setProperty("AlignedDim1", dimStr);
  dim = ws->getDimension(2);
  dimStr = dim->getName() + "," +
      boost::lexical_cast<std::string>(dim->getMinimum())+ "," +
      boost::lexical_cast<std::string>(dim->getMaximum()) + "," +
      boost::lexical_cast<std::string>(dim->getNBins());
  childAlg->setProperty("AlignedDim2", dimStr);
  childAlg->execute();
  if (!childAlg->isExecuted())
    throw(std::runtime_error("Can not properly execute child algorithm to set "
                             "boxes"));
  Workspace_sptr out = childAlg->getProperty("OutputWorkspace");
  outWs = boost::dynamic_pointer_cast<IMDEventWorkspace>(out);
// Save it on the output.
 setProperty("OutputWorkspace", outWs);
}

} // namespace Mantid
} // namespace MDAlgorithms
