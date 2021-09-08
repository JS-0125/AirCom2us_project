#pragma once
#include "default.h"

constexpr int SERVER_ID = 0;
constexpr int MAX_BUFFER = 1024;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_ATTACK, OP_CHASE, OP_POINT_MOVE, OP_DEAD, OP_RESPAWN };
enum PLAYER_STATE { PLST_FREE, PLST_CONNECTED, PLST_INGAME };

struct EX_OVER
{
	WSAOVERLAPPED	m_over;
	WSABUF			m_wsabuf[1];
	unsigned char	m_packetbuf[MAX_BUFFER];
	OP_TYPE			m_op;
	SOCKET			m_csocket;					// OP_ACCEPT에서만 사용
};

class Player
{
public:
	atomic<PLAYER_STATE> m_state;
	int		id;
	EX_OVER m_recv_over;
	short level;
	int exp;
	SOCKET m_socket;
	int m_prev_size;
	int move_time;
	float x, y;

	void operator=(const Player& rhs) {
		m_state.store(rhs.m_state);
		id = rhs.id;
		m_recv_over = rhs.m_recv_over;
		level = rhs.level;
		exp = rhs.exp;
		m_socket = rhs.m_socket;
		m_prev_size = rhs.m_prev_size;
		move_time = rhs.move_time;
		x = rhs.x;
		y = rhs.y;
	}
};