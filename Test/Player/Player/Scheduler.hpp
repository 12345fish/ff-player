#pragma once

#include "FFPlayerBase.hpp"

class Scheduler
{
private:
private:
	NotifyEvent OnTime_;
public:
	void Play()
	{

	}

	void Stop()
	{

	}
public:
	void setOnTime(NotifyEvent event) { OnTime_ = event; }
};