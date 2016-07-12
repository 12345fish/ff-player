#pragma once

#include <string>
#include <ryulib/base.hpp>
#include <ryulib/ThreadQueue.hpp>
#include "FFPlayerBase.h"

extern "C" {
#include "libavformat/avformat.h"
}

class FFStream
{
private:
	AVFormatContext *format_ctx_;
	ThreadQueue<AVPacket *> *audio_buffer_;
	ThreadQueue<AVPacket *> *video_buffer_;
	bool read_frame();
private:
	bool is_opened_;
	int position_;
	int audio_index_;
	int video_index_;
public:
	FFStream();
	~FFStream();

	void Open(std::string filename);
	void Close();

	EncodedData *ReadAudio();

	/**
		Read packet from stream where audio data decoding now.
		@param position current position of audio decoder.
	*/
	EncodedData *ReadVideo(int position);
public:
	void *getHandle();
	void setAudioIndex(int value) { audio_index_ = value; };
	void setVideoIndex(int value) { video_index_ = value; };
};

