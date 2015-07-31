#ifndef MANTID_CUSTOMINTERFACES_VESUVIOTAB_H_
#define MANTID_CUSTOMINTERFACES_VESUVIOTAB_H_

#include "MantidKernel/System.h"
#include "IndirectTab.h"
#include <QSettings>
#include <QWidget>

namespace MantidQt {
namespace CustomInterfaces {
/**
TODO

@author Dan Nixon

Copyright &copy; 2013 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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

class DLLExport VesuvioTab : public IndirectTab {
  Q_OBJECT

public:
  VesuvioTab(QWidget *parent = 0);
  ~VesuvioTab();

signals:
  /// Send signal to parent window to execute python script
  void executePythonScript(const QString &pyInput, bool output);
  /// Send signal to parent window to show a message box to user
  void showMessageBox(const QString &message);

protected:
  virtual void setup() = 0;
  virtual void run() = 0;
  virtual bool validate() = 0;

  void runPythonScript(const QString &pyInput);
};

} // namespace CustomInterfaces
} // namespace Mantid

#endif
