#ifndef MANTIDQTCUSTOMINTERFACES_VESUVIOFITTING_H_
#define MANTIDQTCUSTOMINTERFACES_VESUVIOFITTING_H_

#include "ui_VesuvioFitting.h"
#include "VesuvioTab.h"
#include "MantidAPI/ExperimentInfo.h"

#include <QComboBox>
#include <QMap>
#include <QStringList>

namespace MantidQt {
namespace CustomInterfaces {
class DLLExport VesuvioFitting : public VesuvioTab {
  Q_OBJECT

public:
  VesuvioFitting(QWidget *parent = 0);

protected:
  void setup();
  bool validate();
  void run();

private:
  /// The UI form
  Ui::VesuvioFitting m_uiForm;
};
} // namespace CustomInterfaces
} // namespace MantidQt

#endif
