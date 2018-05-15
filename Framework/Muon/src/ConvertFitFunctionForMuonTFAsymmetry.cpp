//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidMuon/ConvertFitFunctionForMuonTFAsymmetry.h"
#include "MantidAPI/FunctionProperty.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/Workspace_fwd.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/MandatoryValidator.h"

#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/CompositeValidator.h"
#include "MantidAPI/MultiDomainFunction.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/TableRow.h"
#include "MantidKernel/PhysicalConstants.h"
#include "MantidAPI/ADSValidator.h"
#include <cmath>
#include <numeric>
#include <vector>
namespace {
	constexpr double MICROSECONDS_PER_SECOND{ 1000000.0 };
	constexpr double MUON_LIFETIME_MICROSECONDS{
	Mantid::PhysicalConstants::MuonLifetime * MICROSECONDS_PER_SECOND };
	constexpr bool FIX = false; // fix function needs false to work 
}
namespace Mantid {

	namespace Algorithms {

		using namespace Kernel;
		using namespace API;
		using std::size_t;

		// Register the class into the algorithm factory
		DECLARE_ALGORITHM(ConvertFitFunctionForMuonTFAsymmetry)

		/** Initialisation method. Declares properties to be used in algorithm.
		 *
		 */
		void ConvertFitFunctionForMuonTFAsymmetry::init() {
			declareProperty(make_unique<FunctionProperty>("InputFunction"),
				"The fitting function to be converted.");
			// table of name, norms
			// if construct -> read relevant norms into sorted list
			// if Extract -> update values in table
			declareProperty(
				make_unique<API::WorkspaceProperty<API::ITableWorkspace>>(
					"NormalisationTable", "", Direction::InOut),
				"Name of the table containing the normalisations for the asymmetries.");
			// list of workspaces 
			declareProperty(Kernel::make_unique<Kernel::ArrayProperty<std::string>>(
				"WorkspaceList", boost::make_shared<API::ADSValidator>()),
				"An ordered list of workspaces (to get the initial values for the normalisations).");

			std::vector<std::string> allowedModes{ "Construct", "Extract" };
			auto modeVal = boost::make_shared<Kernel::CompositeValidator>();
			modeVal->add(boost::make_shared<Kernel::StringListValidator>(allowedModes));
			modeVal->add(boost::make_shared<Kernel::MandatoryValidator<std::string>>());
			declareProperty("Mode", "Construct", modeVal,
				"Mode to run in. Construct will convert the"
				"input function into one suitable for calculating the"
				" TF Asymmetry. Extract will find the original user function"
				" from a function that is suitable for TF Asymmetry calculations.");

				declareProperty(make_unique<FunctionProperty>("OutputFunction",Direction::Output),
					"The fitting function to be converted.");
		}

		/*
		* Validate the input parameters
		* @returns map with keys corresponding to properties with errors and values
		* containing the error messages.
		*/
		std::map<std::string, std::string>
			ConvertFitFunctionForMuonTFAsymmetry::validateInputs() {
			// create the map
			std::map<std::string, std::string> validationOutput;
			// check start and end times
			/*double startX = getProperty("StartX");
			double endX = getProperty("EndX");
			if (startX > endX) {
			  validationOutput["StartX"] = "Start time is after the end time.";
			} else if (startX == endX) {
			  validationOutput["StartX"] = "Start and end times are equal, there is no "
										   "data to apply the algorithm to.";
			}
			double norm = getProperty("NormalizationIn");
			if (norm < 0.0) {
			  validationOutput["NormalizationIn"] =
				  "Normalization to use must be positive.";
			}*/
			return validationOutput;
		}

		/** Executes the algorithm
		 *
		 */
		void ConvertFitFunctionForMuonTFAsymmetry::exec() {
			auto dfsat = getPropertyValue("InputFunction");

			IFunction_sptr inputFitFunction = FunctionFactory::Instance().createInitialized(
				dfsat);//getProperty("InputFunction");
			auto ttweta = inputFitFunction->asString();
			auto mode = getPropertyValue("Mode");
			if (mode == "Construct") {
				std::vector<double> norms = getNorms();
				auto outputFitFunction = getTFAsymmFitFunction(inputFitFunction, norms);
				setProperty("OutputFunction", outputFitFunction);
			}
			else {

			}
		}
		/*
		Converts from a TF Asymmetry function N(1+f) to the original f.
		*/


