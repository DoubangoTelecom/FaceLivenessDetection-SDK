/* Copyright (C) 2011-2021 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
* WebSite: https://www.doubango.org/webapps/face-liveness/
*/

#include <FLD-SDK-API-PUBLIC.h>

#include "fld_image_utils.h"

using namespace FaceLiveness;

static void printUsage(const std::string& message = "");

/*
* Entry point
*/
int main(int argc, char *argv[])
{
	// Parsing args
	std::map<std::string, std::string > args;
	if (!fldParseArgs(argc, argv, args)) {
		printUsage();
		return -1;
	}
	if (args.find("--assets") == args.end()) {
		printUsage("--assets required");
		return -1;
	}
	bool rawInsteadOfJSON = false;
	if (args.find("--json") != args.end()) {
		rawInsteadOfJSON = (args["--json"].compare("true") != 0);
	}

	// Build JSON string
	std::string jsonConfig;
	if (args.find("--assets") != args.end()) {
		jsonConfig += std::string("\"assets_folder\": \"") + args["--assets"] + std::string("\"");
	}
	if (args.find("--type") != args.end()) {
		jsonConfig += (jsonConfig.empty() ? "" : ",")
			+ std::string("\"host_type\": \"") + args["--type"] + std::string("\"");
	}
	jsonConfig = "{" + jsonConfig + "}";

	// Initialize the engine
	FLD_SDK_ASSERT(FldSdkEngine::init(jsonConfig.c_str()).isOK());

	// Request runtime license key
	const FldSdkResult result = FldSdkEngine::requestRuntimeLicenseKey(rawInsteadOfJSON);
	if (result.isOK()) {
		FLD_SDK_PRINT_INFO("\n\n%s\n\n",
			result.json()
		);
	}
	else {
		FLD_SDK_PRINT_ERROR("\n\n*** Failed: code -> %d, phrase -> %s ***\n\n",
			result.code(),
			result.phrase()
		);
	}
	
	FLD_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	// DeInitialize the engine
	FLD_SDK_ASSERT(FldSdkEngine::deInit().isOK());

	return 0;
}

/*
* Print usage
*/
static void printUsage(const std::string& message /*= ""*/)
{
	if (!message.empty()) {
		FLD_SDK_PRINT_ERROR("%s", message.c_str());
	}

	FLD_SDK_PRINT_INFO(
		"\n********************************************************************************\n"
		"runtimeKey\n"
		"\t[--json <json-output:bool>] \n"
		"\t--assets <path-to-assets-folder> \n"
		"\t--type <license-type> \n"
		"\n"
		"Options surrounded with [] are optional.\n"
		"\n"
		"--json: Whether to output the runtime license key as JSON string instead of raw string. Default: true.\n"
		"--assets: Path to the assets folder containing the configuration files and models.\n"
		"--type: Defines how the license is attached to the machine/host. Possible values are 'aws-instance', 'aws-byol', 'azure-instance' or 'azure-byol'. Default: null.\n"
		"********************************************************************************\n"
	);
}
