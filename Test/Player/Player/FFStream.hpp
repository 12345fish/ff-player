#pragma once

#include <string>
#include <ryulib\ThreadQueue.hpp>

extern "C" {
#include <libavformat\avformat.h>
}

class FFStream
{
private:
	AVFormatContext *format_ctx_;
	ThreadQueue<AVPacket *> *audio_buffer_;
	ThreadQueue<AVPacket *> *video_buffer_;

	bool read_frame()
	{
		AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

		if (av_read_frame(format_ctx_, packet) < 0) {
			av_free_packet(packet);
			av_free(packet);
			return false;
		}

		int temp = packet->pts * av_q2d(format_ctx_->streams[packet->stream_index]->time_base) * 100;
		if (temp > 0) position_ = temp;
		
		if (packet->stream_index == audio_index_) {
			audio_buffer_->push(packet);
		}
		else if (packet->stream_index == video_index_) {
			video_buffer_->push(packet);
		}
		else {
			av_free_packet(packet);
			av_free(packet);
		}

		return false;
	}
private:
	bool is_opened;
	int position_;
	int audio_index_;
	int video_index_;
public:
	FFStream()
	{
		is_opened = false;
		position_ = 0;
		audio_index_ = -1;
		video_index_ = -1;

		audio_buffer_ = new ThreadQueue<AVPacket *>();
		video_buffer_ = new ThreadQueue<AVPacket *>();

		av_register_all();
		avformat_network_init();
	}

	~FFStream()
	{
		Close();
	}

	void Open(std::string filename)
	{
		Close();

		if (avformat_open_input(&format_ctx_, filename.c_str(), NULL, NULL) != 0)
			throw "FFStream::Open - avformat_open_input() != 0";

		if (avformat_find_stream_info(format_ctx_, NULL) < 0)
			throw "FFStream::Open - avformat_find_stream_info(format_ctx_, NULL) < 0";

		for (int i = 0; i < format_ctx_->nb_streams; i++) {
			if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) audio_index_ = i;
			if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) video_index_ = i;
		}

		is_opened = true;
		position_ = 0;
	}

	void Close()
	{
		if (is_opened) avformat_close_input(&format_ctx_);

		is_opened = false;
		position_ = 0;

		Clear();
	}

	void Clear()
	{
		AVPacket *packet;

		while (audio_buffer_->pop(packet)) {
			av_free_packet(packet);
			av_free(packet);
		}

		while (video_buffer_->pop(packet)) {
			av_free_packet(packet);
			av_free(packet);
		}
	}

	void *ReadAudio()
	{
		while (audio_buffer_->is_empty() == false) {
			if (read_frame() == false) break;
		}

		AVPacket *packet;

		if (audio_buffer_->pop(packet) == false) return nullptr;

		return packet;
	}

	void *ReadVideo(int position)
	{
		AVPacket *packet = video_buffer_->front();
		if (packet == NULL) return nullptr;

		int pos = packet->pts * av_q2d(format_ctx_->streams[packet->stream_index]->time_base) * 1000;
		if (pos > position) return nullptr;

		if (video_buffer_->pop(packet) == false) return nullptr;

		return packet;
	}

	void *getHandle() { return format_ctx_;  }
};