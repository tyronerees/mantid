//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAlgorithms/Q1D2.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/RebinParamsValidator.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidKernel/PhysicalConstants.h"
#include "MantidKernel/VectorHelper.h"
#include "MantidAPI/WorkspaceValidators.h"
#include "MantidAPI/SpectraDetectorMap.h"
#include "MantidDataObjects/Histogram1D.h"

namespace Mantid
{
namespace Algorithms
{

// Register the algorithm into the AlgorithmFactory
//DECLARE_ALGORITHM(Q1D2)

/// Sets documentation strings for this algorithm
void Q1D2::initDocs()
{
  this->setWikiSummary("Part of the 1D data reduction chain for SANS instruments. ");
  this->setOptionalMessage("Part of the 1D data reduction chain for SANS instruments.");
}


using namespace Kernel;
using namespace API;
using namespace Geometry;

void Q1D2::init()
{
  CompositeValidator<> *dataVal = new CompositeValidator<>;
  dataVal->add(new WorkspaceUnitValidator<>("Wavelength"));
  dataVal->add(new HistogramValidator<>);
  dataVal->add(new InstrumentValidator<>);
  dataVal->add(new CommonBinsValidator<>);
  declareProperty(new WorkspaceProperty<>("DetBankWorkspace", "", Direction::Input, dataVal));
  declareProperty(new WorkspaceProperty<>("PixelAdj","", Direction::Input, true));
  CompositeValidator<> *wavVal = new CompositeValidator<>;
  wavVal->add(new WorkspaceUnitValidator<>("Wavelength"));
  wavVal->add(new HistogramValidator<>);
  declareProperty(new WorkspaceProperty<>("WavelengthAdj", "", Direction::Input, true, wavVal));
  declareProperty(new WorkspaceProperty<>("OutputWorkspace", "", Direction::Output));
  declareProperty(new ArrayProperty<double>("OutputBinning", new RebinParamsValidator));
  declareProperty("AccountForGravity",false);
}
/**
  @ throw invalid_argument if the workspaces are not mututially compatible
*/
void Q1D2::exec()
{
  m_dataWS = getProperty("DetBankWorkspace");
  MatrixWorkspace_const_sptr waveAdj = getProperty("WavelengthAdj");
  // this pointer could be NULL as PixelAdj is an optional property
  MatrixWorkspace_const_sptr pixelAdj = getProperty("PixelAdj");
  const bool doGravity = getProperty("AccountForGravity");

  //throws if we don't have common binning or another incompatibility
  examineInput(waveAdj, pixelAdj);
  // normalization as a function of wavelength (i.e. centers of x-value bins)
  MantidVec const * const binNorms = waveAdj ? &(waveAdj->readY(0)) : NULL;
  // error on the wavelength normalization
  MantidVec const * const binNormEs = waveAdj ? &(waveAdj->readE(0)) : NULL;

  //define the (large number of) data objects that are going to be used in all iterations of the loop below
    // Construct a new spectra map. This will be faster than remapping the old one
  API::SpectraDetectorMap *specMap = new SpectraDetectorMap;
  // this will become the output workspace from this algorithm
  MatrixWorkspace_sptr outputWS = setUpOutputWorkspace(getProperty("OutputBinning"), specMap);
  const MantidVec & QOut = outputWS->readX(0);
  MantidVec & YOut = outputWS->dataY(0);
  MantidVec & EOutTo2 = outputWS->dataE(0);
  // normalisation that is applied to counts in each Q bin
  MantidVec normSum(YOut.size(), 0.0);
  // the error on the normalisation
  MantidVec normError2(YOut.size(), 0.0);

  const Geometry::ISpectraDetectorMap & inSpecMap = m_dataWS->spectraMap();
  const Axis* const spectraAxis = m_dataWS->getAxis(1);



  const size_t numSpec = m_dataWS->getNumberHistograms();
  Progress progress(this, 0.1, 1.0, numSpec+1);

  PARALLEL_FOR3(m_dataWS, outputWS, pixelAdj)
  for (int i = 0; i < numSpec; ++i)
  {
    PARALLEL_START_INTERUPT_REGION
    // Get the pixel relating to this spectrum
    IDetector_const_sptr det;
    try {
      det = m_dataWS->getDetector(i);
    } catch (Exception::NotFoundError&) {
      g_log.warning() << "Spectrum index " << i << " has no detector assigned to it - discarding" << std::endl;
      // Catch if no detector. Next line tests whether this happened - test placed
      // outside here because Mac Intel compiler doesn't like 'continue' in a catch
      // in an openmp block.
    }
    // If no detector found or if detector is masked shouldn't be included skip onto the next spectrum
    if ( !det || det->isMonitor() || det->isMasked() )
    {
      continue;
    }

    // A temporary vector to store the Q values for the input workspace before the rebin
    MantidVec QIn(m_numInBins);
    convertWavetoQ(i, doGravity, QIn);
    const MantidVec & YIn = m_dataWS->readY(i);
    const MantidVec & EIn = m_dataWS->readE(i);

    double detectorAdj, detAdjErr;
    pixelWeight(pixelAdj, i, detectorAdj, detAdjErr);

    // the weighting for this input spectrum that is added to the normalization
    MantidVec norm(m_numInBins, detectorAdj);
    // the error on these weights, it contributes to the error calculation on the output workspace
    MantidVec normETo2(m_numInBins, detAdjErr*detAdjErr);
    addWaveAdj(pixelAdj, binNorms, binNormEs, norm, normETo2);
    normToBinWidth(i, QIn, norm, normETo2);
    
    //find the output bin that each input y-value will fall into, remembering there is one more bin boundary than bins
    MantidVec::const_iterator loc = QOut.end();
    for(size_t j=0; j < m_numInBins; ++j)
    {
      //Q goes from a high value to a low one in the QIn array (high Q particles arrive at low TOF) so we know loc will go downwards
      loc = std::upper_bound(QOut.begin(), loc, QIn[j]);
      // ignore counts that are out of the output range
      if ( (loc != QOut.begin()) && (loc != QOut.end()) )
      {
        const size_t bin = loc - QOut.begin() - 1;
        PARALLEL_CRITICAL(q1d_counts_sum)
        {
          YOut[bin] += YIn[j];
          normSum[bin] += norm[j];
          //these are the errors squared which will be summed and square rooted at the end
          EOutTo2[bin] += EIn[j]*EIn[j];
          normError2[bin] += normETo2[j];
        }
        //this is used to restrict the search range above for a modest increase in speed
        ++loc;
      }
    }
    
    PARALLEL_CRITICAL(q1d_spectra_map)
    {
      updateSpecMap(i, specMap, inSpecMap, outputWS);
    }

    progress.report("Computing I(Q)");
    PARALLEL_END_INTERUPT_REGION
  }
  PARALLEL_CHECK_INTERUPT_REGION

  for (int k = 0; k < YOut.size(); ++k)
  {
    // the normalisation is a = b/c where b = counts c =normalistion term
    const double c = normSum[k];
    const double a = YOut[k] /= c;
    // when a = b/c, the formula for Da, the error on a, in terms of Db, etc. is (Da/a)^2 = (Db/b)^2 + (Dc/c)^2
    //(Da)^2 = ((Db/b)^2 + (Dc/c)^2)*(b^2/c^2) = ((Db/c)^2 + (b*Dc/c^2)^2) = (Db^2 + (b*Dc/c)^2)/c^2 = (Db^2 + (Dc*a)^2)/c^2
    //this will work as long as c>0, but then the above formula above can't deal with 0 either
    const double aOverc = a/c;
    EOutTo2[k] = std::sqrt(EOutTo2[k]/(c*c) + normError2[k]*aOverc*aOverc);

    progress.report("Computing I(Q)");
  }

  setProperty("OutputWorkspace",outputWS);
}
/** If the distribution/raw counts status and binning on all the input workspaces
*  is the same and this reads some workspace description but throws if not
  @param binWS workpace that will be checked to see if it has one spectrum and the same number of bins as dataWS
  @param detectWS passing NULL for this wont raise an error, if set it will be checked this workspace has as many histograms as dataWS each with one bin
  @throw invalid_argument if the workspaces are not mututially compatible
*/
void Q1D2::examineInput(API::MatrixWorkspace_const_sptr binAdj, API::MatrixWorkspace_const_sptr detectAdj)
{
  if ( m_dataWS->getNumberHistograms() < 1 )
  {
    throw std::invalid_argument("Empty data workspace passed, can not continue");
  }
  m_numInBins = m_dataWS->readY(0).size();

  //it is not an error for these workspaces not to exist
  if (binAdj)
  {
    if ( binAdj->getNumberHistograms() != 1 )
    {
      throw std::invalid_argument("The WavelengthAdj workspace must have one spectrum");
    }
    if ( binAdj->readY(0).size() != m_numInBins )
    {
      throw std::invalid_argument("The WavelengthAdj workspace's bins must match those of the detector bank workspace");
    }
    MantidVec::const_iterator reqX = m_dataWS->readX(0).begin();
    MantidVec::const_iterator testX = binAdj->readX(0).begin();
    for ( ; reqX != m_dataWS->readX(0).end(); ++reqX, ++testX)
    {
      if ( *reqX != *testX )
      {
        throw std::invalid_argument("The WavelengthAdj workspace must have matching bins with the detector bank workspace");
      }
    }
/* the distribution status of workspaces isn't getting propogated reliably enough for this work yet i.e. a distribution workspace x a raw counts = raw counts
if ( binAdj->isDistribution() != m_dataWS->isDistribution() )
    {
      throw std::invalid_argument("The distrbution/raw counts status of the wavelengthAdj and DetBankWorkspace must be the same");
    }*/
  }
  else if( ! m_dataWS->isDistribution() )
  {
    throw std::invalid_argument("The data workspace must be a distrbution if there is no Wavelength dependent adjustment");
  }
  
  if (detectAdj)
  {
    if ( detectAdj->blocksize() != 1 )
    {
      throw std::invalid_argument("The PixelAdj workspace must point to a workspace with single bin spectra, as only the first bin is used");
    }
    if ( detectAdj->getNumberHistograms() != m_dataWS->getNumberHistograms() )
    {
      throw std::invalid_argument("The PixelAdj workspace must have one spectrum for each spectrum in the detector bank workspace");
    }
    g_log.debug() << "Optional PixelAdj workspace " << detectAdj->getName() << " validated successfully\n";
  }

  g_log.debug() << "All input workspaces were found to be valid\n";
}
/** Creates the output workspace, its size, units, etc.
*  @param binParams the bin boundary specification using the same same syntax as param the Rebin algorithm
*  @param specMap a spectra map that the new workspace should use and take owner ship of
*  @return A pointer to the newly-created workspace
*/
API::MatrixWorkspace_sptr Q1D2::setUpOutputWorkspace(const std::vector<double> & binParams,  const API::SpectraDetectorMap * const specMap) const
{
  // Calculate the output binning
  MantidVecPtr XOut;
  size_t sizeOut = static_cast<size_t>(
    VectorHelper::createAxisFromRebinParams(binParams, XOut.access()));

  // Now create the output workspace
  MatrixWorkspace_sptr outputWS = WorkspaceFactory::Instance().create(m_dataWS,1,sizeOut,sizeOut-1);
  outputWS->getAxis(0)->unit() = UnitFactory::Instance().create("MomentumTransfer");
  outputWS->setYUnitLabel("1/cm");

  // Set the X vector for the output workspace
  outputWS->setX(0, XOut);
  outputWS->isDistribution(true);

  outputWS->replaceSpectraMap(specMap);
  return outputWS;
}
/** Fills a vector with the Q values calculated from the wavelengths in the input workspace and the workspace
*  geometry as Q = 4*pi*sin(theta)/lambda
*  @param[in] specIndex the spectrum to calculate
*  @param[in] doGravity if to include gravity in the calculation of Q
*  @param[out] Qs a preallocated array that is large enough to contain all the calculated Q values
*  @throw NotFoundError if the detector associated with the spectrum is not found in the instrument definition
*/
void Q1D2::convertWavetoQ(const size_t specIndex, const bool doGravity, MantidVec & Qs) const
{
  static const double FOUR_PI=4.0*M_PI;
  const MantidVec & XIn = m_dataWS->readX(specIndex);
  IDetector_const_sptr det = m_dataWS->getDetector(specIndex);

  if (doGravity)
  {
    GravitySANSHelper grav(m_dataWS, det);
    for ( size_t j = 0; j < m_numInBins; ++j)
    {
      // as the fall under gravity is wavelength dependent sin theta is now different for each bin with each detector 
      // the HistogramValidator at the start should ensure that we have one more bin on the readX()s
      const double lambda = (XIn[j]+XIn[j+1])/2.0;
      const double sinTheta = grav.calcSinTheta(lambda);
      // Now we're ready to go to Q
      Qs[j] = FOUR_PI*sinTheta/lambda;
    }
  }
  else
  {
    // Calculate the Q values for the current spectrum, using Q = 4*pi*sin(theta)/lambda
    const double factor = 2.0* FOUR_PI*sin( m_dataWS->detectorTwoTheta(det)/2.0 );
    for ( size_t j = 0; j < m_numInBins; ++j)
    {
      // the HistogramValidator at the start should ensure that we have one more bin on the readX()s
      Qs[j] = factor/(XIn[j]+XIn[j+1]);
    }
  }
}
/** Calculates the normalisation for the spectrum specified by the index number that was passed
*  as the solid anlge multiplied by the pixelAdj that was passed
*  @param[in] pixelAdj if not NULL this is workspace contains single bins with the adjustments, e.g. detector efficencie, for hte given spectrum index
*  @param[in] specIndex the spectrum index to return the data from
*  @param[out] weight the solid angle or if pixelAdj the solid anlge times the pixel adjustment for this spectrum
*  @param[out] error the error on the weight, only non-zero if pixelAdj
*  @throw LogicError if the solid angle is tiny or negative
*/
void Q1D2::pixelWeight(API::MatrixWorkspace_const_sptr pixelAdj,  const size_t specIndex, double & weight, double & error) const
{
  const V3D samplePos = m_dataWS->getInstrument()->getSample()->getPos();

  weight = m_dataWS->getDetector(specIndex)->solidAngle(samplePos);
  if ( weight < 1e-200 )
  {
    throw std::logic_error("Invalid (zero or negative) solid angle for one detector");
  }
  // this input multiplies up the adjustment if it exists
  if (pixelAdj)
  {
    weight *= pixelAdj->readY(specIndex)[0];
    error = pixelAdj->readE(specIndex)[0];
  }
  else
  {
    error = 0.0;
  }
}
/** Calculates the contribution to the normalization terms from each bin in a spectrum
*  @param[in] pixelAdj detector efficiency input workspace
*  @param[in] binNorms the wavelength dependent normalization term
*  @param[in] binNormEs the wavelength dependent term's error
*  @param[in,out] outNorms normalization for each bin, this method multiplise this by the proportion that is not masked and the normalization workspace
*  @param[in, out] outETo2 the error on the normalisation term before the WavelengthAdj term
*/
void Q1D2::addWaveAdj(API::MatrixWorkspace_const_sptr pixelAdj, const MantidVec * const binNorms, const MantidVec * const binNormEs, MantidVec & outNorms, MantidVec & outETo2) const
{

  // normalize by the wavelength dependent correction, keeping the percentage errors the same
  if (binNorms && binNormEs)
  {
    // the error when a = b*c, the formula for Da, the error on a, in terms of Db, etc. is (Da/a)^2 = (Db/b)^2 + (Dc/c)^2
    //(Da)^2 = ((Db*a/b)^2 + (Dc*a/c)^2) = (Db*c)^2 + (Dc*b)^2
    // name the things to fit with the equation above: existing values (b=bInOut(=outNorms)) multiplied by the additional errors (Dc=binNormEs), existing errors (Db=sqrt(e2InOut(=outETo2))) times new factor (c=binNorms)
    MantidVec::iterator e2InOut=outETo2.begin(), bInOut=outNorms.begin();
    MantidVec::const_iterator c=binNorms->begin(), Dc=binNormEs->begin();
    MantidVec::const_iterator end = outETo2.end();
    for( ; e2InOut != end; ++e2InOut, ++c, ++Dc, ++bInOut)
    {
      //first the error
      *e2InOut += ( (*e2InOut)*(*c)*(*c) )+( (*Dc)*(*Dc)*(*bInOut)*(*bInOut) );
      // now the actual calculation a = b*c
      *bInOut = (*bInOut)*(*c);
    }
  }
}
/** Add the bin widths, scaled to bin masking, to the normalization
*  @param[in] specIndex the spectrum to calculate
*  @param[in] QIns Q bin boundaries
*  @param[in,out] theNorms normalization for each bin, this is multiplied by the proportion that is not masked and the normalization workspace
*  @param[in,out] errorSquared the running total of the error on the normalization
*/
void Q1D2::normToBinWidth(const size_t specIndex, const MantidVec & QIns, MantidVec & theNorms, MantidVec & errorSquared) const
{
/*  //normally this is false but handling this would mean more combinations of distribution/raw counts workspaces could be accepted
  if (m_convToDistr)
  {
    for(int i = 0; i < theNorms.size(); ++i)
    {
      const double width = ???;
      theNorms[i] *= width;
      errorSquared[i] *= width*width;
    }
  }*/
  
  // if any bins are masked it is normally a small proportion
  if ( m_dataWS->hasMaskedBins(specIndex) )
  {
    // Get a reference to the list of masked bins
    const MatrixWorkspace::MaskList & mask = m_dataWS->maskedBins(specIndex);
    // Now iterate over the list, adjusting the weights for the affected bins
    MatrixWorkspace::MaskList::const_iterator it;
    for (it = mask.begin(); it != mask.end(); ++it)
    {
      // The weight for this masked bin is 1 - the degree to which this bin is masked
      const double factor = 1.0-(it->second);
      theNorms[it->first] *= factor;
      errorSquared[it->first] *= factor*factor;
    }
  }
}
/** !!!PROTOTYPE needs more testing !!! Map all the detectors onto the spectrum of the output
*  @param[in] specIndex the spectrum to add
*  @param[out] specMap the map in the output workspace to write to
*  @param[in] inSpecMap spectrum data
*  @param[out] outputWS the workspace with the spectra axis
*/
void Q1D2::updateSpecMap(const size_t specIndex, API::SpectraDetectorMap * const specMap, const Geometry::ISpectraDetectorMap & inSpecMap, API::MatrixWorkspace_sptr outputWS) const
{
  Axis* const spectraAxis = m_dataWS->getAxis(1);
  if (spectraAxis->isSpectra())
  {
    specid_t newSpectrumNo = outputWS->getAxis(1)->spectraNo(0) = spectraAxis->spectraNo(specIndex);
    specMap->addSpectrumEntries(newSpectrumNo,inSpecMap.getDetectors(spectraAxis->spectraNo(specIndex)));
  }
}

} // namespace Algorithms
} // namespace Mantid

