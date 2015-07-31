#ifndef MANTIDQTCUSTOMINTERFACES_VESUVIOLOAD_H_
#define MANTIDQTCUSTOMINTERFACES_VESUVIOLOAD_H_

#include "ui_VesuvioLoad.h"
#include "VesuvioTab.h"
#include "MantidAPI/ExperimentInfo.h"

#include <QComboBox>
#include <QMap>
#include <QStringList>

namespace MantidQt {
namespace CustomInterfaces {
class DLLExport VesuvioLoad : public VesuvioTab {
  Q_OBJECT

public:
  VesuvioLoad(QWidget *parent = 0);

protected:
  void setup();
  bool validate();
  void run();

private:
  /// The UI form
  Ui::VesuvioLoad m_uiForm;
};
} // namespace CustomInterfaces
} // namespace MantidQt

#endif
