#include "MantidMDAlgorithms/IntegrateNormalizedMDHistoWorkspace.h"

#include <iostream>
#include <fstream>
#include "MantidDataObjects/MDEventFactory.h"
#include "MantidDataObjects/MDEventInserter.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"
#include "MantidKernel/CompositeValidator.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidAPI/WorkspaceValidators.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


namespace Mantid {
namespace MDAlgorithms {

using namespace API;
using namespace DataObjects;
using namespace Geometry;
using namespace Kernel;
// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(IntegrateNormalizedMDHistoWorkspace)

//----------------------------------------------------------------------------------------------
/** Constructor
 */
IntegrateNormalizedMDHistoWorkspace::IntegrateNormalizedMDHistoWorkspace() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
IntegrateNormalizedMDHistoWorkspace::~IntegrateNormalizedMDHistoWorkspace() {}

//----------------------------------------------------------------------------------------------
/// Algorithm's name for identification. @see Algorithm::name
const std::string IntegrateNormalizedMDHistoWorkspace::name() const {
  return "IntegrateNormalizedMDHistoWorkspace";
}

/// Algorithm's version for identification. @see Algorithm::version
int IntegrateNormalizedMDHistoWorkspace::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string IntegrateNormalizedMDHistoWorkspace::category() const {
  return "MDAlgorithms";
}

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void IntegrateNormalizedMDHistoWorkspace::init() {
  declareProperty(new WorkspaceProperty<IMDEventWorkspace>("InputWorkspace", "",
                                                           Direction::Input),
                  "An input MDWorkspace.");

  auto fluxValidator = boost::make_shared<CompositeValidator>();
  fluxValidator->add<WorkspaceUnitValidator>("Momentum");
  fluxValidator->add<InstrumentValidator>();
  fluxValidator->add<CommonBinsValidator>();
  auto solidAngleValidator = fluxValidator->clone();

  declareProperty(new WorkspaceProperty<Workspace>("FluxWorkspace", "", Direction::Input,
                                          fluxValidator),
                  "An input workspace containing momentum dependent flux.");
  declareProperty(new WorkspaceProperty<Workspace>("SolidAngleWorkspace", "",
                                          Direction::Input,
                                          solidAngleValidator),
                  "An input workspace containing momentum integrated vanadium "
                  "(a measure of the solid angle).");

  declareProperty("MaxHKL", 10,
                        "h, k, and l values between -MaxHKL and MaxHKL");

  declareProperty("Steps", 4,
                        "Integer number of steps between hkl values");

  declareProperty(new WorkspaceProperty<Workspace>("OutputWorkspace", "",
                                                   Direction::Output),
                  "A name for the output data MDHistoWorkspace.");
  declareProperty(new WorkspaceProperty<IPeaksWorkspace>("OutputPeaksWorkspace", "",
                                                   Direction::Output),
                  "A name for the output data PeaksWorkspace.");

}

/**
Extracts mdevent information from the histo data and directs the creation of new
DataObjects on the workspace.
@param ws: Workspace to add the events to.
*/
template <typename MDE, size_t nd>
void IntegrateNormalizedMDHistoWorkspace::addEventsData(
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
void IntegrateNormalizedMDHistoWorkspace::exec() {
  IMDEventWorkspace_sptr MDdata = getProperty("InputWorkspace");
  Workspace_sptr flux = getProperty("FluxWorkspace");
  Workspace_sptr sa =
      getProperty("SolidAngleWorkspace");
  int steps = getProperty("Steps");
  int maxHKL = getProperty("MaxHKL");
  double boxSize = 1./double(steps);
  int numBins=int(maxHKL*2.0/boxSize) + 1;
  double boxEdge=double(maxHKL) + boxSize / 2.0;
  std::ostringstream s0;
  s0 <<"[H,0,0],-"<< boxEdge <<","<<boxEdge<<","<<numBins;
  std::ostringstream s1;
  s1 <<"[0,K,0],-"<< boxEdge <<","<<boxEdge<<","<<numBins;
  std::ostringstream s2;
  s2 <<"[0,0,L],-"<< boxEdge <<","<<boxEdge<<","<<numBins;

  IAlgorithm_sptr alg = createChildAlgorithm("MDNormSCD");
  alg->setProperty("InputWorkspace", MDdata);
  alg->setProperty("FluxWorkspace", flux);
  alg->setProperty("SolidAngleWorkspace", sa);
  alg->setPropertyValue("AlignedDim0", s0.str());
  alg->setPropertyValue("AlignedDim1", s1.str());
  alg->setPropertyValue("AlignedDim2", s2.str());
  alg->setPropertyValue("OutputWorkspace", "OutWSName");
  alg->setPropertyValue("OutputNormalizationWorkspace", "OutNormWSName");
  alg->executeAsChildAlg();
  Workspace_sptr out = alg->getProperty("OutputWorkspace");
  Workspace_sptr norm = alg->getProperty("OutputNormalizationWorkspace");
  IMDHistoWorkspace_sptr mdout = boost::dynamic_pointer_cast<IMDHistoWorkspace>(out);
  IMDHistoWorkspace_sptr mdnorm = boost::dynamic_pointer_cast<IMDHistoWorkspace>(norm);

  IAlgorithm_sptr divide = createChildAlgorithm("DivideMD");
  divide->setProperty("LHSWorkspace",mdout);
  divide->setProperty("RHSWorkspace",mdnorm);
  divide->executeAsChildAlg();
  MDHistoWorkspace_sptr normalized=divide->getProperty("OutputWorkspace");
  setProperty("OutputWorkspace",normalized);

  IMDEventWorkspace_sptr mdout_events=convertToMDEvents(mdout);
  IMDEventWorkspace_sptr mdnorm_events=convertToMDEvents(mdnorm);

  IAlgorithm_sptr predict = createChildAlgorithm("PredictPeaks");
  predict->setProperty("InputWorkspace",MDdata);
  predict->setProperty("MinDSpacing",0.4);
  predict->executeAsChildAlg();
  PeaksWorkspace_sptr peaks = predict->getProperty("OutputWorkspace");

  double radInt =  boxEdge / numBins;
  double radBkg=3.0 * radInt;
  PeaksWorkspace_sptr interior=integrate(mdout_events, peaks, radInt);
  PeaksWorkspace_sptr plusBkg=integrate(mdout_events, peaks, radBkg);

  PeaksWorkspace_sptr normInt=integrate(mdnorm_events, peaks, radInt);
  PeaksWorkspace_sptr normBkg=integrate(mdnorm_events, peaks, radBkg);

  int npeaks=peaks->getNumberPeaks();
  double ratio = std::pow(radInt, 3)/(std::pow(radBkg, 3)-std::pow(radInt, 3));
  for (int i = 0; i < npeaks; ++i) {
          double sample=interior->getPeak(i).getIntensity();
          double norm=normInt->getPeak(i).getIntensity();
          double bkgSample=plusBkg->getPeak(i).getIntensity()-sample;
          double bkgNorm=normBkg->getPeak(i).getIntensity()-norm;
          double e2sample=std::pow(interior->getPeak(i).getSigmaIntensity(), 2);
          double e2norm=std::pow(normInt->getPeak(i).getSigmaIntensity(), 2);
          double e2bkgSample=std::pow(plusBkg->getPeak(i).getSigmaIntensity(), 2)-e2sample;
          double e2bkgNorm=std::pow(normBkg->getPeak(i).getSigmaIntensity(), 2)-e2norm;
          if (bkgNorm != 0 && norm != 0){
              double intensity=(sample/norm-ratio *bkgSample/bkgNorm)*1e6;
              double sigma=std::sqrt(e2sample/norm*norm+e2norm*(sample/(norm*norm))*(sample/(norm*norm))+
                  ratio*ratio*(e2bkgSample/bkgNorm*bkgNorm+e2bkgNorm*(bkgSample/(bkgNorm*bkgNorm))*(bkgSample/(bkgNorm*bkgNorm))))*1e6;
              peaks->getPeak(i).setIntensity(intensity);
              peaks->getPeak(i).setSigmaIntensity(sigma);
          }
  }

}
PeaksWorkspace_sptr IntegrateNormalizedMDHistoWorkspace::integrate( IMDEventWorkspace_sptr inWS, PeaksWorkspace_sptr peaks, double radius) {
  IAlgorithm_sptr alg = createChildAlgorithm("IntegratePeaksMD");
  alg->setProperty("InputWorkspace", inWS);
  alg->setProperty("PeaksWorkspace", peaks);
  alg->setProperty("PeakRadius", radius );
  alg->setProperty("IntegrateIfOnEdge", false);
  alg->executeAsChildAlg();
  PeaksWorkspace_sptr integratedPeaksWS = alg->getProperty("OutputWorkspace");
  return integratedPeaksWS;

}
IMDEventWorkspace_sptr IntegrateNormalizedMDHistoWorkspace::convertToMDEvents( IMDHistoWorkspace_sptr inWS) {

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
  childAlg->executeAsChildAlg();
  if (!childAlg->isExecuted())
    throw(std::runtime_error("Can not properly execute child algorithm to set "
                             "boxes"));
  Workspace_sptr out = childAlg->getProperty("OutputWorkspace");
  outWs = boost::dynamic_pointer_cast<IMDEventWorkspace>(out);
// Save it on the output.
  return outWs;
}

} // namespace Mantid
} // namespace MDAlgorithms
