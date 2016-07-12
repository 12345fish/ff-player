#pragma once

#include <functional>

extern "C" {
#include "libavformat/avformat.h"
}

using namespace std;

/**
	���ڵ� �Ǿ� �ִ� ������ ������ �ٷ�ϴ�.
	���� �����ʹ� �������� �ʰ� ���۷����� �����մϴ�.
*/
class EncodedData
{
private:
	AVPacket *packet_;
	int position_;
public:
	EncodedData(AVPacket *packet, int position)
		: packet_(packet), position_(position)
	{
	}

	~EncodedData() 
	{
		if (packet_ != nullptr) {
			av_free_packet(packet_);
			av_free(packet_);
		}
	}
public:
	AVPacket *getPacket() { return packet_;  }
	int getPosition() { return position_;  }
};

/**
	���ڵ� �Ǿ� �ִ� ������ ������ �ٷ�ϴ�.
	���� �����ʹ� �������� �ʰ� ���۷����� �����մϴ�.
*/
class DecodedData
{
private:
	void *data_;
	int size_;
	int position_;
public:
	DecodedData(void *data, int size, int position)
		:data_(data), size_(size), position_(position)
	{
	}

	~DecodedData()
	{
		if (data_ != nullptr) free(data_);
	}
public:
	void *getData() { return data_; }
	int getSize() { return size_;  }
	int getPosition() { return position_; }
};

typedef function<void(DecodedData *data)> DecodedDataEvent;
typedef function<void(AVFrame *frame)> DecodedFrameEvent;
