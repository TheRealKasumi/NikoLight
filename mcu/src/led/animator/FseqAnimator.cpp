/**
 * @file FseqAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::FseqAnimator}.
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
#include "led/animator/FseqAnimator.h"

/**
 * @brief Create a new instance of {@link NL::FseqAnimator}.
 * @param fseqLoader reference to a {@link NL::FseqLoader} instance
 * @param loop if set to true, the animation will loop
 */
NL::FseqAnimator::FseqAnimator(NL::FseqLoader *fseqLoader, const bool loop)
{
	this->fseqLoader = fseqLoader;
	this->loop = loop;
}

/**
 * @brief Destroy the {@link NL::FseqAnimator}.
 */
NL::FseqAnimator::~FseqAnimator()
{
}

/**
 * @brief Initialize the {@link FseqAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::FseqAnimator::init(NL::LedStrip &ledStrip)
{
	this->fseqLoader->moveToStart();
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the values from the fseq file to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void NL::FseqAnimator::render(NL::LedStrip &ledStrip)
{
	if (this->fseqLoader->available() < ledStrip.getLedCount())
	{
		if (this->loop)
		{
			this->fseqLoader->moveToStart();
		}
		else
		{
			return;
		}
	}

	const NL::FseqLoader::Error fseqError = this->fseqLoader->readLedStrip(ledStrip);
	if (fseqError != NL::FseqLoader::Error::OK)
	{
		for (size_t i = 0; i < ledStrip.getLedCount(); i++)
		{
			ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
		}
	}

	if (this->reverse)
	{
		this->reversePixels(ledStrip);
	}
	this->applyBrightness(ledStrip);
}
