#pragma once

#include "FFPlayerBase.hpp"
#include <ryulib/SimpleThread.hpp>
#include <ryulib/ThreadQueue.hpp>

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
	AVFormatContext *format_ctx_;
	AVCodecContext *codec_ctx_;
	AVCodec *codec_;
	SwrContext *swr_ctx_;
	ThreadQueue<void *> *queue_in_;
	ThreadQueue<void *> *queue_out_;
	bool is_opened_;
	int stream_index_;
private:
	SimpleThread *thread_;

	void thread_OnExecute(SimpleThread *thread)
	{
		while (thread->isTerminated() == false) {
			while (isBusy()) thread->Sleep(5);

			void *data;

			while (queue_in_->pop(data)) {
				decode_audio(data);
				if (thread->isTerminated()) break;
			}
		}
	}

	void decode_audio(void *data)
	{
		AVPacket *packet = (AVPacket *) data;

#ifdef _DEBUG
		printf("AudioDecoder::decode_audio - packet->size: %d \n", packet->size);
#endif // _DEBUG

		int position = packet->pts * av_q2d(format_ctx_->streams[packet->stream_index]->time_base) * 1000;

		AVFrame *audio_frame = av_frame_alloc();

		int got_frame = 0;
		int converted_size = 0;
		int data_size = 0;

		while (packet->size > 0) {
			int len = avcodec_decode_audio4(codec_ctx_, audio_frame, &got_frame, packet);

			if (len < 0) break;

			packet->size = packet->size - len;
			packet->data = packet->data + len;

			if (got_frame) {
				if (codec_ctx_->sample_fmt != AV_SAMPLE_FMT_S16) {
					data_size = audio_frame->nb_samples * codec_ctx_->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

					uint8_t *buffer = (uint8_t *) malloc(data_size);

					converted_size = swr_convert(
						swr_ctx_,
						&buffer, audio_frame->nb_samples,
						(const uint8_t **) audio_frame->extended_data, audio_frame->nb_samples
					);

					queue_out_->push(new DecodedData(buffer, data_size, position));
				}
				else {
					data_size = av_samples_get_buffer_size(NULL, codec_ctx_->channels, audio_frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
					queue_out_->push(new DecodedData(audio_frame->data[0], data_size, position));
				}
			}
		}

		av_frame_free(&audio_frame);
		av_free_packet(packet);
		av_free(packet);
	}

	void thread_OnTerminated(SimpleThread *thread)
	{
		delete queue_in_;
		delete queue_out_;
	}
public:
	AudioDecoder()
	{
		queue_in_  = new ThreadQueue<void *>();
		queue_out_ = new ThreadQueue<void *>();

		thread_ = new SimpleThread(
			bind(&AudioDecoder::thread_OnExecute, this, placeholders::_1),
			bind(&AudioDecoder::thread_OnTerminated, this, placeholders::_1)
		);
	}

	~AudioDecoder()
	{
		Close();

		delete thread_;
	}

	void Open(void *handle)
	{
		Close();

		format_ctx_ = (AVFormatContext *) handle;

		stream_index_ = -1;
		for (int i = 0; i < format_ctx_->nb_streams; i++) {
			if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
				stream_index_ = i;
				break;
			}
		}

		if (stream_index_ == -1)
			throw "AudioDecoder::Open - stream_index_ == -1";

		codec_ = avcodec_find_decoder(format_ctx_->streams[stream_index_]->codec->codec_id);
		if (codec_ == NULL)
			throw "AudioDecoder::Open - codec_ == NULL";

		codec_ctx_ = avcodec_alloc_context3(codec_);

		if (avcodec_copy_context(codec_ctx_, format_ctx_->streams[stream_index_]->codec) != 0)
			throw "AudioDecoder::Open - avcodec_copy_context";

		if (avcodec_open2(codec_ctx_, codec_, NULL) < 0)
			throw "AudioDecoder::Open - avcodec_open2";

		swr_ctx_ = swr_alloc();

		uint64_t channel_layout = codec_ctx_->channel_layout;
		if (channel_layout == 0) channel_layout = av_get_default_channel_layout(codec_ctx_->channels);

		av_opt_set_int(swr_ctx_, "in_channel_layout", channel_layout, 0);
		av_opt_set_int(swr_ctx_, "in_sample_rate", codec_ctx_->sample_rate, 0);
		av_opt_set_sample_fmt(swr_ctx_, "in_sample_fmt", codec_ctx_->sample_fmt, 0);

		av_opt_set_int(swr_ctx_, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
		av_opt_set_int(swr_ctx_, "out_sample_rate", codec_ctx_->sample_rate, 0);
		av_opt_set_sample_fmt(swr_ctx_, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

		if (swr_init(swr_ctx_) < 0)
			throw "AudioDecoder::Open - swr_init";

		is_opened_ = true;
	}

	void Close()
	{
		if (is_opened_) {
			is_opened_ = false;
			avcodec_close(codec_ctx_);
		}

		if (swr_ctx_ != nullptr) {
			swr_free(&swr_ctx_);
			swr_ctx_ = nullptr;
		}

		Clear();
	}

	void Clear()
	{

	}

	void Decode(void *data)
	{
#ifdef _DEBUG
		printf("AudioDecoder::Decode -  \n");
#endif // _DEBUG

		queue_in_->push(data);
		thread_->WakeUp();
	}

	void *getDecodeData()
	{
		void *data = queue_out_->pop();
		if (data == NULL) data = nullptr;
		return data;
	}
public:
	bool isBusy() { return (queue_in_->size() + queue_out_->size()) >= BUFFER_LIMIT; }
	int getPosition() { return 0; }
};