#ifndef MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILE_H_
#define MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILE_H_

#include "MantidKernel/System.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/MatrixWorkspace.h"

namespace Mantid
{
namespace DataHandling
{

  /** ExportSampleLogsToCSVFile : TODO: DESCRIPTION

    Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge National Laboratory & European Spallation Source

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
class DLLExport ExportSampleLogsToCSVFile : public API::Algorithm {
  public:
    ExportSampleLogsToCSVFile();
    virtual ~ExportSampleLogsToCSVFile();

  private:
    void init();

    void exec();

    void readSampleLogs(
        std::map<std::string, std::vector<Kernel::DateAndTime> > &maptimes,
        std::map<std::string, std::vector<double> > &mapvalues);

    void getProperties();

    /// Input matrix workspace to write logs from
    API::MatrixWorkspace_sptr m_inputWS;

    /// Output file name
    std::string m_outputFileName;

    /// Names of the logs to output
    std::vector<std::string> m_vecLogNames;

    /// Options for header
    bool m_writeHeader;
    std::string m_headerContent;

    /// Time
    double m_timeTol;
  };


} // namespace DataHandling
} // namespace Mantid

#endif /* MANTID_DATAHANDLING_EXPORTSAMPLELOGSTOCSVFILE_H_ */
