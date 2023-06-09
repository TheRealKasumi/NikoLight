/**
 * @file FseqAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::FseqAnimator}.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
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
