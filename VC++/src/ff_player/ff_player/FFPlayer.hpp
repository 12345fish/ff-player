#pragma once

#include <string>
#include "RyuMPEG.h"
#include "FFStream.hpp"
#include "Scheduler.hpp"
#include "AudioDecoder.hpp"
#include "VideoDecoder.hpp"
#include "AudioRenderer.hpp"

class FFPlayer
{
private:
	FFStream *stream_;
	AudioRenderer *audio_renderer_;
	VideoDecoder *video_decoder_;
private:
	AudioDecoder *audio_decoder_;
	void audio_decoder_OnDecodeFinished(void *sender, void *data, int size)
	{
		audio_renderer_->Play(data, size);
	}
private:
	Scheduler *scheduler_;
	void scheduler_OnTimer(void *sender)
	{
		if (audio_decoder_->isBusy() || audio_renderer_->isBusy()) return;

		void *frame = stream_->Read();
		if (frame == nullptr) return;

		printf("\r get_frame_position: %d", get_frame_position(frame));

		switch (get_frame_type(frame)) {
			case AUDIO_PACKET: audio_decoder_->Decode(frame); break;
			case VIDEO_PACKET: video_decoder_->Deocde(frame); break;
			default: release_frame(frame);
		}
	}
public:
	FFPlayer()
	{
		stream_ = new FFStream();
		audio_renderer_ = new AudioRenderer();
		video_decoder_ = new VideoDecoder();

		audio_decoder_ = new AudioDecoder();
		audio_decoder_->setOnDecodeFinished(bind(&FFPlayer::audio_decoder_OnDecodeFinished, this, placeholders::_1, placeholders::_2, placeholders::_3));

		scheduler_ = new Scheduler();
		scheduler_->setOnTime( bind(&FFPlayer::scheduler_OnTimer, this, placeholders::_1) );
	}

	~FFPlayer()
	{
		Close();

		delete scheduler_;
		delete stream_;
		delete audio_decoder_;
		delete audio_renderer_;
		delete video_decoder_;
	}
		
	void Open(std::string filename)
	{
		stream_->Open(filename);
		audio_decoder_->Open(stream_->getHandle());
		video_decoder_->Open(stream_->getHandle());
		audio_renderer_->Open(audio_decoder_->getHandle());
	}

	void Close()
	{
		Stop();

		audio_renderer_->Close();
		audio_decoder_->Close();
		video_decoder_->Close();
		stream_->Close();
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