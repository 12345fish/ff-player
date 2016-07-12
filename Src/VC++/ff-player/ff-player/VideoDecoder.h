#pragma once

#include <ryulib/ThreadQueue.hpp>
#include <ryulib/SimpleThread.hpp>
#include "FFPlayerBase.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class VideoDecoder
{
private:
	AVCodecContext *codec_ctx_;
	AVCodec *codec_;
	AVFrame *frame_;
	int video_width_;
	int video_height_;
	ThreadQueue<EncodedData*> *queue_;
	void decode_video(EncodedData *data);
private:
	SimpleThread *thread_;
	void thread_OnExecute(SimpleThread *thread);
	void thread_OnTerminated(SimpleThread *thread);
private:
	bool is_opened_;
	int stream_index_;
private:
	DecodedFrameEvent OnDecodeFinished_;
public:
	VideoDecoder();
	~VideoDecoder();

	void Open(void *handle);
	void Close();

	void Clear();

	void Decode(EncodedData *data);
public:
	int getStreamIndex() { return stream_index_; };
	int getVideoWidth() { return codec_ctx_->width; }
	int getVideoHeight() { return codec_ctx_->height; }
public:
	void setOnDecodeFinished(DecodedFrameEvent event);
};

