#include "stdafx.h"
#include "FFStream.h"


FFStream::FFStream()
{
	is_opened_ = false;
	position_ = 0;

	audio_buffer_ = new ThreadQueue<AVPacket *>();
	video_buffer_ = new ThreadQueue<AVPacket *>();

	av_register_all();
	avformat_network_init();
}

FFStream::~FFStream()
{
	Close();

	delete audio_buffer_;
	delete video_buffer_;
}

void FFStream::Open(std::string filename)
{
	if (avformat_open_input(&format_ctx_, filename.c_str(), NULL, NULL) != 0)
		throw "FFStream::Open - avformat_open_input error!";

	if (avformat_find_stream_info(format_ctx_, NULL) < 0)
		throw "FFStream::Open - avformat_find_stream_info error!";

	for (int i = 0; i < format_ctx_->nb_streams; i++) {
		if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			video_index_ = i;
			break;
		}
	}

	for (int i = 0; i < format_ctx_->nb_streams; i++) {
		if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			audio_index_ = i;
			break;
		}
	}

#ifdef _DEBUG
	printf("pHandle->audioStream: %d \n", audio_index_);
	printf("pHandle->videoStream: %d \n", video_index_);
#endif // _DEBUG

	is_opened_ = true;
	position_ = 0;
}

void FFStream::Close()
{
	if (is_opened_) {
		is_opened_ = false;
		position_ = 0;

		avformat_close_input(&format_ctx_);
	}
}

EncodedData *FFStream::ReadAudio()
{
	while (audio_buffer_->is_empty()) {
		if (read_frame() == false) break;
	}

	AVPacket *packet = nullptr;

	if (audio_buffer_->pop(packet) == false) return nullptr;

	return new EncodedData(packet, position_);
}

EncodedData *FFStream::ReadVideo(int position)
{
	AVPacket *packet = video_buffer_->front();
	if (packet == NULL) return nullptr;

	int pos = (int) packet->pts * av_q2d(format_ctx_->streams[packet->stream_index]->time_base) * 1000;
	if (pos < position) return nullptr;

	if (video_buffer_->pop(packet) == false) return nullptr;

	return new EncodedData(packet, pos);
}

void *FFStream::getHandle()
{
	return format_ctx_;
}

bool FFStream::read_frame()
{
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

	if (av_read_frame(format_ctx_, packet) < 0) {
		av_free_packet(packet);
		av_free(packet);
		return false;
	}

	int temp = (int) packet->pts * av_q2d(format_ctx_->streams[packet->stream_index]->time_base) * 1000;
	if (temp > 0) position_ = temp;

#ifdef _DEBUG
	//printf("FFStream::read_frame - packet->stream_index: %d, position: %d, packet.size: %d \n", packet->stream_index, position_, packet->size);
#endif // _DEBUG

		if (packet->stream_index == audio_index_) {
			audio_buffer_->push(packet);
		} else if (packet->stream_index == video_index_) {
			video_buffer_->push(packet);
		} else {
			av_free_packet(packet);
			av_free(packet);
		}

	return true;
}