#pragma once

#include "FFPlayerBase.hpp"

class VideoDecoder
{
private:
private:
	DataEvent OnDecodeFinished_;
public:
	void Open()
	{

	}

	void Close()
	{

	}

	void Deocde(void *data)
	{

	}
public:
	void setOnDecodeFinished(DataEvent event) { OnDecodeFinished_ = event; }
};
