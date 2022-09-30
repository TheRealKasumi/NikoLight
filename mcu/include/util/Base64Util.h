/**
 * @file Base64Util.h
 * @author TheRealKasumi
 * @brief Class to encode and decode base64.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BASE64_UTIL_H
#define BASE64_UTIL_H

#include <Arduino.h>
#include "mbedtls/base64.h"
#include "logging/Logger.h"

namespace TesLight
{
	class Base64Util
	{
	public:
		static size_t getEncodedSize(uint8_t *data, const size_t length);
		static size_t getDecodedSize(uint8_t *data, const size_t length);

		static String encode(uint8_t *data, const size_t length);
		static uint8_t *decode(const String data, size_t &length);

	private:
		Base64Util();
	};
}

#endif