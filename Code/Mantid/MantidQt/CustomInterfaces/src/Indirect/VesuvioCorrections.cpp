#include "MantidQtCustomInterfaces/Indirect/VesuvioCorrections.h"

namespace MantidQt {
namespace CustomInterfaces {
VesuvioCorrections::VesuvioCorrections(QWidget *parent) : VesuvioTab(parent) {
  m_uiForm.setupUi(parent);
}

void VesuvioCorrections::setup() {}

bool VesuvioCorrections::validate() { return false; }

void VesuvioCorrections::run() {}

} // namespace CustomInterfaces
} // namespace MantidQt
