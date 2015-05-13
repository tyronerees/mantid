#include "MantidDataHandling/ExportSampleLogsToCSVFile.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/FileProperty.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/ListValidator.h"

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
    declareProperty(new FileProperty("OutputFilename", "", FileAction::Output, outputformats),
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
    declareProperty("TimeTolerance", 0.01,
                    "If any 2 log entries with log times within the time tolerance, "
                    "they will be recorded in one line. Unit is second. ");

  }



} // namespace DataHandling
} // namespace Mantid
