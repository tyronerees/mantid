#include "MantidAPI/CatalogManager.h"
#include "MantidAPI/ICatalogInfoService.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidICat/CatalogAlgorithmHelper.h"
#include "MantidICat/CatalogDownloadDataFiles.h"
#include "MantidKernel/PropertyWithValue.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/ConfigService.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/InternetHelper.h"
#include "MantidKernel/Exception.h"

#include <Poco/Path.h>

#include <fstream>
#include <iomanip>

namespace Mantid {
namespace ICat {
using namespace Kernel;
using namespace API;

DECLARE_ALGORITHM(CatalogDownloadDataFiles)

/// declaring algorithm properties
void CatalogDownloadDataFiles::init() {
  declareProperty(new ArrayProperty<int64_t>("FileIds"),
                  "List of fileids to download from the data server");
  declareProperty(new ArrayProperty<std::string>("FileNames"),
                  "List of filenames to download from the data server");
  declareProperty("DownloadPath", "", "The path to save the downloaded files.");
  declareProperty("Session", "",
                  "The session information of the catalog to use.");
  declareProperty(new ArrayProperty<std::string>(
                      "FileLocations", std::vector<std::string>(),
                      boost::make_shared<NullValidator>(), Direction::Output),
                  "A list of file locations to the catalog datafiles.");
}

/// Execute the algorithm
void CatalogDownloadDataFiles::exec() {
  // Cast a catalog to a catalogInfoService to access downloading functionality.
  auto catalogInfoService =
      boost::dynamic_pointer_cast<API::ICatalogInfoService>(
          API::CatalogManager::Instance().getCatalog(
              getPropertyValue("Session")));
  // Check if the catalog created supports publishing functionality.
  if (!catalogInfoService)
    throw std::runtime_error("The catalog that you are using does not support "
                             "external downloading.");

  // Used in order to transform the archive path to the user's operating system.
  CatalogInfo catalogInfo =
      ConfigService::Instance().getFacility().catalogInfo();

  std::vector<int64_t> fileIDs = getProperty("FileIds");
  std::vector<std::string> fileNames = getProperty("FileNames");

  // Stores the paths to the related files located in the archives (if user has
  // access).
  // Otherwise, stores the path to the downloaded file.
  std::vector<std::string> fileLocations;

  m_prog = 0.0;

  std::vector<int64_t>::const_iterator fileID = fileIDs.begin();
  std::vector<std::string>::const_iterator fileName = fileNames.begin();

  // For every file with the given ID.
  for (; fileID != fileIDs.end(); ++fileID, ++fileName) {
    m_prog += 0.1;
    double prog = m_prog / (double(fileIDs.size()) / 10);

    progress(prog, "getting location string...");

    // The location of the file (on the server) stored in the archives.
    std::string fileLocation = catalogInfoService->getFileLocation(*fileID);

    g_log.debug()
        << "CatalogDownloadDataFiles -> File location before transform is: "
        << fileLocation << std::endl;
    // Transform the archive path to the path of the user's operating system.
    fileLocation = catalogInfo.transformArchivePath(fileLocation);
    g_log.debug()
        << "CatalogDownloadDataFiles -> File location after transform is:  "
        << fileLocation << std::endl;

    // Can we open the file (Hence, have access to the archives?)
    std::ifstream hasAccessToArchives(fileLocation.c_str());
    if (hasAccessToArchives) { 
      g_log.information() << "File (" << *fileName << ") located in archives ("
                          << fileLocation << ")." << std::endl;
      fileLocations.push_back(fileLocation);
    } else {
      g_log.information()
          << "Unable to open file (" << *fileName
          << ") from archive. Beginning to download over Internet."
          << std::endl;
      progress(prog / 2, "getting the url ....");
      // Obtain URL for related file to download from net.
      const std::string url = catalogInfoService->getDownloadURL(*fileID);
      progress(prog, "downloading over internet...");
      const std::string fullPathDownloadedFile =
          doDownloadandSavetoLocalDrive(url, *fileName);
      fileLocations.push_back(fullPathDownloadedFile);
    }
  }

  // Set the fileLocations property
  setProperty("FileLocations", fileLocations);
}

/**
* Checks to see if the file to be downloaded is a datafile.
* @param fileName :: Name of data file to download.
* @returns True if the file is a data file.
*/
bool CatalogDownloadDataFiles::isDataFile(const std::string &fileName) {
  std::string extension = Poco::Path(fileName).getExtension();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 tolower);
  return (extension.compare("raw") == 0 || extension.compare("nxs") == 0);
}

/**
 * Downloads datafiles from the archives, and saves to the users save default
 * directory.
 * @param URL :: The URL of the file to download.
 * @param fileName :: The name of the file to save to disk.
 * @return The full path to the saved file.
 */
std::string CatalogDownloadDataFiles::doDownloadandSavetoLocalDrive(
    const std::string &URL, const std::string &fileName) {

  std::string filepath =
      Poco::Path(getPropertyValue("DownloadPath"), fileName).toString();

  clock_t start;

  InternetHelper inetHelper;
  start = clock();
  try {
    inetHelper.downloadFile(URL,filepath);
  }
  catch (Exception::InternetError& ie)
  {
    // previous approach parsed the error from the response stream
    // Output an appropriate error message from the JSON object returned by
    // the IDS.
    //std::string IDSError =
    //    CatalogAlgorithmHelper().getIDSError(HTTPStatus, responseStream);
    this->cancel();

    g_log.error(ie.what());
    return "";
  }

  //// Save the file to local disk if no errors occurred on the IDS.
  //pathToDownloadedDatafile = saveFiletoDisk(responseStream, fileName);

  clock_t end = clock();
  float diff = float(end - start) / CLOCKS_PER_SEC;
  g_log.information() << "Time taken to download file " << fileName << " is "
                      << std::fixed << std::setprecision(2) << diff
                      << " seconds" << std::endl;

  return filepath;
}

/**
 * This method is used for unit testing purpose.
 * as the Poco::Net library httpget throws an exception when the nd server n/w
 * is slow
 * I'm testing the download from mantid server.
 * as the downlaod method I've written is private I can't access that in unit
 * testing.
 * so adding this public method to call the private downlaod method and testing.
 * @param URL :: URL of the file to download
 * @param fileName :: name of the file
 * @return Full path of where file is saved to
 */
std::string
CatalogDownloadDataFiles::testDownload(const std::string &URL,
                                       const std::string &fileName) {
  return doDownloadandSavetoLocalDrive(URL, fileName);
}
}
}
