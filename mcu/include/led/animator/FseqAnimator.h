/**
 * @file FseqAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a custom animation from a fseq file.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FSEQ_ANIMATOR_H
#define FSEQ_ANIMATOR_H

#include "led/animator/LedAnimator.h"
#include "util/FseqLoader.h"

namespace TesLight
{
	class FseqAnimator : public LedAnimator
	{
	public:
		FseqAnimator();
		FseqAnimator(TesLight::FseqLoader *fseqLoader, const bool loop = false);
		~FseqAnimator();

		void setFseqLoader(TesLight::FseqLoader *fseqLoader);
		void setLoop(const bool loop);

		void init();
		void render();

	private:
		TesLight::FseqLoader *fseqLoader;
		bool loop;
	};
}

#endif