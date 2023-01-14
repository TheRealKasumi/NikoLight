/**
 * @file FseqAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a custom animation from a fseq file.
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
		FseqAnimator(TL::FseqLoader *fseqLoader, const bool loop = false);
		~FseqAnimator();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

	private:
		TL::FseqLoader *fseqLoader;
		bool loop;
	};
}

#endif