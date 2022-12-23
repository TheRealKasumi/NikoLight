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

#include <vector>
#include "led/animator/LedAnimator.h"
#include "util/FseqLoader.h"

namespace TL
{
	class FseqAnimator : public LedAnimator
	{
	public:
		FseqAnimator();
		FseqAnimator(TL::FseqLoader *fseqLoader, const bool loop = false);
		~FseqAnimator();

		void setFseqLoader(TL::FseqLoader *fseqLoader);
		void setLoop(const bool loop);

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

	private:
		TL::FseqLoader *fseqLoader;
		bool loop;
	};
}

#endif