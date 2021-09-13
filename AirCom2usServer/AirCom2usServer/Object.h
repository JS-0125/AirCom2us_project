#pragma once
#include "default.h"

constexpr int SERVER_ID = 0;
constexpr int MAX_BUFFER = 1024;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_ATTACK, OP_CHASE, OP_POINT_MOVE, OP_DEAD, OP_RESPAWN };
enum OBJECT_STATE { OBJST_FREE, OBJST_CONNECTED, OBJST_INGAME };

struct EX_OVER
{
	WSAOVERLAPPED	m_over;
	WSABUF			m_wsabuf[1];
	unsigned char	m_packetbuf[MAX_BUFFER];
	OP_TYPE			m_op;
	SOCKET			m_csocket;					// OP_ACCEPT에서만 사용
};


class Object
{
public:
	int		m_id;
	int m_move_time;
	float m_x, m_y;
	atomic<OBJECT_STATE> m_state;
};

