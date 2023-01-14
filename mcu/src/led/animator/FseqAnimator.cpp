/**
 * @file FseqAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FseqAnimator}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
 * @brief Create a new instance of {@link TL::FseqAnimator}.
 * @param fseqLoader reference to a {@link TL::FseqLoader} instance
 * @param loop if set to true, the animation will loop
 */
TL::FseqAnimator::FseqAnimator(TL::FseqLoader *fseqLoader, const bool loop)
{
	this->fseqLoader = fseqLoader;
	this->loop = loop;
}

/**
 * @brief Destroy the {@link TL::FseqAnimator}.
 */
TL::FseqAnimator::~FseqAnimator()
{
}

/**
 * @brief Initialize the {@link FseqAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::FseqAnimator::init(std::vector<CRGB> &pixels)
{
	this->fseqLoader->moveToStart();
	std::fill(pixels.begin(), pixels.end(), CRGB::Black);
}

/**
 * @brief Render the values from the fseq file to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::FseqAnimator::render(std::vector<CRGB> &pixels)
{
	if (this->fseqLoader->available() < pixels.size())
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

	if (!this->fseqLoader->readPixelBuffer(pixels))
	{
		for (size_t i = 0; i < pixels.size(); i++)
		{
			pixels.at(i) = CRGB::Black;
		}
	}

	if (this->reverse)
	{
		for (uint16_t i = 0, j = pixels.size() - 1; i < j; i++, j--)
		{
			CRGB temp = pixels.at(i);
			pixels.at(i) = pixels.at(j);
			pixels.at(j) = temp;
		}
	}

	this->applyBrightness(pixels);
}
