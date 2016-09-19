#pragma once

#include <string>
#include "FFPlayerBase.hpp"
#include "FFStream.hpp"
#include "Scheduler.hpp"
#include "AudioDecoder.hpp"
#include "VideoDecoder.hpp"
#include "AudioRenderer.hpp"
#include "VideoRenderer.hpp"

class FFPlayer
{
private:
	FFStream *stream_;
	AudioDecoder *audio_decoder_;
	AudioRenderer *audio_renderer_;
	VideoRenderer *video_renderer_;
private:
	Scheduler *scheduler_;
	void scheduler_OnTimer()
	{
		if (audio_decoder_->isBusy() == false) {
			void *data = stream_->ReadAudio();
			if (data != nullptr) audio_decoder_->Decode(data);
		}

		if (audio_renderer_->isBusy() == false) {
			DecodedData *data = audio_decoder_->getDecodeData();
			if (data != nullptr) audio_renderer_->AudioOut(data);
		}

		int position = audio_decoder_->getPosition();
		void *data = stream_->ReadVideo(position);
		if (data != nullptr) video_decoder_->Deocde(data);
	}
private:
	VideoDecoder *video_decoder_;
	void video_decoder_OnDecodeFinished(void *data)
	{
		video_renderer_->VideoOut(data);
	}
public:
	FFPlayer()
	{
		stream_ = new FFStream();		
		audio_decoder_ = new AudioDecoder();

		video_decoder_ = new VideoDecoder();
		video_decoder_->setOnDecodeFinished( bind(&FFPlayer::video_decoder_OnDecodeFinished, this, placeholders::_1) );

		audio_renderer_ = new AudioRenderer();
		video_renderer_ = new VideoRenderer();

		scheduler_ = new Scheduler();
		scheduler_->setOnTime( bind(&FFPlayer::scheduler_OnTimer, this) );
	}

	void Open(std::string filename)
	{
#ifdef _DEBUG
		printf("FFPlayer::Open - filename: %s \n", filename.c_str());
#endif // _DEBUG

		stream_->Open(filename);

		audio_decoder_->Open(stream_->getHandle());
		video_decoder_->Open();

		audio_renderer_->Open(audio_decoder_->getChannels(), audio_decoder_->getSampleRate());
		video_renderer_->Open();
	}

	void Close()
	{
		Stop();

		stream_->Close();
		audio_decoder_->Close();
		video_decoder_->Close();
		audio_renderer_->Close();
		video_renderer_->Close();
	}

	void Play()
	{
		scheduler_->Play();
	}

	void Stop()
	{
		scheduler_->Stop();
	}
};