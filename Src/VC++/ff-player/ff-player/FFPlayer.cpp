#include "stdafx.h"
#include "FFPlayer.h"


FFPlayer::FFPlayer()
{
	is_opened_ = false;

	stream_ = new FFStream();
	audio_decoder_ = new AudioDecoder();

	video_decoder_ = new VideoDecoder();
	video_decoder_->setOnDecodeFinished( bind(&FFPlayer::video_decoder_OnDecodeFinished, this, placeholders::_1) );

	scheduler_ = new Scheduler();
	scheduler_->setOnTime( bind(&FFPlayer::scheduler_OnTime, this) );
}

FFPlayer::~FFPlayer()
{
	Close();

	delete stream_;
	delete scheduler_;
	delete audio_renderer_;
	delete video_renderer_;
	delete audio_decoder_;
	delete video_decoder_;
}

void FFPlayer::Open(std::string filename)
{
	try {
		stream_->Open(filename);

		audio_decoder_->Open( stream_->getHandle() );
		video_decoder_->Open( stream_->getHandle() );

		audio_renderer_->Open( audio_decoder_->getChannels(), audio_decoder_->getSampleRate() );
		video_renderer_->Open(video_decoder_->getVideoWidth(), video_decoder_->getVideoHeight() );

		stream_->setAudioIndex( audio_decoder_->getStreamIndex() );
		stream_->setVideoIndex( video_decoder_->getStreamIndex() );

		is_opened_ = true;
	}
	catch (char *msg) {
		is_opened_ = false;
		printf("FFPlayer::Open - Error, filename: %s", filename.c_str());
		printf("  %s", msg);
	}
}

void FFPlayer::Close()
{
	Stop();

	audio_decoder_->Close();
	video_decoder_->Close();

	audio_renderer_->Close();
	video_renderer_->Close();

	stream_->Close();
}

void FFPlayer::Play()
{
	scheduler_->Play();
}

void FFPlayer::Stop()
{
	scheduler_->Stop();
}

void FFPlayer::setAudioRenderer(AudioRendererInterface *audio_renderer)
{
	audio_renderer_ = audio_renderer;
}

void FFPlayer::setVideoRenderer(VideoRendererInterface *video_renderer)
{
	video_renderer_ = video_renderer;
}

void FFPlayer::scheduler_OnTime()
{
#ifdef _DEBUG
	//printf("FFPlayer::scheduler_OnTime \n");
#endif // _DEBUG

	if (audio_decoder_->isBusy() == false) {
		EncodedData *data = stream_->ReadAudio();
		if (data != nullptr) audio_decoder_->Decode(data);
	}

	if (audio_renderer_->isBusy() == false) {
		DecodedData *data = audio_decoder_->getDecodedData();
		if (data != nullptr) audio_renderer_->AudioOut(data);
	}

	EncodedData *data = stream_->ReadVideo(audio_decoder_->getPosition());
	if (data != nullptr) video_decoder_->Decode(data);
}

void FFPlayer::video_decoder_OnDecodeFinished(AVFrame *frame)
{
	video_renderer_->VideoOut(frame);
}