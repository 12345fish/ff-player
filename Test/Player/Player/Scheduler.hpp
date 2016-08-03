#pragma once

#include "FFPlayerBase.hpp"
#include <ryulib\SimpleThread.hpp>
#include <mutex>
#include <condition_variable>

class Scheduler
{
private:
	bool is_running;
	std::mutex mutex_;
	std::condition_variable_any condition_;
private:
	SimpleThread *thread_;

	void thread_OnExecute(SimpleThread *thread)
	{
		while (thread->isTerminated() == false) {
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.wait_for(lock, std::chrono::milliseconds(5));
			}

			if (is_running && (OnTime_ != nullptr)) OnTime_();
		}
	}

	void thread_OnTerminated(SimpleThread *thread)
	{

	}
private:
	NotifyEvent OnTime_;
public:
	Scheduler()
	{
		is_running = false;
		OnTime_ = nullptr;

		thread_ = new SimpleThread(
			bind(&Scheduler::thread_OnExecute, this, placeholders::_1),
			bind(&Scheduler::thread_OnTerminated, this, placeholders::_1)
		);
	}

	~Scheduler()
	{
		thread_->TerminateAndWait();
		delete thread_;
	}

	void WakeUp()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.notify_all();
	}

	void Play()
	{
		is_running = true;
		WakeUp();
	}

	void Stop()
	{
		is_running = false;
		WakeUp();
	}
public:
	void setOnTime(NotifyEvent event) { OnTime_ = event; }
};