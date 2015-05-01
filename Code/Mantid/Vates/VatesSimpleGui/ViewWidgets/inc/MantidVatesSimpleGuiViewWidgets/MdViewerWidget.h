#ifndef MDVIEWERWIDGET_H_
#define MDVIEWERWIDGET_H_

#include "ui_MdViewerWidget.h"
#include "MantidVatesSimpleGuiViewWidgets/WidgetDllOption.h"
#include "MantidVatesSimpleGuiViewWidgets/RebinAlgorithmDialogProvider.h"
#include "MantidVatesSimpleGuiViewWidgets/RebinnedSourcesManager.h"

#include "MantidQtAPI/VatesViewerInterface.h"
#include "MantidQtAPI/WorkspaceObserver.h"
#include "boost/shared_ptr.hpp"
#include "MantidQtAPI/MdConstants.h"
#include "MantidQtAPI/MdSettings.h"
#include "MantidVatesSimpleGuiViewWidgets/BackgroundRgbProvider.h"

#include <QPointer>
#include <QWidget>
#include <QString>

class pqApplicationSettingsReaction;
class pqLoadDataReaction;
class pqPipelineSource;
class vtkSMDoubleVectorProperty;

class QDragEnterEvent;
class QDropEvent;
class QAction;
class QEvent;
class QHBoxLayout;
class QObject;
class QString;

