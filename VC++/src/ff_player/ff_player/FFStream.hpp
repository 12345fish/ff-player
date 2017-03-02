#pragma once

#include <string>
#include <mutex>
#include <ryulib\ThreadQueue.hpp>
#include "RyuMPEG.h"

class FFStream
{
private:
	std::mutex mutex_;
	void *handle_;
public:
	FFStream()
	{
		handle_ = nullptr;
	}

	~FFStream()
	{
		Close();
	}

	void Open(std::string filename)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ != nullptr) close_stream(handle_);

		int error_code;
		char *name = new char[filename.size() + 1];
		strcpy_s(name, filename.size() + 1, filename.c_str());
		handle_ = open_stream(name, &error_code);
		if (error_code != 0)
			throw "FFStream.Open() - can't open file.";
	}

	void Close()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ == nullptr) return;

		close_stream(handle_);
		handle_ = nullptr;
	}

	void *Read()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ == nullptr) return nullptr;

		void *result = read_frame(handle_);
		if (result != NULL) return result;
		else return nullptr;
	}
public:
	void *getHandle() { return handle_;  }
};