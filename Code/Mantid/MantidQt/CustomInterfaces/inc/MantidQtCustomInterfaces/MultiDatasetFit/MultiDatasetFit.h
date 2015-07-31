#ifndef MULTIDATASETFIT_H_
#define MULTIDATASETFIT_H_

#include "MantidQtCustomInterfaces/DllConfig.h"
#include "MantidQtAPI/UserSubWindow.h"
#include "ui_MultiDatasetFit.h"

namespace Mantid
{
namespace API
{
  class IFunction;
  class IAlgorithm;
  class MatrixWorkspace;
}
}

namespace MantidQt
{

// Forward declarations
namespace MantidWidgets
{
  class FunctionBrowser;
  class FitOptionsBrowser;
}
namespace API
{
  class AlgorithmRunner;
}

namespace CustomInterfaces
{

// Forward declarations
namespace MDF
{
  class DataController;
  class PlotController;
}

/**
 * Class MultiDatasetFitDialog implements a dialog for setting up a multi-dataset fit
 * and displaying the results.
 */
class MANTIDQT_CUSTOMINTERFACES_DLL MultiDatasetFit: public API::UserSubWindow
{
  Q_OBJECT
public:
  /// The name of the interface as registered into the factory
  static std::string name() { return "Multi dataset fitting"; }
  // This interface's categories.
  static QString categoryInfo() { return "General"; }
  /// Constructor
  MultiDatasetFit(QWidget *parent = NULL);
  /// Destructor
  ~MultiDatasetFit();
  /// Get the name of the output workspace
  QString getOutputWorkspaceName() const {return QString::fromStdString(m_outputWorkspaceName);}
  /// Workspace name for the i-th spectrum
  std::string getWorkspaceName(int i) const;
  /// Workspace index of the i-th spectrum
  int getWorkspaceIndex(int i) const;
  /// Get the fitting range for the i-th spectrum
  std::pair<double,double> getFittingRange(int i) const;
  /// Total number of spectra (datasets).
  int getNumberOfSpectra() const;
  /// Display info about the plot.
  void showPlotInfo();
  /// Check that the data sets in the table are valid
  void checkSpectra();
  /// Get value of a local parameter
  double getLocalParameterValue(const QString& parName, int i) const;
  /// Set value of a local parameter
  void setLocalParameterValue(const QString& parName, int i, double value);
  /// Check if a local parameter is fixed
  bool isLocalParameterFixed(const QString& parName, int i) const;
  /// Fix/unfix local parameter
  void setLocalParameterFixed(const QString& parName, int i, bool fixed);

public slots:
  void reset();

private slots:
  void fit();
  void editLocalParameterValues(const QString& parName);
  void finishFit(bool);
  void enableZoom();
  void enablePan();
  void enableRange();
  void checkFittingType();
  void setLogNames();

protected:
  virtual void initLayout();

private:
  void createPlotToolbar();
  boost::shared_ptr<Mantid::API::IFunction> createFunction() const;
  void updateParameters(const Mantid::API::IFunction& fun);
  void showInfo(const QString& text);
  bool eventFilter(QObject *widget, QEvent *evn);
  void showFunctionBrowserInfo();
  void showFitOptionsBrowserInfo();
  void showTableInfo();
  void removeSpectra(QList<int> rows);
  void loadSettings();
  void saveSettings() const;
  void fitSequential();
  void fitSimultaneous();

  /// The form generated by Qt Designer
  Ui::MultiDatasetFit m_uiForm;
  /// Controls the plot and plotted data.
  MDF::PlotController *m_plotController;
  /// Contains all logic of dealing with data sets.
  MDF::DataController *m_dataController;
  /// Function editor
  MantidWidgets::FunctionBrowser *m_functionBrowser;
  /// Browser for setting other Fit properties
  MantidWidgets::FitOptionsBrowser *m_fitOptionsBrowser;
  /// Name of the output workspace
  std::string m_outputWorkspaceName;
  /// Fit algorithm runner
  boost::shared_ptr<API::AlgorithmRunner> m_fitRunner;
};

} // CustomInterfaces
} // MantidQt


#endif /*MULTIDATASETFITDIALOG_H_*/
