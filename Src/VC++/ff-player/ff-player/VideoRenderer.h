#pragma once

#include <SDL.h>
#include "FFPlayerBase.h"
#include "VideoRendererInterface.h"

extern "C" {
#include "libavformat/avformat.h"
}

class VideoRenderer :
	public VideoRendererInterface
{
private:
	int width_;
	int height_;
	SDL_Window *window_;
	SDL_Surface *surface_;
	SDL_Renderer *renderer_;
	SDL_Texture *texture_;
public:
	VideoRenderer();
	~VideoRenderer();

	void Open(int width, int height);
	void Close();

	void Clear();

	void VideoOut(AVFrame *frame);
};

