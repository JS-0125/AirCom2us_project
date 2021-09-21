#include "Timer.h"
mutex Timer::timer_l;
priority_queue<TIMER_EVENT> Timer::timer_queue;

 void Timer::add_event(int obj, int target_id, OP_TYPE ev_t, int delay_ms)
{
	TIMER_EVENT ev;
	ev.e_type = ev_t;
	ev.object = obj;
	ev.start_time = chrono::system_clock::now() + chrono::milliseconds(delay_ms);
	ev.target_id = target_id;
	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
}

 void Timer::do_timer()
{
	using namespace chrono;

	for (;;) {
		timer_l.lock();
		if ((false == timer_queue.empty())
			&& (timer_queue.top().start_time <= system_clock::now())) {
			TIMER_EVENT ev = timer_queue.top();
			timer_queue.pop();
			timer_l.unlock();

			if (ev.e_type == OP_POINT_MOVE) {
				EX_OVER* ex_over = new EX_OVER;
				ex_over->m_op = OP_POINT_MOVE;
				*reinterpret_cast<int*>(ex_over->m_packetbuf) = ev.target_id;
				ServerManager::PostQueued(ev.object, ex_over->m_over); //PostQueuedCompletionStatus(h_iocp, 1, ev.object, &ex_over->m_over);
			}
		}
		else {
			timer_l.unlock();
			this_thread::sleep_for(10ms);
		}
	}
}
