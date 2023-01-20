/**
 * @file main.cpp
 * @author TheRealKasumi
 * @brief Entry point for the TesLight Update Packaging Tool.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <iostream>
#include <filesystem>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#include <fcntl.h>
#endif

#include "TUPFile.h"

// Function declarations
void printHeader();
void printHelp();

/**
 * @brief Entry point of the application.
 * @param argc number of command line arguments
 * @param argv command line argument
 * @return int status code, 0 for success or the error code otherwise
 */
int main(int argc, char *argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	_setmode(_fileno(stdout), _O_U16TEXT);
#else
	std::wcout.sync_with_stdio(false);
	std::wcout.imbue(std::locale("en_US.utf8"));
#endif

	printHeader();
	if (argc != 3)
	{
		printHelp();
		exit(1);
	}

	const std::filesystem::path outputFile = argv[1];
	const std::filesystem::path updateFolder = argv[2];
	if (!std::filesystem::exists(updateFolder) || !std::filesystem::is_directory(updateFolder))
	{
		std::cerr << "The update folder " << updateFolder << " is not valid." << std::endl
				  << std::endl;
		printHelp();
		exit(2);
	}

	// Generate the TUP file
	std::wcout << L"Generate TesLight Update Package from folder: " << updateFolder << std::endl;
	TUPFile tupFile;
	if (!tupFile.generateFromFolder(updateFolder))
	{
		std::cerr << "Failed to generate TesLight Update Package from folder.";
		exit(3);
	}

	// Write the TUP to the disk
	std::wcout << L"Write TesLight Update Package to: " << outputFile << std::endl;
	if (!tupFile.saveToFile(outputFile))
	{
		std::cerr << "Failed to write TesLight Update Package.";
		exit(4);
	}

	std::wcout << L"Nice! The TesLight Update Package created successfully.";
	exit(0);
}

/**
 * @brief Print the header because we can.
 */
void printHeader()
{
	std::wcout << L"████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗    ██╗   ██╗██████╗ ██████╗  █████╗ ████████╗███████╗" << std::endl;
	std::wcout << L"╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝    ██║   ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██╔════╝" << std::endl;
	std::wcout << L"   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║       ██║   ██║██████╔╝██║  ██║███████║   ██║   █████╗  " << std::endl;
	std::wcout << L"   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║       ██║   ██║██╔═══╝ ██║  ██║██╔══██║   ██║   ██╔══╝  " << std::endl;
	std::wcout << L"   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║       ╚██████╔╝██║     ██████╔╝██║  ██║   ██║   ███████╗" << std::endl;
	std::wcout << L"   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝        ╚═════╝ ╚═╝     ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝" << std::endl;
	std::wcout << L"                                                                                                                    " << std::endl;
	std::wcout << L"         ██████╗  █████╗  ██████╗██╗  ██╗ █████╗  ██████╗ ███████╗    ████████╗ ██████╗  ██████╗ ██╗                " << std::endl;
	std::wcout << L"         ██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔══██╗██╔════╝ ██╔════╝    ╚══██╔══╝██╔═══██╗██╔═══██╗██║                " << std::endl;
	std::wcout << L"         ██████╔╝███████║██║     █████╔╝ ███████║██║  ███╗█████╗         ██║   ██║   ██║██║   ██║██║                " << std::endl;
	std::wcout << L"         ██╔═══╝ ██╔══██║██║     ██╔═██╗ ██╔══██║██║   ██║██╔══╝         ██║   ██║   ██║██║   ██║██║                " << std::endl;
	std::wcout << L"         ██║     ██║  ██║╚██████╗██║  ██╗██║  ██║╚██████╔╝███████╗       ██║   ╚██████╔╝╚██████╔╝███████╗           " << std::endl;
	std::wcout << L"         ╚═╝     ╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝       ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝           " << std::endl;
	std::wcout << std::endl;
}

/**
 * @brief Print the help.
 */
void printHelp()
{
	std::wcout << L"This tool will help you to create a TesLight Update Package (TUP). ";
	std::wcout << L"These packages are used to update the TesLight firmware and MicroSD card content. ";
	std::wcout << L"If you want to generate such a file, you need to create a folder first. ";
	std::wcout << L"Here you need to copy all data for the update, including the firmware and the frotnend files. ";
	std::wcout << L"All other data, except user settings, log and animation data is removed from the MicroSD card. ";
	std::wcout << L"This can make the controller unusable without connecting it to your computer again, so be careful. ";
	std::wcout << L"By convention the firmware file for the controller is called 'firmware.bin' and must be in the root of the update folder. ";
	std::wcout << L"Once you copied all files to the update folder, we are ready to go." << std::endl
			   << std::endl;
	std::wcout << L"Please call me again with the following arguments: tupt <output_file> <source_directory>";
}
