#pragma once
#include"Object.h"
#include"AirCom2us_protocol.h"

class Player: public Object
{
private:
	EX_OVER m_recv_over;
	short m_level;
	int m_exp;
	SOCKET m_socket;
	int m_prev_size;
public:
	int m_sessionId;

	Player();

	void Init(int level, int exp, float x, float y);
	void PlayerAccept(HANDLE h_iocp, int c_id);
	bool IsState(OBJECT_STATE plst);
	void Connected(SOCKET &p_socket);
	void ResetInGameData();
	void CloseSocket();
	void CloseSocket(string msg);
	int GetPrevSize();
	void SetPrevSize(int prevSize);
	EX_OVER* GetOverlapped();
	SOCKET* GetSocket();
	bool CheckAbnormalAction(cs_packet_move* packet);

	void operator=(const Player& rhs) {
		m_state.store(rhs.m_state);
		m_id = rhs.m_id;
		m_recv_over = rhs.m_recv_over;
		m_level = rhs.m_level;
		m_exp = rhs.m_exp;
		m_socket = rhs.m_socket;
		m_prev_size = rhs.m_prev_size;
		m_move_time = rhs.m_move_time;
		m_x = rhs.m_x;
		m_y = rhs.m_y;
	}
};