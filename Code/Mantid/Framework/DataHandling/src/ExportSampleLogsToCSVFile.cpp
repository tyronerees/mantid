#include "MantidDataHandling/ExportSampleLogsToCSVFile.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/FileProperty.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/TimeSeriesProperty.h"

#include <sstream>

namespace Mantid
{
namespace DataHandling
{

  using namespace Mantid;
  using namespace Mantid::API;
  using namespace Mantid::Kernel;

  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  ExportSampleLogsToCSVFile::ExportSampleLogsToCSVFile()
  {
  }

  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  ExportSampleLogsToCSVFile::~ExportSampleLogsToCSVFile()
  {
  }

  //----------------------------------------------------------------------------------------------
  /** Init
   */
  void ExportSampleLogsToCSVFile::init()
  {
    declareProperty(new WorkspaceProperty<MatrixWorkspace>("InputWorkspace", "",
                                                           Direction::Input),
                     "Name of data workspace containing sample logs to be exported. ");

    std::vector<std::string> outputformats;
    outputformats.push_back(".txt");
    outputformats.push_back(".csv");
    declareProperty(new FileProperty("OutputFilename", "",
                                     API::FileProperty::Save, outputformats),
                    "Name of the output sample environment log file name.");

    // Sample log names
    declareProperty(new ArrayProperty<std::string>("SampleLogNames"),
                    "Names of sample logs to be exported in a same file.");

    // Header
    declareProperty("WriteHeaderFile", false, "Flag to generate a sample log header file.");

    declareProperty("Header", "", "String in the header file.");

    // Time zone
    std::vector<std::string> timezones;
    timezones.push_back("UTC");
    timezones.push_back("America/New_York");
    timezones.push_back("Asia/Shanghai");
    timezones.push_back("Australia/Sydney");
    timezones.push_back("Europe/London");
    timezones.push_back("GMT+0");
    timezones.push_back("Europe/Paris");
    timezones.push_back("Europe/Copenhagen");

    auto timezonevalidator = boost::make_shared<ListValidator<std::string> >(timezones);
    declareProperty("TimeZone",  "America/New_York", timezonevalidator,
                    "Sample logs recorded in NeXus files (in SNS) are in UTC time.  TimeZone "
                    "can allow the algorithm to output the log with local time.");

    // Log time tolerance
    auto nonnegval = boost::make_shared<Kernel::BoundedValidator>();
    nonnegval->setLower(1.0E-10);
    declareProperty(
        "TimeTolerance", 0.01, nonnegval,
        "If any 2 log entries with log times within the time tolerance, "
        "they will be recorded in one line. Unit is second. ");
  }

  //  Get and process properties
  void ExportSampleLogsToCSVFile::getProperties() {
    // Input and output workspace
    m_inputWS = getProperty("InputWorkspace");

    m_outputFileName = getPropertyValue("OutputFilename");
    std::string filedir = getDir(m_outputFileName);
    if (!doesExist(filedir)) {
      std::stringstream ess;
      ess << "Directory " << filedir
          << " does not exist.  File cannot be written.";
      throw std::runtime_error(ess.str());
    }

    // Log names
    m_vecLogNames = getProperty("SampleLogNames");
    if (m_vecLogNames.size() == 0)
      throw std::runtime_error("Sample logs names cannot be empty.");

    // Options for writing
    m_writeHeader = getProperty("WriteHeaderFile");
    m_headerContent = getPropertyValue("Header");
    if (m_writeHeader && m_headerContent.size() == 0) {
      g_log.warning(
          "Header is empty. Thus WriteHeaderFile is forced to be False.");
      m_writeHeader = false;
    }

    m_timeTol = getProperty("TimeTolerance");
  }

  // Read sample logs
  void ExportSampleLogsToCSVFile::readSampleLogs(
      std::map<std::string, std::vector<Kernel::DateAndTime> > &maptimes,
      std::map<std::string, std::vector<double> > &mapvalues) {
    for (size_t i = 0; i < m_vecLogNames.size(); ++i) {
      std::string samplename = m_vecLogNames[i];
      bool logexist = m_inputWS->run().hasProperty(samplename);

      // Skip as log does no exist
      if (!logexist) {
        g_log.warning() << "Specified sample log " << samplename
                        << " does not exist."
                        << "\n";
        continue;
      }

      // Convert to time series property
      Kernel::TimeSeriesProperty<double> *sampleproperty =
          dynamic_cast<Kernel::TimeSeriesProperty<double> *>(
              m_inputWS->run().getProperty(samplename));
      if (!sampleproperty) {
        std::stringstream errmsg;
        errmsg << "TimeSeriesProperty Log " << samplename
               << " does not exist in workspace " << m_inputWS->getName();
        g_log.warning(errmsg.str());
        continue;
      }

      // Put sample log to vector
      std::vector<Kernel::DateAndTime> vectime =
          sampleproperty->timesAsVector();
      std::vector<double> vecvalue = sampleproperty->valuesAsVector();

      // Add to map
      maptimes.insert(std::make_pair(samplename, vectime));
      mapvalues.insert(std::make_pair(samplename, vecvalue));
    }

    if (maptimes.size() == 0)
      g_log.error("None of given log names is found in workspace. ");

    return;
  }

