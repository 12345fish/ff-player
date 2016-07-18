#include "stdafx.h"
#include "AudioDecoder.h"
#include <functional>

AudioDecoder::AudioDecoder()
{
	is_opened_ = false;
	swr_ctx_ = nullptr;

	queue_in_ = new ThreadQueue<EncodedData*>();
	queue_out_ = new ThreadQueue<DecodedData*>();
	thread_ = new SimpleThread(
		bind(&AudioDecoder::thread_OnExecute, this, placeholders::_1),
		bind(&AudioDecoder::thread_OnTerminated, this, placeholders::_1)
	);
}

void AudioDecoder::thread_OnExecute(SimpleThread *thread)
{
	while (thread->isTerminated() == false) {
		while (isBusy()) thread->Sleep(5);

		EncodedData *data = nullptr;
		
		while (queue_in_->pop(data)) {
			if (thread->isTerminated()) {
				delete data;
				break;
			}

			decode_audio(data);
			delete data;
		}
	}
}

void AudioDecoder::decode_audio(EncodedData *data)
{
	AVFrame *audio_frame = av_frame_alloc();

	int gotFrame = 0;
	int converted_size = 0;
	int data_size = 0;

	while (data->getPacket()->size > 0) {
		int len = avcodec_decode_audio4(codec_ctx_, audio_frame, &gotFrame, data->getPacket());

		if (len < 0) break;

		data->getPacket()->size -= len;
		data->getPacket()->data += len;

		if (gotFrame) {
			if (codec_ctx_->sample_fmt != AV_SAMPLE_FMT_S16) {
				data_size = audio_frame->nb_samples * codec_ctx_->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

				uint8_t *buffer = (uint8_t *) malloc(data_size);

				converted_size = swr_convert(
					swr_ctx_,
					&buffer, audio_frame->nb_samples,
					(const uint8_t **)audio_frame->extended_data, audio_frame->nb_samples
					);

#ifdef _DEBUG
				//printf("AudioDecoder::decode_audio - not AV_SAMPLE_FMT_S16 data_size: %d \n", data_size);
#endif // _DEBUG

				queue_out_->push(new DecodedData(buffer, data_size, data->getPosition()));
			}
			else {
				data_size = av_samples_get_buffer_size(NULL, codec_ctx_->channels, audio_frame->nb_samples, AV_SAMPLE_FMT_S16, 1);

#ifdef _DEBUG
				//printf("AudioDecoder::decode_audio - not AV_SAMPLE_FMT_S16 data_size: %d \n", data_size);
#endif // _DEBUG

				queue_out_->push(new DecodedData(audio_frame->data[0], data_size, data->getPosition()));
			}
		}
	}

	av_frame_free(&audio_frame);
}

void AudioDecoder::thread_OnTerminated(SimpleThread *thread)
{
	delete queue_in_;
	delete queue_out_;
}

AudioDecoder::~AudioDecoder()
{
	Close();

	delete thread_;
}

void AudioDecoder::Open(void *handle)
{
	Clear();

	AVFormatContext *format_ctx = (AVFormatContext *)handle;

	// TODO: 스트림이 하나가 아니거나 아예 없거나 하는 경우에 대한 처리
	stream_index_ = -1;
	for (int i = 0; format_ctx->nb_streams; i++) {
		if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			stream_index_ = i;
			break;
		}
	}

	if (stream_index_ == -1)
		throw "AudioDecoder::Open - stream_index_ == -1";

	codec_ = avcodec_find_decoder(format_ctx->streams[stream_index_]->codec->codec_id);
	if (codec_ == NULL)
		throw "AudioDecoder::Open - codec_ == NULL";

	codec_ctx_ = avcodec_alloc_context3(codec_);

	if (avcodec_copy_context(codec_ctx_, format_ctx->streams[stream_index_]->codec) != 0)
		throw "AudioDecoder::Open - avcodec_copy_context(codec_ctx_, format_ctx->streams[stream_index_]->codec) != 0";

	if (avcodec_open2(codec_ctx_, codec_, NULL) < 0)
	    throw "AudioDecoder::Open - avcodec_open2(codec_ctx_, codec_, NULL) < 0";

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
		throw "AudioDecoder::Open - swr_init(swr_ctx_) < 0";

	is_opened_ = true;
}

void AudioDecoder::Close()
{
	if (is_opened_) {
		is_opened_ = false;
		avcodec_close(codec_ctx_);
		if (swr_ctx_ != nullptr) swr_free(&swr_ctx_);
	}

	Clear();
}

void AudioDecoder::Clear()
{
	EncodedData *encoded_data;
	while (queue_in_->pop(encoded_data)) delete encoded_data;

	DecodedData *decoded_data;
	while (queue_out_->pop(decoded_data)) delete decoded_data;
}

void AudioDecoder::Decode(EncodedData *data)
{
#ifdef _DEBUG
	//printf("AudioDecoder::decode - data->position: %d \n", data->getPosition());
#endif // _DEBUG

	queue_in_->push(data);
	thread_->WakeUp();
}

DecodedData *AudioDecoder::getDecodedData()
{
	DecodedData *data = queue_out_->pop();
	if (data == NULL) data = nullptr;
	return data;
}

bool AudioDecoder::isBusy()
{
#ifdef _DEBUG
	//printf("AudioDecoder::isBusy - buffer_in_->size() + buffer_out_->size(): %d \n", buffer_in_->size() + buffer_out_->size());
#endif // _DEBUG

	return (queue_in_->size() + queue_out_->size()) >= BUFFER_LIMIT;
}

int AudioDecoder::getPosition()
{
	// TODO: 
	return 0;
}

