#ifndef RYULIB_BASE_HPP
#define RYULIB_BASE_HPP

#include <cstring>
#include <functional>

using namespace std;

typedef function<void(void *)> NotifyEvent;
typedef function<void(void *, void *)> PointerEvent;
typedef function<void(void *, int)> IntegerEvent;
typedef function<void(void *, void *, int)> DataEvent;

/**
* (void *data, int size)를 묶어서 사용하기 위한 클래스
*/
class Memory
{
private:
	void *data_;
	int size_;
public:
	Memory(void *adata, int asize)
	{
		size_ = asize;

		if (asize > 0) {
			data_ = malloc(asize);
			memcpy(data_, adata, asize);
		}
		else {
			data_ = nullptr;
		}
	}

	~Memory()
	{
		if (data_ != nullptr) free(data_);
	}

	void *getData() { return data_; }

	int getSize() { return size_; }
};

#endif  // RYULIB_BASE_HPP
