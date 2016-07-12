#pragma once

#include <functional>

extern "C" {
#include "libavformat/avformat.h"
}

using namespace std;

/**
	인코딩 되어 있는 데이터 정보를 다룹니다.
	실제 데이터는 복사하지 않고 레퍼런스만 유지합니다.
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
	디코딩 되어 있는 데이터 정보를 다룹니다.
	실제 데이터는 복사하지 않고 레퍼런스만 유지합니다.
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
