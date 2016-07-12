#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <ryulib/base.hpp>
#include "FFPlayerBase.h"

// TODO: Play, Stop, OnTime ��� �����ٷ����� ����
// TODO: �����ϸ� �ٷ� �ʱ� ���۸� ä��� �۾� (push while not Decoder.isBusy)

class Scheduler
{
private:
	// TOTO: SimpleThread Class�� ��ü
	std::thread thread_;
	std::mutex mutex_;
	std::condition_variable_any condition_;
private:
	bool is_terminated_;
	bool is_running;
private:
	NotifyEvent OnTime_;
public:
	Scheduler();
	~Scheduler();

	void Terminate();
	void WakeUp();

	void Play();
	void Stop();
public:
	void setOnTime(NotifyEvent event);
};

