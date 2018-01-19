#include "JumpFit.h"
#include "../General/UserInputValidator.h"
#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/IFunction.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/WorkspaceGroup.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

using namespace Mantid::API;

namespace MantidQt {
namespace CustomInterfaces {
namespace IDA {

JumpFit::JumpFit(QWidget *parent)
    : IndirectFitAnalysisTab(parent), m_uiForm(new Ui::JumpFit) {
  m_uiForm->setupUi(parent);
  IndirectFitAnalysisTab::addPropertyBrowserToUI(m_uiForm.get());
}

void JumpFit::setup() {
  auto chudleyElliot =
      FunctionFactory::Instance().createFunction("ChudleyElliot");
  auto hallRoss = FunctionFactory::Instance().createFunction("HallRoss");
  auto fickDiffusion =
      FunctionFactory::Instance().createFunction("FickDiffusion");
  auto teixeiraWater =
      FunctionFactory::Instance().createFunction("TeixeiraWater");
  addComboBoxFunctionGroup("ChudleyElliot", {chudleyElliot});
  addComboBoxFunctionGroup("HallRoss", {hallRoss});
  addComboBoxFunctionGroup("FickDiffusion", {fickDiffusion});
  addComboBoxFunctionGroup("TeixeiraWater", {teixeiraWater});

  disablePlotGuess();

  // Create range selector
  auto qRangeSelector = m_uiForm->ppPlotTop->addRangeSelector("JumpFitQ");
  connect(qRangeSelector, SIGNAL(minValueChanged(double)), this,
          SLOT(xMinSelected(double)));
  connect(qRangeSelector, SIGNAL(maxValueChanged(double)), this,
          SLOT(xMaxSelected(double)));

  m_uiForm->cbWidth->setEnabled(false);

  // Connect data selector to handler method
  connect(m_uiForm->dsSample, SIGNAL(dataReady(const QString &)), this,
          SLOT(handleSampleInputReady(const QString &)));
  // Connect width selector to handler method
  connect(m_uiForm->cbWidth, SIGNAL(currentIndexChanged(const QString &)), this,
          SLOT(handleWidthChange(const QString &)));

  // Handle plotting and saving
  connect(m_uiForm->pbSave, SIGNAL(clicked()), this, SLOT(saveClicked()));
  connect(m_uiForm->pbPlot, SIGNAL(clicked()), this, SLOT(plotClicked()));
  connect(m_uiForm->pbPlotPreview, SIGNAL(clicked()), this,
          SLOT(plotCurrentPreview()));

  connect(m_uiForm->ckPlotGuess, SIGNAL(stateChanged(int)), this,
          SLOT(plotGuess()));
}

/**
 * Validate the form to check the program can be run
 *
 * @return :: Whether the form was valid
 */
bool JumpFit::validate() {
  UserInputValidator uiv;
  uiv.checkDataSelectorIsValid("Sample", m_uiForm->dsSample);

  // this workspace doesn't have any valid widths
  if (m_spectraList.size() == 0) {
    uiv.addErrorMessage(
        "Input workspace doesn't appear to contain any width data.");
  }

  if (emptyModel())
    uiv.addErrorMessage("No fit function has been selected");

  QString errors = uiv.generateErrorMessage();
  if (!errors.isEmpty()) {
    emit showMessageBox(errors);
    return false;
  }

  return true;
}

/**
 * Collect the settings on the GUI and build a python
 * script that runs JumpFit
 */
void JumpFit::run() {
  if (validate())
    executeSequentialFit();
}

/**
 * Handles the JumpFit algorithm finishing, used to plot fit in miniplot.
 *
 * @param error True if the algorithm failed, false otherwise
 */
void JumpFit::algorithmComplete(bool error) {
  // Ignore errors
  if (error)
    return;
  m_uiForm->pbPlot->setEnabled(true);
  m_uiForm->pbSave->setEnabled(true);

  // Process the parameters table
  const auto paramWsName = outputWorkspaceName() + "_Parameters";
  const auto resultWsName = outputWorkspaceName() + "_Result";
  deleteWorkspaceAlgorithm(paramWsName)->execute();
  renameWorkspaceAlgorithm(outputWorkspaceName(), paramWsName)->execute();
  processParametersAlgorithm(paramWsName, resultWsName)->execute();
  IndirectFitAnalysisTab::fitAlgorithmComplete(paramWsName);
}

/**
 * Set the data selectors to use the default save directory
 * when browsing for input files.
 *
 * @param settings :: The current settings
 */
void JumpFit::loadSettings(const QSettings &settings) {
  m_uiForm->dsSample->readSettings(settings.group());
}

/**
 * Plots the loaded file to the miniplot and sets the guides
 * and the range
 *
 * @param filename :: The name of the workspace to plot
 */
void JumpFit::handleSampleInputReady(const QString &filename) {
  // Scale to convert to HWHM
  const auto sample = filename + "_HWHM";
  scaleAlgorithm(filename.toStdString(), sample.toStdString(), 0.5)->execute();

  IndirectFitAnalysisTab::newInputDataLoaded(sample);

  QPair<double, double> res;
  QPair<double, double> range = m_uiForm->ppPlotTop->getCurveRange("Sample");
  auto bounds = getResolutionRangeFromWs(sample, res) ? res : range;
  auto qRangeSelector = m_uiForm->ppPlotTop->getRangeSelector("JumpFitQ");
  qRangeSelector->setMinimum(bounds.first);
  qRangeSelector->setMaximum(bounds.second);

  findAllWidths(inputWorkspace());

  if (m_spectraList.size() > 0) {
    m_uiForm->cbWidth->setEnabled(true);
    const auto currentWidth = m_uiForm->cbWidth->currentText().toStdString();
    setSelectedSpectrum(static_cast<int>(m_spectraList[currentWidth]));
    setMinimumSpectrum(static_cast<int>(m_spectraList[currentWidth]));
    setMaximumSpectrum(static_cast<int>(m_spectraList[currentWidth]));
  } else {
    m_uiForm->cbWidth->setEnabled(false);
    emit showMessageBox("Workspace doesn't appear to contain any width data");
  }
}

/**
 * Find all of the spectra in the workspace that have width data
 *
 * @param ws :: The workspace to search
 */
void JumpFit::findAllWidths(MatrixWorkspace_const_sptr ws) {
  m_uiForm->cbWidth->blockSignals(true);
  m_uiForm->cbWidth->clear();

  auto axis = dynamic_cast<TextAxis *>(ws->getAxis(1));

  if (axis) {
    m_spectraList = findAxisLabelsWithSubstrings(axis, {".Width", ".FWHM"}, 3);

    for (const auto &iter : m_spectraList)
      m_uiForm->cbWidth->addItem(QString::fromStdString(iter.first));
  } else {
    m_spectraList.clear();
  }

  m_uiForm->cbWidth->blockSignals(false);
}

std::map<std::string, size_t> JumpFit::findAxisLabelsWithSubstrings(
    TextAxis *axis, const std::vector<std::string> &substrings,
    const size_t &maximumNumber) const {
  std::map<std::string, size_t> labels;

  for (size_t i = 0u; i < axis->length(); ++i) {
    const auto label = axis->label(i);
    size_t substringIndex = 0;
    size_t foundIndex = std::string::npos;

    while (substringIndex < substrings.size() &&
           foundIndex == std::string::npos && labels.size() < maximumNumber)
      foundIndex = label.find(substrings[substringIndex++]);

    if (foundIndex != std::string::npos)
      labels[label] = i;
  }
  return labels;
}

/**
 * Plots the loaded file to the miniplot when the selected spectrum changes
 *
 * @param text :: The name spectrum index to plot
 */
void JumpFit::handleWidthChange(const QString &text) {
  const auto width = text.toStdString();

  if (m_spectraList.find(width) != m_spectraList.end()) {
    setSelectedSpectrum(static_cast<int>(m_spectraList[width]));
    updatePreviewPlots();
  }
}

void JumpFit::startXChanged(double startX) {
  auto rangeSelector = m_uiForm->ppPlotTop->getRangeSelector("JumpFitQ");
  rangeSelector->blockSignals(true);
  rangeSelector->setMinimum(startX);
  rangeSelector->blockSignals(false);
}

void JumpFit::endXChanged(double endX) {
  auto rangeSelector = m_uiForm->ppPlotTop->getRangeSelector("JumpFitQ");
  rangeSelector->blockSignals(true);
  rangeSelector->setMaximum(endX);
  rangeSelector->blockSignals(false);
}

void JumpFit::disablePlotGuess() {
  m_uiForm->ckPlotGuess->setEnabled(false);
  m_uiForm->ckPlotGuess->blockSignals(true);
}

void JumpFit::enablePlotGuess() {
  m_uiForm->ckPlotGuess->setEnabled(true);
  m_uiForm->ckPlotGuess->blockSignals(false);
}

/**
 * Updates the plot
 */
void JumpFit::updatePreviewPlots() {
  const auto baseGroupName = outputWorkspaceName() + "_Workspaces";
  IndirectFitAnalysisTab::updatePlot(baseGroupName, m_uiForm->ppPlotTop,
                                     m_uiForm->ppPlotBottom);
}

void JumpFit::updatePlotRange() {
  IndirectDataAnalysisTab::updatePlotRange("JumpFitQ", m_uiForm->ppPlotTop);
}

void JumpFit::plotGuess() {
  // Do nothing if there is not a sample
  if (m_uiForm->ckPlotGuess->isEnabled() &&
      m_uiForm->ckPlotGuess->isChecked()) {
    IndirectFitAnalysisTab::plotGuess(m_uiForm->ppPlotTop);
  } else {
    m_uiForm->ppPlotTop->removeSpectrum("Guess");
    m_uiForm->ckPlotGuess->setChecked(false);
  }
}

std::string JumpFit::createSingleFitOutputName() const {
  auto outputName = inputWorkspace()->getName();

  // Remove _red
  const auto cutIndex = outputName.find_last_of('_');
  if (cutIndex != std::string::npos)
    outputName = outputName.substr(0, cutIndex);
  return outputName + "_" + selectedFitType().toStdString() + "_JumpFit";
}

IAlgorithm_sptr JumpFit::singleFitAlgorithm() const {
  const auto sample = inputWorkspace()->getName();
  const auto widthText = m_uiForm->cbWidth->currentText().toStdString();
  const auto width = m_spectraList.at(widthText);

  auto fitAlg = AlgorithmManager::Instance().create("PlotPeakByLogValue");
  fitAlg->initialize();
  fitAlg->setProperty("Input", sample + ",i" + std::to_string(width));
  fitAlg->setProperty("OutputWorkspace", outputWorkspaceName());
  fitAlg->setProperty("CreateOutput", true);
  return fitAlg;
}

/*
 * Creates an algorithm for processing an output parameters workspace.
 *
 * @param parameterWSName The name of the parameters workspace.
 * @return                A processing algorithm.
 */
IAlgorithm_sptr
JumpFit::processParametersAlgorithm(const std::string &parameterWSName,
                                    const std::string &resultWSName) {
  const auto parameterNames =
      boost::algorithm::join(fitFunction()->getParameterNames(), ",");

  auto processAlg =
      AlgorithmManager::Instance().create("ProcessIndirectFitParameters");
  processAlg->setProperty("InputWorkspace", parameterWSName);
  processAlg->setProperty("ColumnX", "axis-1");
  processAlg->setProperty("XAxisUnit", "MomentumTransfer");
  processAlg->setProperty("ParameterNames", parameterNames);
  processAlg->setProperty("OutputWorkspace", resultWSName);
  return processAlg;
}

IAlgorithm_sptr
JumpFit::deleteWorkspaceAlgorithm(const std::string &workspaceName) {
  auto deleteAlg = AlgorithmManager::Instance().create("DeleteWorkspace");
  deleteAlg->setProperty("Workspace", workspaceName);
  return deleteAlg;
}

IAlgorithm_sptr
JumpFit::renameWorkspaceAlgorithm(const std::string &workspaceToRename,
                                  const std::string &newName) {
  auto renameAlg = AlgorithmManager::Instance().create("RenameWorkspace");
  renameAlg->setProperty("InputWorkspace", workspaceToRename);
  renameAlg->setProperty("OutputWorkspace", newName);
  return renameAlg;
}

IAlgorithm_sptr JumpFit::scaleAlgorithm(const std::string &workspaceToScale,
                                        const std::string &outputName,
                                        double scaleFactor) {
  auto scaleAlg = AlgorithmManager::Instance().create("Scale");
  scaleAlg->initialize();
  scaleAlg->setProperty("InputWorkspace", workspaceToScale);
  scaleAlg->setProperty("OutputWorkspace", outputName);
  scaleAlg->setProperty("Factor", scaleFactor);
  return scaleAlg;
}

void JumpFit::updatePlotOptions() {}

/**
 * Handles mantid plotting
 */
void JumpFit::plotClicked() {
  const auto outWsName = outputWorkspaceName() + "_Workspace";
  IndirectFitAnalysisTab::plotResult(outWsName, "All");
}

/**
 * Handles saving of workspace
 */
void JumpFit::saveClicked() {
  const auto outWsName = outputWorkspaceName() + "_Workspace";
  IndirectFitAnalysisTab::saveResult(outWsName);
}

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt
