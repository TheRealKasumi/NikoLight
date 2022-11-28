/**
 * @file Base64Util.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::Base64Util}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/Base64Util.h"

/**
 * @brief Calculate the length of the base64 encoded data.
 * @param data buffer holding the input data
 * @param length number of bytes to encode
 * @return size_t size of the encoded data in bytes
 */
size_t TesLight::Base64Util::getEncodedSize(uint8_t *data, const size_t length)
{
	size_t encodedLength;
	mbedtls_base64_encode(nullptr, 0, &encodedLength, data, length);
	return encodedLength;
}

/**
 * @brief Calculate the length of the decoded data.
 * @param data buffer holding the encoded data
 * @param length number of encoded bytes
 * @return size_t size of the decoded data in bytes
 */
size_t TesLight::Base64Util::getDecodedSize(uint8_t *data, const size_t length)
{
	size_t decodedLength;
	mbedtls_base64_decode(nullptr, 0, &decodedLength, data, length);
	return decodedLength;
}

/**
 * @brief Encode the input data with base64 and return the string representation of the data.
 * @param data input buffer
 * @param length length of the input buffer
 * @return String base64 encoded data
 */
String TesLight::Base64Util::encode(uint8_t *data, const size_t length)
{
	if (length == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Nothing to encode."));
		return F("BASE64_ERROR");
	}

	size_t encodedLength = getEncodedSize(data, length);
	unsigned char encoded[encodedLength];
	const int result = mbedtls_base64_encode(encoded, encodedLength, &encodedLength, data, length);

	if (result == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode input data because the output buffer is too small."));
		return F("BASE64_ERROR");
	}

	String encodedString;
	encodedString.reserve(encodedLength);
	for (size_t i = 0; i < encodedLength; i++)
	{
		encodedString += (char)encoded[i];
	}

	return encodedString;
}

/**
 * @brief Decode a base64 encoded string and return the decoded data.
 * @param data base64 encoded input string
 * @param length length of the data, will be set to the length of the output data when successful
 * @return uint8_t* pointer to a buffer holding the decoded data
 */
uint8_t *TesLight::Base64Util::decode(const String data, size_t &length)
{
	if (data.length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Nothing to decode."));
		length = 0;
		return nullptr;
	}

	size_t decodedLength = getDecodedSize((uint8_t *)data.c_str(), data.length());
	uint8_t *decoded = new uint8_t[decodedLength];
	const int restult = mbedtls_base64_decode(decoded, decodedLength, &decodedLength, (uint8_t *)data.c_str(), data.length());
	length = decodedLength;

	if (restult == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode input data because the output buffer is too small."));
		delete[] decoded;
		decoded = nullptr;
		length = 0;
	}
	else if (restult == MBEDTLS_ERR_BASE64_INVALID_CHARACTER)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode input data because it contains invalid characters."));
		delete[] decoded;
		decoded = nullptr;
		length = 0;
	}

	return decoded;
}