/**
 * @file UpdateEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to load update packages to the NikoLight controller
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "server/UpdateEndpoint.h"

// Initialize
FS *NL::UpdateEndpoint::fileSystem = nullptr;
File NL::UpdateEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::UpdateEndpoint::begin(FS *_fileSystem)
{
	NL::Logger::log(NL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize update endpoint."));

	NL::Logger::log(NL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create storage directory for package file."));
	NL::UpdateEndpoint::fileSystem = _fileSystem;
	NL::UpdateEndpoint::fileSystem->mkdir(UPDATE_DIRECTORY);

	NL::Logger::log(NL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register update endpoints."));
	NL::WebServerManager::addUploadRequestHandler((getBaseUri() + F("update")).c_str(), http_method::HTTP_POST, NL::UpdateEndpoint::postPackage, NL::UpdateEndpoint::packageUpload);
}

/**
 * @brief Is called after the update package upload.
 */
void NL::UpdateEndpoint::postPackage()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Package upload successful, update will start after reboot."));
	NL::UpdateEndpoint::sendSimpleResponse(200, F("Yay, I received a package. I will unpack it. Can't wait to see what's inside."));

	if (NL::UpdateEndpoint::uploadFile)
	{
		NL::UpdateEndpoint::uploadFile.close();
	}

	// Reboot the controller, update will be installed after the reboot
	NL::Updater::reboot(F("Update"), 3000);
}

/**
 * @brief Upload a new update package to the controller.
 */
void NL::UpdateEndpoint::packageUpload()
{
	HTTPUpload &upload = webServer->upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload update package."));
		NL::UpdateEndpoint::uploadFile = NL::UpdateEndpoint::fileSystem->open((String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME, FILE_WRITE);
		if (!uploadFile)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			NL::UpdateEndpoint::sendSimpleResponse(500, F("Failed to write to file for upload."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if (NL::UpdateEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
			NL::UpdateEndpoint::sendSimpleResponse(500, F("Failed to write chunk to file. Not all bytes were written."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		NL::UpdateEndpoint::uploadFile.close();
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Upload was aborted by the client. The file will be deleted."));
		NL::UpdateEndpoint::uploadFile.close();
		NL::UpdateEndpoint::fileSystem->remove((String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME);
		NL::UpdateEndpoint::sendSimpleResponse(400, F("Upload was aborted by the client. The file will be deleted."));
	}
}
