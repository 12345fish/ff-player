#include "stdafx.h"
#include "Scheduler.h"


Scheduler::Scheduler()
{
	is_terminated_ = false;
	is_running = false;
	OnTime_ = nullptr;

	thread_ = std::thread(
		[&]()
		{
			while (is_terminated_ == false) {
				{
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait_for(lock, std::chrono::milliseconds(5));
				}

				if (is_terminated_) break;

				if (is_running && (OnTime_ != nullptr)) OnTime_();
			}
		}
	);
}

Scheduler::~Scheduler()
{
	Terminate();
	WakeUp();
	thread_.detach();
}

void Scheduler::Terminate()
{
	is_terminated_ = true;
}

void Scheduler::WakeUp()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.notify_all();
}

void Scheduler::Play()
{
	is_running = true;
	WakeUp();
}

void Scheduler::Stop()
{
	is_running = false;
}

void Scheduler::setOnTime(NotifyEvent event)
{
	OnTime_ = event;
}

