#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <ryulib/base.hpp>
#include "FFPlayerBase.h"

// TODO: Play, Stop, OnTime 모두 스케줄러에서 관리
// TODO: 오픈하면 바로 초기 버퍼를 채우는 작업 (push while not Decoder.isBusy)

class Scheduler
{
private:
	// TOTO: SimpleThread Class로 교체
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

