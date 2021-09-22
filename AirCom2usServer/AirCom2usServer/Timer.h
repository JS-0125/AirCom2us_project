#pragma once
#include"default.h"
#include"ServerManager.h"

struct TIMER_EVENT {
	int object;
	int e_type;
	chrono::system_clock::time_point start_time;
	int target_id;

	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (start_time > L.start_time);
	}
};

class Timer
{
private:
	static priority_queue <TIMER_EVENT> m_timerQueue;
	static mutex m_timerLock;
public:
	static void AddEvent(int obj, int target_id, OP_TYPE ev_t, int delay_ms);
	void DoTimer();
};

