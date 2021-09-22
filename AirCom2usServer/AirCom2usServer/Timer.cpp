#include "Timer.h"
mutex Timer::m_timerLock;
priority_queue<TIMER_EVENT> Timer::m_timerQueue;

 void Timer::AddEvent(int obj, int target_id, OP_TYPE ev_t, int delay_ms)
{
	TIMER_EVENT ev;
	ev.e_type = ev_t;
	ev.object = obj;
	ev.start_time = chrono::system_clock::now() + chrono::milliseconds(delay_ms);
	ev.target_id = target_id;
	m_timerLock.lock();
	m_timerQueue.push(ev);
	m_timerLock.unlock();
}

 void Timer::DoTimer()
{
	using namespace chrono;

	for (;;) {
		m_timerLock.lock();
		if ((false == m_timerQueue.empty())
			&& (m_timerQueue.top().start_time <= system_clock::now())) {
			TIMER_EVENT ev = m_timerQueue.top();
			m_timerQueue.pop();
			m_timerLock.unlock();

			if (ev.e_type == OP_POINT_MOVE) {
				EX_OVER* ex_over = new EX_OVER;
				ex_over->m_op = OP_POINT_MOVE;
				*reinterpret_cast<int*>(ex_over->m_packetbuf) = ev.target_id;
				ServerManager::PostQueued(ev.object, ex_over->m_over); //PostQueuedCompletionStatus(h_iocp, 1, ev.object, &ex_over->m_over);
			}
		}
		else {
			m_timerLock.unlock();
			this_thread::sleep_for(10ms);
		}
	}
}
