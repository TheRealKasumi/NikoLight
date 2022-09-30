/**
 * @file FseqAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::FseqAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/FseqAnimator.h"

/**
 * @brief Create a new instance of {@link TesLight::FseqAnimator}.
 */
TesLight::FseqAnimator::FseqAnimator()
{
	this->fseqLoader = nullptr;
	this->loop = false;
}

/**
 * @brief Create a new instance of {@link TesLight::FseqAnimator}.
 * @param fseqLoader reference to a {@link TesLight::FseqLoader} instance
 */
TesLight::FseqAnimator::FseqAnimator(TesLight::FseqLoader *fseqLoader, const bool loop)
{
	this->fseqLoader = fseqLoader;
	this->loop = loop;
}

/**
 * @brief Destroy the {@link TesLight::FseqAnimator}.
 */
TesLight::FseqAnimator::~FseqAnimator()
{
}

/**
 * @brief Set the looping function to on/off.
 * @param loop on or off
 * @param loop loop the animation
 */
void TesLight::FseqAnimator::setLoop(const bool loop)
{
	this->loop = loop;
}

/**
 * @brief Set the reference to a {@link TesLight::FseqLoader} instance
 * @param fseqLoader reference to the {@link TesLight::FseqLoader} instance
 */
void TesLight::FseqAnimator::setFseqLoader(TesLight::FseqLoader *fseqLoader)
{
	this->fseqLoader = fseqLoader;
}

/**
 * @brief Initialize the {@link FseqAnimator}.
 */
void TesLight::FseqAnimator::init()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i] = CRGB::Black;
	}
}

/**
 * @brief Render the values from the fseq file to the {@link TesLight::Pixel} array.
 */
void TesLight::FseqAnimator::render()
{
	if (this->fseqLoader == nullptr)
	{
		return;
	}

	if (this->fseqLoader->available() < this->pixelCount)
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

	if (!this->fseqLoader->readPixelbuffer(this->pixels, this->pixelCount))
	{
		for (uint16_t i = 0; i < this->pixelCount; i++)
		{
			this->pixels[i] = CRGB::Black;
		}
	}

	if (this->reverse)
	{

		for (uint16_t i = 0, j = this->pixelCount - 1; i < j; i++, j--)
		{
			CRGB temp = this->pixels[i];
			this->pixels[i] = this->pixels[j];
			this->pixels[j] = temp;
		}
	}

	this->applyBrightness();
}
