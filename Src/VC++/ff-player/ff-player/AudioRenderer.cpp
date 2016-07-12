#include "stdafx.h"
#include "AudioRenderer.h"

void fill_audio(void *udata, Uint8 *stream, int len)
{
	AudioRenderer *renderer = (AudioRenderer *) udata;
	if (renderer->getData(stream, len) == false) memset(stream, 0, len);
}

AudioRenderer::AudioRenderer()
{
	packet_reader_ = new PacketReader();

	queue_ = new ThreadQueue<DecodedData*>();

	audio_spec.format = AUDIO_S16SYS;
	audio_spec.samples = 1024;
	audio_spec.callback = fill_audio;
	audio_spec.userdata = this;
}

AudioRenderer::~AudioRenderer()
{
	Close();

	delete packet_reader_;
	delete queue_;
}

void AudioRenderer::Open(int channle, int sample_rate)
{
	Clear();

	audio_spec.freq = sample_rate;
	audio_spec.channels = channle;

	if (SDL_OpenAudio(&audio_spec, NULL) < 0) throw "AudioRenderer::Open - Couldn't open audio";

	SDL_PauseAudio(0);
}

void AudioRenderer::Close()
{
	SDL_CloseAudio();
	Clear();
}

void AudioRenderer::Clear()
{
	DecodedData *data;
	while (queue_->pop(data)) delete data;
}

void AudioRenderer::AudioOut(DecodedData *data)
{
#ifdef _DEBUG
//	printf("AudioRenderer::AudioOut - data->getPosition(): %d \n", data->getPosition());
#endif // _DEBUG

	queue_->push(data);
}

bool AudioRenderer::getData(void *data, int size)
{
	DecodedData *decoded_data;

	while (packet_reader_->canRead(size) == false) {
		if (queue_->pop(decoded_data) == false) break;
		packet_reader_->Write(decoded_data->getData(), decoded_data->getSize());
		delete decoded_data;
	}

	if (packet_reader_->canRead(size) == false) return false;

	void *temp = packet_reader_->Read(size);
	if (temp != nullptr) memcpy(data, temp, size);

	return true;
}

bool AudioRenderer::isBusy()
{
	return queue_->size() >= 3;
}
