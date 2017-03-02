#pragma once

#include <ryulib\base.hpp>
#include <ryulib\SimpleThread.hpp>
#include <mutex>
#include <condition_variable>

class Scheduler
{
private:
	bool is_running;
private:
	SimpleThread *thread_;
	void thread_OnExecute(SimpleThread *thread)
	{
		while (thread->isTerminated() == false) {
			thread->Sleep(1);
			if (is_running && (OnTime_ != nullptr)) OnTime_(this);
		}
	}
private:
	NotifyEvent OnTime_;
public:
	Scheduler()
	{
		is_running = false;
		OnTime_ = nullptr;

		thread_ = new SimpleThread(
			bind(&Scheduler::thread_OnExecute, this, placeholders::_1)
		);
	}

	~Scheduler()
	{
		thread_->TerminateNow();

		delete thread_;
	}

	void Play()
	{
		is_running = true;
	}

	void Stop()
	{
		is_running = false;
	}

public:
	void setOnTime(NotifyEvent event) { OnTime_ = event; }
};