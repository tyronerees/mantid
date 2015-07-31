#include "MantidQtAPI/UserSubWindow.h"
#include "MantidQtCustomInterfaces/Indirect/VesuvioTab.h"

namespace MantidQt {
namespace CustomInterfaces {

//----------------------------------------------------------------------------------------------
/** Constructor
 */
VesuvioTab::VesuvioTab(QWidget *parent) : IndirectTab(parent) {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
VesuvioTab::~VesuvioTab() {}

/**
 * Emits a signal to run a python script using the method in the parent
 * UserSubWindow
 *
 * @param pyInput :: A string of python code to execute
 */
void VesuvioTab::runPythonScript(const QString &pyInput) {
  emit executePythonScript(pyInput, false);
}
}
} // namespace MantidQt
