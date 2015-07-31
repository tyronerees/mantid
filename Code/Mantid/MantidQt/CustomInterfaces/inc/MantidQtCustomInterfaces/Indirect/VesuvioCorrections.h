#ifndef MANTIDQTCUSTOMINTERFACES_VESUVIOCORRECTIONS_H_
#define MANTIDQTCUSTOMINTERFACES_VESUVIOCORRECTIONS_H_

#include "ui_VesuvioCorrections.h"
#include "VesuvioTab.h"
#include "MantidAPI/ExperimentInfo.h"

#include <QComboBox>
#include <QMap>
#include <QStringList>

namespace MantidQt {
namespace CustomInterfaces {
class DLLExport VesuvioCorrections : public VesuvioTab {
  Q_OBJECT

public:
  VesuvioCorrections(QWidget *parent = 0);

protected:
  void setup();
  bool validate();
  void run();

private:
  /// The UI form
  Ui::VesuvioCorrections m_uiForm;
};
} // namespace CustomInterfaces
} // namespace MantidQt

#endif