namespace Mantid
{
namespace Vates
{
namespace SimpleGui
{

class RotationPointDialog;
class SaveScreenshotReaction;
class ViewBase;
class RebinDialog;
/**
 *
  This class represents the central widget for handling VATES visualization
  operations for 3D and 4D datasets.

  @date 11/08/2011

  Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge National Laboratory & European Spallation Source

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
class EXPORT_OPT_MANTIDVATES_SIMPLEGUI_VIEWWIDGETS MdViewerWidget : public MantidQt::API::VatesViewerInterface, MantidQt::API::WorkspaceObserver
{
  Q_OBJECT

public:
  /// Plugin mode constructor.
  MdViewerWidget();
  /// Standalone mode constructor.
  MdViewerWidget(QWidget *parent);
  /// Default destructor.
  virtual ~MdViewerWidget();

  /// Add extra menus for standalone mode.
  void addMenus();
  /// Connect data loader.
  void connectLoadDataReaction(QAction *action);
  /// Filter events to check for hide.
  bool eventFilter(QObject *obj, QEvent *ev);
  /// See MantidQt::API::VatesViewerInterface
  void renderWorkspace(QString workspaceName, int workspaceType, std::string instrumentName);
  /// See MantidQt::API::VatesViewerInterface
  void setupPluginMode();

public slots:
  /// Seet MantidQt::API::VatesViewerInterface
  void shutdown();

protected slots:
  /// Check for certain updates when an accept is fired.
  void checkForUpdates();
  /// Turn on/off the LOD threshold.
  void onLodToggled(bool state);
  /// Pop-up the rotation point dialog.
  void onRotationPoint();
  /// Show the wiki help in a browser.
  void onWikiHelp();
  /// Load and render data.
  void onDataLoaded(pqPipelineSource *source);
  /// Perform actions when rendering is done.
  void renderingDone();
  /// Execute view switch.
  void switchViews(ModeControlWidget::Views v);
  /// Triggered when panel is changed.
  void panelChanged();
  /// On rebin 
  void onRebin(std::string algorithmType);
  /// On  unbin
  void onUnbin();
  /// On switching an MDEvent source to a temporary source.
  void onSwitchSoures(std::string rebinnedWorkspaceName, std::string sourceType);
protected:
  /// Handle workspace preDeletion tasks.
  void preDeleteHandle(const std::string &wsName,
                       const boost::shared_ptr<Mantid::API::Workspace> ws);
  /// Handle workspace replacement tasks.
  void afterReplaceHandle(const std::string &wsName,
                          const boost::shared_ptr<Mantid::API::Workspace> ws);
  /// Detects if something is dragged onto the VSI
  void dragEnterEvent(QDragEnterEvent *e);
 /// Reacts to something being dropped onto the VSI
 void dropEvent(QDropEvent *e);
private:
  Q_DISABLE_COPY(MdViewerWidget)
  QString m_widgetName;
  ViewBase *currentView; ///< Holder for the current view
  pqLoadDataReaction *dataLoader; ///< Holder for the load data reaction
  ViewBase *hiddenView; ///< Holder for the view that is being switched from
  double lodThreshold; ///< Default value for the LOD threshold (5 MB)
  QAction *lodAction; ///< Holder for the LOD threshold menu item
  bool pluginMode; ///< Flag to say widget is in plugin mode
  RotationPointDialog *rotPointDialog; ///< Holder for the rotation point dialog
  SaveScreenshotReaction *screenShot; ///< Holder for the screen shot reaction
  Ui::MdViewerWidgetClass ui; ///< The MD viewer's UI form
  QHBoxLayout *viewLayout; ///< Layout manager for the view widget
  pqApplicationSettingsReaction *viewSettings; ///< Holder for the view settings reaction
  bool viewSwitched;
  ModeControlWidget::Views initialView; ///< Holds the initial view
  MantidQt::API::MdSettings mdSettings;///<Holds the MD settings which are used to persist data
  MantidQt::API::MdConstants mdConstants;/// < Holds the MD constants
  RebinAlgorithmDialogProvider m_rebinAlgorithmDialogProvider; ///<Provides dialogs to execute rebin algorithms
  RebinnedSourcesManager m_rebinnedSourcesManager; ///<Holds the rebinned sources manager
  QString m_rebinnedWorkspaceIdentifier; ///< Holds the identifier for temporary workspaces

  /// Setup color selection widget connections.
  void connectColorSelectionWidget();
  /// Setup connections for all dialogs.
  void connectDialogs();
  /// Setup rotation point dialog connections.
  void connectRotationPointDialog();
  /// Add view specific stuff to a menu.
  void createMenus();
  /// Disconnect dialog connections.
  void disconnectDialogs();
  /// Consolidate constructor related items.
  void internalSetup(bool pMode);
  /// Perform first render and final setup for mode buttons.
  void renderAndFinalSetup();
  /// Set the signals/slots for the ParaView components based on the view.
  void setParaViewComponentsForView();
  /// Run the necessary setup for the main view.
  void setupMainView();
  /// Creates the UI and mode switch connection.
  void setupUiAndConnections();
  /// Create the requested view.
  ViewBase *setMainViewWidget(QWidget *container, ModeControlWidget::Views v);
  /// Helper function to swap current and hidden view pointers.
  void swapViews();
  /// Update the state of application widgets.
  void updateAppState();
  /// Get the initial view for the current workspace and user setting
  ModeControlWidget::Views getInitialView(int workspaceType, std::string instrumentName);
  /// Check that the view is valid for teh workspace type
  ModeControlWidget::Views checkViewAgainstWorkspace(ModeControlWidget::Views view, int workspaceType);
  /// Get the technique associated with an instrument.
  const std::string getTechniqueForInstrument(const std::string& instrumentName) const;
  /// Get the view for a specified instrument
  QString getViewForInstrument(const std::string& instrument) const;
  /// Check if a technique contains a keyword
  bool checkIfTechniqueContainsKeyword(const std::set<std::string>& techniques, const std::string& keyword) const;
  /// Reset the current view to the appropriate initial view.
  void resetCurrentView(int workspaceType, const std::string& instrumentName);
  /// Render rebinned workspace
  pqPipelineSource* prepareRebinnedWorkspace(const std::string rebinnedWorkspaceName, std::string sourceType); 
  /// Handle drag and drop of peaks workspcaes
  void handleDragAndDropPeaksWorkspaces(QEvent* e, QString text, QStringList& wsNames);
  /// Set up the default color for the background of the view.
  void setColorForBackground();
  /// Render the original workspace
  pqPipelineSource* renderOriginalWorkspace(const std::string originalWorkspaceName);
  /// Remove the rebinning when switching views or otherwise.
  void removeRebinning(pqPipelineSource* source, bool forced, ModeControlWidget::Views view = ModeControlWidget::STANDARD);
  /// Remove all rebinned sources
  void removeAllRebinning(ModeControlWidget::Views view);
  /// Sets a listener for when sources are being destroyed
  void setDestroyedListener();
};

} // SimpleGui
} // Vates
} // Mantid

#endif // MDVIEWERWIDGET_H_
