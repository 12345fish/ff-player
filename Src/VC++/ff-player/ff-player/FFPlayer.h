#pragma once

#include <string>
#include <ryulib/base.hpp>
#include "FFPlayerBase.h"
#include "FFStream.h"
#include "Scheduler.h"
#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "AudioRendererInterface.h"
#include "VideoRendererInterface.h"

extern "C" {
#include "libavformat/avformat.h"
}

class FFPlayer
{
private:
	FFStream *stream_;
	AudioDecoder *audio_decoder_;
	AudioRendererInterface *audio_renderer_;
	VideoRendererInterface *video_renderer_;
private:
	VideoDecoder *video_decoder_;
	void video_decoder_OnDecodeFinished(AVFrame *frame);
private:
	Scheduler *scheduler_;
	void scheduler_OnTime();
private:
	bool is_opened_;
public:
	FFPlayer();
	~FFPlayer();

	void Open(std::string filename);
	void Close();
	void Play();
	void Stop();

	void setAudioRenderer(AudioRendererInterface *audio_renderer);
	void setVideoRenderer(VideoRendererInterface *video_renderer);
};

