#pragma once

#include <functional>

using namespace std;

typedef function<void()> NotifyEvent;
typedef function<void(void *data)> DataEvent;

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

	void *getData() { return data_;  }
	int getSize() { return size_;  }
	int getPosition() { return position_;  }
};