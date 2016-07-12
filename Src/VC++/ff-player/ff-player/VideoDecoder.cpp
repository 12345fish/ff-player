#include "stdafx.h"
#include "VideoDecoder.h"


void VideoDecoder::decode_video(EncodedData *data)
{
	int is_frame_finished = 0;
	int result = avcodec_decode_video2(codec_ctx_, frame_, &is_frame_finished, data->getPacket());

#ifdef _DEBUG
	//printf("VideoDecoder::decode_video - data->position: %d, isFrameFinished: %d \n", data->getPosition(), is_frame_finished);
#endif // _DEBUG

	if ((is_frame_finished) && (OnDecodeFinished_ != nullptr)) OnDecodeFinished_(frame_);
}

void VideoDecoder::thread_OnExecute(SimpleThread *thread)
{
	while (thread->isTerminated() == false) {
		EncodedData *data = nullptr;

		while (queue_->pop(data)) {
			if (thread->isTerminated()) {
				delete data;
				break;
			}

			decode_video(data);
			delete data;
		}
	}
}

void VideoDecoder::thread_OnTerminated(SimpleThread *thread)
{
	av_frame_free(&frame_);
	delete queue_;
}

VideoDecoder::VideoDecoder()
{
	is_opened_ = false;
	video_width_ = 0;
	video_height_ = 0;
	OnDecodeFinished_ = nullptr;

	frame_ = av_frame_alloc();

	queue_ = new ThreadQueue<EncodedData*>();
	thread_ = new SimpleThread(
		bind(&VideoDecoder::thread_OnExecute, this, placeholders::_1),
		bind(&VideoDecoder::thread_OnTerminated, this, placeholders::_1)
	);
}

VideoDecoder::~VideoDecoder()
{
	Close();

	delete thread_;
}

void VideoDecoder::Open(void *handle)
{
	AVFormatContext *format_ctx = (AVFormatContext *) handle;

    // TODO: 스트림이 하나가 아니거나 아예 없거나 하는 경우에 대한 처리
	stream_index_ = -1;
	for (int i = 0; format_ctx->nb_streams; i++) {
		if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			stream_index_ = i;
			break;
		}
	}

	if (stream_index_ == -1)
		throw "VideoDecoder::Open - stream_index_ == -1";

	codec_ = avcodec_find_decoder(format_ctx->streams[stream_index_]->codec->codec_id);
	if (codec_ == NULL)
		throw "VideoDecoder::Open - codec_ == NULL";

	codec_ctx_ = avcodec_alloc_context3(codec_);

	if (avcodec_copy_context(codec_ctx_, format_ctx->streams[stream_index_]->codec) != 0)
		throw "VideoDecoder::Open - avcodec_copy_context(codec_ctx_, format_ctx->streams[stream_index_]->codec) != 0";

	if (avcodec_open2(codec_ctx_, codec_, NULL) < 0)
		throw "VideoDecoder::Open - avcodec_open2(codec_ctx_, codec_, NULL) < 0";

	is_opened_ = true;
}

void VideoDecoder::Close()
{
	if (is_opened_) {
		is_opened_ = false;
		avcodec_close(codec_ctx_);
	}

	Clear();
}

void VideoDecoder::Clear()
{
	EncodedData *encoded_data;
	while (queue_->pop(encoded_data)) delete encoded_data;
}

void VideoDecoder::Decode(EncodedData *data)
{
#ifdef _DEBUG
	//printf("VideoDecoder::decode - data->position: %d \n", data->getPosition());
#endif // _DEBUG

	queue_->push(data);
	thread_->WakeUp();
}

void VideoDecoder::setOnDecodeFinished(DecodedFrameEvent event)
{
	OnDecodeFinished_ = event;
}