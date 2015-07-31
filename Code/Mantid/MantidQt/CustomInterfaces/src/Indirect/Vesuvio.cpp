#include "MantidKernel/ConfigService.h"
#include "MantidQtAPI/HelpWindow.h"
#include "MantidQtAPI/ManageUserDirectories.h"
#include "MantidQtCustomInterfaces/Indirect/Vesuvio.h"
#include "MantidQtCustomInterfaces/Indirect/VesuvioCorrections.h"
#include "MantidQtCustomInterfaces/Indirect/VesuvioFitting.h"
#include "MantidQtCustomInterfaces/Indirect/VesuvioLoad.h"

#include <QDesktopServices>
#include <QUrl>

// Add this class to the list of specialised dialogs in this namespace
namespace MantidQt {
namespace CustomInterfaces {
DECLARE_SUBWINDOW(Vesuvio)
}
}

using namespace MantidQt::CustomInterfaces;

Vesuvio::Vesuvio(QWidget *parent) : UserSubWindow(parent) {}

Vesuvio::~Vesuvio() {}

void Vesuvio::initLayout() {
  m_uiForm.setupUi(this);

  m_tabs.insert(std::make_pair(
      LOAD, new VesuvioLoad(m_uiForm.twVesuvioTabs->widget(LOAD))));
  m_tabs.insert(std::make_pair(
      CORRECTIONS,
      new VesuvioCorrections(m_uiForm.twVesuvioTabs->widget(CORRECTIONS))));
  m_tabs.insert(std::make_pair(
      FITTING, new VesuvioFitting(m_uiForm.twVesuvioTabs->widget(FITTING))));

  // Connect each tab to the actions available in this GUI
  std::map<unsigned int, VesuvioTab *>::iterator iter;
  for (iter = m_tabs.begin(); iter != m_tabs.end(); ++iter) {
    connect(iter->second, SIGNAL(executePythonScript(const QString &, bool)),
            this, SIGNAL(runAsPythonScript(const QString &, bool)));
    connect(iter->second, SIGNAL(showMessageBox(const QString &)), this,
            SLOT(showMessageBox(const QString &)));
    iter->second->setupTab();
  }

  // Connect statements for the buttons shared between all tabs on the Indirect
  // Bayes interface
  connect(m_uiForm.pbRun, SIGNAL(clicked()), this, SLOT(runClicked()));
  connect(m_uiForm.pbHelp, SIGNAL(clicked()), this, SLOT(helpClicked()));
  connect(m_uiForm.pbManageDirs, SIGNAL(clicked()), this,
          SLOT(manageUserDirectories()));
}

/**
 * Slot to run the underlying algorithm code based on the currently selected
 * tab.
 *
 * This method checks the tabs validate method is passing before calling
 * the run method.
 */
void Vesuvio::runClicked() {
  int tabIndex = m_uiForm.twVesuvioTabs->currentIndex();
  m_tabs[tabIndex]->runTab();
}

/**
 * Slot to open a new browser window and navigate to the help page
 * on the wiki for the currently selected tab.
 */
void Vesuvio::helpClicked() {
  MantidQt::API::HelpWindow::showCustomInterface(NULL,
                                                 QString("Indirect_VESUVIO"));
}

/**
 * Slot to show the manage user dicrectories dialog when the user clicks
 * the button on the interface.
 */
void Vesuvio::manageUserDirectories() {
  MantidQt::API::ManageUserDirectories *ad =
      new MantidQt::API::ManageUserDirectories(this);
  ad->show();
  ad->setFocus();
}

/**
 * Slot to wrap the protected showInformationBox method defined
 * in UserSubWindow and provide access to composed tabs.
 *
 * @param message :: The message to display in the message box
 */
void Vesuvio::showMessageBox(const QString &message) {
  showInformationBox(message);
}
