#pragma once

#include "FFPlayerBase.h"

extern "C" {
#include "libavformat/avformat.h"
}

class VideoRendererInterface
{
public:
	virtual void Open(int width, int height) = 0;
	virtual void Close() = 0;
	virtual void Clear() = 0;
	virtual void VideoOut(AVFrame *frame) = 0;
};

