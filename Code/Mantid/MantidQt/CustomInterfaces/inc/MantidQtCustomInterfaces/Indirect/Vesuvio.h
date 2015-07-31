#ifndef MANTIDQTCUSTOMINTERFACES_VESUVIO_H_
#define MANTIDQTCUSTOMINTERFACES_VESUVIO_H_

//----------------------
// Includes
//----------------------
#include "ui_Vesuvio.h"
#include "MantidKernel/ConfigService.h"
#include "MantidQtAPI/UserSubWindow.h"
#include "VesuvioTab.h"

#include <Poco/NObserver.h>

namespace MantidQt {
namespace CustomInterfaces {
/**
TODO

@author Dan Nixon

Copyright &copy; 2010 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
National Laboratory & European Spallation Source

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

class DLLExport Vesuvio : public MantidQt::API::UserSubWindow {
  Q_OBJECT

public:
  /// Enumeration for the index of each tab
  enum TabChoice { LOAD, CORRECTIONS, FITTING };

public:
  /// Default Constructor
  Vesuvio(QWidget *parent = 0);
  /// Destructor
  ~Vesuvio();

  /// Interface name
  static std::string name() { return "VESUVIO"; }
  // This interface's categories.
  static QString categoryInfo() { return "Indirect"; }

  virtual void initLayout();

private slots:
  /// Slot for clicking on the run button
  void runClicked();
  /// Slot for clicking on the hlep button
  void helpClicked();
  /// Slot for clicking on the manage directories button
  void manageUserDirectories();
  /// Slot showing a message box to the user
  void showMessageBox(const QString &message);

private:
  /// Map of tabs indexed by position on the window
  std::map<unsigned int, VesuvioTab *> m_tabs;

  /// User interface form
  Ui::Vesuvio m_uiForm;
};
}
}

#endif
