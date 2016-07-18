#pragma once

#include "FFPlayerBase.h"
#include <SDL.h>
#include <ryulib/ThreadQueue.hpp>
#include <ryulib/PacketReader.hpp>
#include "AudioRendererInterface.h"

class AudioRenderer :
	public AudioRendererInterface
{
private:
	SDL_AudioSpec audio_spec;
	ThreadQueue<DecodedData*> *queue_;
	PacketReader *packet_reader_;
public:
	AudioRenderer();
	~AudioRenderer();

	void Open(int channle, int sample_rate);
	void Close();

	void Clear();

	void AudioOut(DecodedData *data);
public:
	bool isBusy();
	bool getData(void *data, int size);
};

