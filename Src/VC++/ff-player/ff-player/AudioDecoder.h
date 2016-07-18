#pragma once

#include <thread>
#include <ryulib/ThreadQueue.hpp>
#include <ryulib/SimpleThread.hpp>
#include "FFPlayerBase.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

#define BUFFER_LIMIT 16

class AudioDecoder
{
private:
	AVCodecContext *codec_ctx_;
	AVCodec *codec_;
	SwrContext *swr_ctx_;
	ThreadQueue<EncodedData*> *queue_in_;
	ThreadQueue<DecodedData*> *queue_out_;
	void decode_audio(EncodedData *data);
private:
	SimpleThread *thread_;
	void thread_OnExecute(SimpleThread *thread);
	void thread_OnTerminated(SimpleThread *thread);
private:
	bool is_opened_;
	int stream_index_;
public:
	AudioDecoder();
	~AudioDecoder();

	void Open(void *handle);
	void Close();

	void Clear();

	void Decode(EncodedData *data);

	DecodedData *getDecodedData();
public:
	bool isBusy();
	int getStreamIndex() { return stream_index_; }
	int getPosition();

	// TODO: Remove magic number
	int getChannels() { return 2;  }

	int getSampleRate() { return codec_ctx_->sample_rate; }
};

