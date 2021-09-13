#pragma once
#include"Object.h"

class Player: public Object
{
private:
	EX_OVER m_recv_over;
	short m_level;
	int m_exp;
	SOCKET m_socket;

public:
	int m_prev_size;

	Player() {
		m_state = OBJECT_STATE::OBJST_FREE;
	}

	void Init(int level, int exp, float x, float y) {
		m_level = level;
		m_exp = exp;
		m_x = x;
		m_y = y;
	}

	void PlayerAccept(HANDLE h_iocp, int c_id) {
		m_recv_over.m_op = OP_RECV;
		m_prev_size = 0;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket), h_iocp, c_id, 0);
	}

	bool isState(OBJECT_STATE plst) {
		if (m_state == plst)
			return true;
		return false;
	}

	void Connected(SOCKET p_socket) {
		m_state = OBJECT_STATE::OBJST_CONNECTED;
		m_socket = p_socket;
	}

	void Recv()
	{
		m_recv_over.m_wsabuf[0].buf =
			reinterpret_cast<char*>(m_recv_over.m_packetbuf)
			+ m_prev_size;
		m_recv_over.m_wsabuf[0].len = MAX_BUFFER - m_prev_size;
		memset(&m_recv_over.m_over, 0, sizeof(m_recv_over.m_over));
		DWORD r_flag = 0;
		WSARecv(m_socket, m_recv_over.m_wsabuf, 1,
			NULL, &r_flag, &m_recv_over.m_over, NULL);
		/*int ret = WSARecv(m_socket,m_recv_over.m_wsabuf, 1,
			NULL, &r_flag, &m_recv_over.m_over, NULL);

		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				display_error("WSARecv : ", WSAGetLastError());
		}*/
	}

	void Send(void* p)
	{
		int p_size = reinterpret_cast<unsigned char*>(p)[0];
		int p_type = reinterpret_cast<unsigned char*>(p)[1];
		//cout << "To client [" << p_id << "] : ";
		//cout << "Packet [" << p_type << "]\n";
		EX_OVER* s_over = new EX_OVER;
		s_over->m_op = OP_SEND;
		memset(&s_over->m_over, 0, sizeof(s_over->m_over));
		memcpy(s_over->m_packetbuf, p, p_size);
		s_over->m_wsabuf[0].buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
		s_over->m_wsabuf[0].len = p_size;

		WSASend(m_socket, s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, 0);

		//int ret = WSASend(m_socket, s_over->m_wsabuf, 1,
		//	NULL, 0, &s_over->m_over, 0);
		//if (0 != ret) {
		//	int err_no = WSAGetLastError();
		//	if (WSA_IO_PENDING != err_no) {
		//		display_error("WSASend : ", WSAGetLastError());
		//		//disconnect(p_id);
		//	}
		//}
	}

	void CloseSocket() {
		closesocket(m_socket);
	}

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