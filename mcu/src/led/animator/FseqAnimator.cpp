/**
 * @file FseqAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FseqAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/FseqAnimator.h"

/**
 * @brief Create a new instance of {@link TL::FseqAnimator}.
 */
TL::FseqAnimator::FseqAnimator()
{
	this->fseqLoader = nullptr;
	this->loop = false;
}

/**
 * @brief Create a new instance of {@link TL::FseqAnimator}.
 * @param fseqLoader reference to a {@link TL::FseqLoader} instance
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
 * @brief Set the looping function to on/off.
 * @param loop on or off
 * @param loop loop the animation
 */
void TL::FseqAnimator::setLoop(const bool loop)
{
	this->loop = loop;
}

/**
 * @brief Set the reference to a {@link TL::FseqLoader} instance
 * @param fseqLoader reference to the {@link TL::FseqLoader} instance
 */
void TL::FseqAnimator::setFseqLoader(TL::FseqLoader *fseqLoader)
{
	this->fseqLoader = fseqLoader;
}

/**
 * @brief Initialize the {@link FseqAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::FseqAnimator::init(std::vector<CRGB> &pixels)
{
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
}

/**
 * @brief Render the values from the fseq file to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::FseqAnimator::render(std::vector<CRGB> &pixels)
{
	if (this->fseqLoader == nullptr)
	{
		return;
	}

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
