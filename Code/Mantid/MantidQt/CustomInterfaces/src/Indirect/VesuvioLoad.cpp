#include "MantidQtCustomInterfaces/Indirect/VesuvioLoad.h"

namespace MantidQt {
namespace CustomInterfaces {
VesuvioLoad::VesuvioLoad(QWidget *parent) : VesuvioTab(parent) {
  m_uiForm.setupUi(parent);
}

void VesuvioLoad::setup() {}

bool VesuvioLoad::validate() { return false; }

void VesuvioLoad::run() {}

} // namespace CustomInterfaces
} // namespace MantidQt
