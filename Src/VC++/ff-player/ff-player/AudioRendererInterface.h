#pragma once

#include "FFPlayerBase.h"

class AudioRendererInterface
{
public:
	virtual void Open(int channle, int sample_rate) = 0;
	virtual void Clear() = 0;
	virtual void Close() = 0;
	virtual void AudioOut(DecodedData *data) = 0;
	virtual bool isBusy() = 0;
};