  // Logs are recorded upon the change of the data
  // time tolerance : two log entries within time tolerance will be recorded as
  // one
  void ExportSampleLogsToCSVFile::writeAscynLogFile(
      const std::vector<std::vector<Kernel::DateAndTime> > &veclogtimes,
      const std::vector<std::vector<double> > &veclogvalues,
      const int64_t localtimediff, const double timetol) {
    std::stringstream wbuf;

    size_t numlogs = veclogtimes.size();
    std::vector<size_t> currtimeindexes(numlogs, 0);
    std::vector<size_t> nextlogindexes;

    bool continuewrite(true);
    size_t linecount(0);

    // Get sample log times' information
    size_t maxcount;
    Kernel::DateAndTime mintime, maxtime;
    getLogsInfo(veclogtimes, maxcount, mintime, maxtime);

    /*
      self._maxtimestamp = maxcount
      self._maxtime = maxtime
      self._starttime = mintime
      self._localtimediff = localtimediff
    */

    while (continuewrite) {
      findNextTimeStamp(veclogtimes, currtimeindexes, timetol, nextlogindexes);
      if (nextlogindexes.size() == 0) {
        // No new indexes that can be found: break the loop
        continuewrite = false;
      } else {
        std::string templine = writeNewLine(veclogtimes, veclogvalues,
                                            currtimeindexes, nextlogindexes);
        wbuf << templine << "\n";
        progressTimeIndexes(currtimeindexes, nextlogindexes);
        ++linecount;
      }

      // validate algorithm
      if (linecount > maxcount)
        throw std::runtime_error("Programming logic error.");

    } // ENDWHILE

    // Remove last \n
    /*
      if wbuf[-1] == "\n":
          wbuf = wbuf[:-1]
     */

    // Write stringstream to file
    /*
      try:
          ofile = open(self._outputfilename, "w")
          ofile.write(wbuf)
          ofile.close()
      except IOError:
          raise NotImplementedError("Unable to write file %s. Check permission."
      % (self._outputfilename))
      */

    return;
  }

  //    Arguments:
  // - nexttimelogindexes : (output) indexes of logs for next time stamp
  void ExportSampleLogsToCSVFile::findNextTimeStamps(
      const std::vector<std::vector<Kernel::DateAndTime> > &veclogtimes,
      const std::vector<size_t> &currtimeindexes, const double timetol,
      std::vector<size_t> &nexttimelogindexes,
      const Kernel::DateAndTime &maxtime) {
    // clear output
    nexttimelogindexes.clear();

    // Initialize
    Kernel::DateAndTime nexttime(maxtime);

       for
         i in xrange(0, len(logtimeslist))
             :
  #skip the None log
               if logtimeslist[i] is None
               : continue timeindex =
                     currtimeindexes[i] if timeindex >= len(logtimeslist[i])
             :
  #skip as out of boundary of log
               continue tmptime = logtimeslist[i][timeindex] self.log().debug(
                         "tmptime type = %s " % (type(tmptime)))

  #difftime = calTimeDiff(tmptime, nexttime)
                                  difftime =
                         (tmptime.totalNanoseconds() -
                          nexttime
                              .totalNanoseconds()) * 1.0E-9 if abs(difftime) <
                         timetol :
  #same...
                         nexttimelogindexes.append(i) elif difftime < 0 :
  #new smaller time
                         nexttime = tmptime nexttimelogindexes[ :] =
                             [] nexttimelogindexes.append(i)
  #ENDIF
  #ENDIF
                 return void ExportSampleLogsToCSVFile::exec() {
        // Read inputs
        getProperties();

        // Read in logs
        std::vector<std::string> vecLogNames;
        std::vector<std::vector<Kernel::DateAndTime> > vecLogTimeVec;
        std::vector<std::vector<double> > vecLogValueVec;
        size_t loglength;
        readSampleLogs(vecLogNames, vecLogTimeVec, vecLogValueVec);

        // Local time difference
        int64_t localtimediff_ns = calLocalTimeDiff(logtimesdict, loglength);

        // Write to log file
        if (vecLogNames.size() == 0) {
          g_log.error("No log is specified or specified correctly for file.");
        }
        if (vecLogNames.size() == 1) {
          // Only 1 log to write to file.  simple case
          writeLogFile(vecLogNames, vecLogTimeVec, vecLogValueVec,
                       localtimediff_ns);
        } else {
          // Need to write more than 1 logs to file
          std::vector<DateAndTime> veclogtimes;
          std::vector<double> veclogvalue;

          for (size_t i = 0; i < m_vecSampleLogNames.size(); ++i) {
            std::string logname = m_vecSampleLogNames[i];
            veclogtimes.push_back(maplogtime[logname]);
            veclogvalue.push_back(maplogvalue[logname]);
          }
          writeAscynLogFile(veclogtimes, veclogvalue, localtimediff_ns,
                            m_timeTolerance);
        }

        // Write header file
        if (m_writeHeader) {
          Kernel::DateAndTime testdatetime =
              m_inputWS->getRun().getProperty("run_start");
          std::string description("Type your description here");
          writeHeaderFile(testdatetime, description);
        }

        return;
      }

} // namespace DataHandling
} // namespace Mantid
