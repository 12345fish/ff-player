#pragma once

#include <ryulib\base.hpp>
#include <ryulib/SimpleThread.hpp>
#include <ryulib/ThreadQueue.hpp>
#include <ryulib/SuspensionQueue.hpp>

#define BUFFER_SIZE (16 * 1024)

class AudioDecoder
{
private:
	std::mutex mutex_;
	void *handle_;
	void *buffer_;
	int size_out_;
	SuspensionQueue<void *> *queue_;
private:
	SimpleThread *thread_;
	void do_Decode(void *frame)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ == nullptr) return;

		decode_audio(handle_, frame, buffer_, &size_out_);
	}
	void thread_OnExecute(SimpleThread *thread)
	{
		while (thread->isTerminated() == false) {
			size_out_ = 0;

			void *frame = queue_->pop();
			do_Decode(frame);
			release_frame(frame);

			if ((size_out_ > 0) && (OnDecodeFinished_ != nullptr)) OnDecodeFinished_(this, buffer_, size_out_);
		}
	}
private:
	DataEvent OnDecodeFinished_;
public:
	AudioDecoder()
	{
		handle_ = nullptr;

		buffer_ = malloc(BUFFER_SIZE);

		queue_ = new SuspensionQueue<void *>();

		thread_ = new SimpleThread(
			bind(&AudioDecoder::thread_OnExecute, this, placeholders::_1)
		);
	}

	~AudioDecoder()
	{
		thread_->TerminateNow();

		Close();

		free(buffer_);

		delete queue_;
		delete thread_;
	}

	void Open(void *stream_handle)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ != nullptr) close_audio(handle_);

		int error_code;
		handle_ = open_audio(stream_handle, &error_code);
		if (error_code != 0)
			throw "AudioDecoder.Open() - can't open audio codec.";
	}

	void Close()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (handle_ == nullptr) return;

		close_audio(handle_);
		handle_ = nullptr;
	}

	void Decode(void *frame)
	{
		if (handle_ != nullptr) queue_->push(frame);
	}
public:
	void *getHandle() { return handle_; }
	bool isBusy() { return queue_->size() > 0; }
	void setOnDecodeFinished(DataEvent event) { OnDecodeFinished_ = event; }
};