		/*
		Convert to a TF Asymmetry function. Input f and changes it to N(1+f)
		*/
		std::vector<double> ConvertFitFunctionForMuonTFAsymmetry::getNorms() {
			API::ITableWorkspace_sptr table = getProperty("NormalisationTable");
			const std::vector<std::string> wsNames = getProperty("WorkspaceList");

			std::vector<double> norms(wsNames.size() ,0);
			for (size_t row = 0; row < table->rowCount(); row++) {
				for (size_t wsPosition = 0; wsPosition < wsNames.size();wsPosition++) {
					if (table->String(row, 0) == wsNames[wsPosition]) {
						norms[wsPosition] = table->Double(row, 1);
					}
				}
			}

			return norms;
		}
		/** Gets the fitting function for TFAsymmetry fit
		* @param original :: The function defined by the user (in GUI)
		* @param norms :: vector of normalization constants
		* @returns :: The fitting function for the TFAsymmetry fit
		*/
		Mantid::API::IFunction_sptr ConvertFitFunctionForMuonTFAsymmetry::getTFAsymmFitFunction(
			Mantid::API::IFunction_sptr original, const std::vector<double> norms) {
			auto multi = boost::make_shared<MultiDomainFunction>();
			auto tmp = boost::dynamic_pointer_cast<MultiDomainFunction>(original);
			size_t numDomains = original->getNumberDomains();
			for (size_t j = 0; j < numDomains; j++) {
				IFunction_sptr userFunc;
				auto constant = FunctionFactory::Instance().createInitialized(
					"name = FlatBackground, A0 = 1.0; ties=(f0.A0=1)");
				if (numDomains == 1) {
					userFunc = original;
				}
				else {
					userFunc = tmp->getFunction(j);
					multi->setDomainIndex(j, j);
				}
				auto inBrace = boost::make_shared<CompositeFunction>();
				inBrace->addFunction(constant);
				inBrace->addFunction(userFunc);
				auto norm = FunctionFactory::Instance().createInitialized(
					"composite=CompositeFunction,NumDeriv=true;name = FlatBackground, A0 "
					"=" +
					std::to_string(norms[j]));
				auto product = boost::dynamic_pointer_cast<CompositeFunction>(
					FunctionFactory::Instance().createFunction("ProductFunction"));
				product->addFunction(norm);
				product->addFunction(inBrace);
				auto composite = boost::make_shared<CompositeFunction>();
				constant = FunctionFactory::Instance().createInitialized(
					"name = ExpDecayMuon, A = 0.0, Lambda = -" + std::to_string(MUON_LIFETIME_MICROSECONDS) + ";ties = (f0.A = 0.0, f0.Lambda = -" + std::to_string(MUON_LIFETIME_MICROSECONDS) + ")");
				composite->addFunction(product);
				composite->addFunction(constant);
				multi->addFunction(composite);
			}
			// if multi data set we need to do the ties manually
			if (numDomains > 1) {
				auto originalNames = original->getParameterNames();
				for (auto name : originalNames) {
					auto index = original->parameterIndex(name);
					auto originalTie = original->getTie(index);
					if (originalTie) {
						auto stringTie = originalTie->asString();
						// change name to reflect new postion
						auto insertPosition = stringTie.find_first_of(".");
						stringTie.insert(insertPosition + 1, "f0.f1.f1.");
						// need to change the other side of =
						insertPosition = stringTie.find_first_of("=");
						insertPosition = stringTie.find_first_of(".", insertPosition);
						stringTie.insert(insertPosition + 1, "f0.f1.f1.");
						multi->addTies(stringTie);
					}
				}
			}

			return boost::dynamic_pointer_cast<IFunction>(multi);
		}
	} // namespace Algorithm
} // namespace Mantid
