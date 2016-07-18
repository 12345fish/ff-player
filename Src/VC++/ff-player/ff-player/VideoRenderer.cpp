#include "stdafx.h"
#include "VideoRenderer.h"


VideoRenderer::VideoRenderer()
{
}

VideoRenderer::~VideoRenderer()
{
	Close();
}

void VideoRenderer::Open(int width, int height)
{
	width_  = width;
	height_ = height;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw "VideoRenderer::Open - SDL_Init(SDL_INIT_VIDEO) < 0";

	window_ = SDL_CreateWindow(
		"FF-Player",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	surface_ = SDL_GetWindowSurface(window_);
	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);
	SDL_UpdateWindowSurface(window_);
}

void VideoRenderer::Close()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void VideoRenderer::Clear()
{

}

void VideoRenderer::VideoOut(AVFrame *frame)
{
	uint8_t *pixels;
	int pitch, size = width_ * height_;

	SDL_LockTexture(texture_, NULL, (void **) &pixels, &pitch);
	memcpy(pixels, frame->data[0], size);
	memcpy(pixels + size, frame->data[2], size / 4);
	memcpy(pixels + size * 5 / 4, frame->data[1], size / 4);
	SDL_UnlockTexture(texture_);

	SDL_UpdateTexture(texture_, NULL, pixels, pitch);

	// refresh screen
	SDL_RenderClear(renderer_);
	SDL_RenderCopy(renderer_, texture_, NULL, NULL);
	SDL_RenderPresent(renderer_);
}
