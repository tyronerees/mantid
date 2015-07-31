#include "MantidQtCustomInterfaces/Indirect/VesuvioFitting.h"

namespace MantidQt {
namespace CustomInterfaces {
VesuvioFitting::VesuvioFitting(QWidget *parent) : VesuvioTab(parent) {
  m_uiForm.setupUi(parent);
}

void VesuvioFitting::setup() {}

bool VesuvioFitting::validate() { return false; }

void VesuvioFitting::run() {}

} // namespace CustomInterfaces
} // namespace MantidQt
