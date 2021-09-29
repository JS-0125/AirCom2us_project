#include "Player.h"

 Player::Player() {
	m_state = OBJECT_STATE::OBJST_FREE;
}

 void Player::Init(int level, int exp, float x, float y) {
	ResetInGameData();
	m_level = level;
	m_exp = exp;
	m_x = x;
	m_y = y;
}

 void Player::PlayerAccept(HANDLE h_iocp, int c_id) {
	m_recv_over.m_op = OP_RECV;
	m_prev_size = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket), h_iocp, c_id, 0);
}

 bool Player::IsState(OBJECT_STATE plst) {
	if (m_state == plst)
		return true;
	return false;
}

 void Player::Connected(SOCKET &p_socket) {
	m_state = OBJECT_STATE::OBJST_CONNECTED;
	m_socket = p_socket;
	ServerManager::TcpSetKeepAlive(m_socket, 1, 1, 10, 1);
	int opt_val = true;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
}

 void Player::ResetInGameData() {
	 m_sessionId = 0;
	 m_move_time = 0;
	 hp = 0;
	 m_x = 0;
	 m_y = 0;
 }

 void Player::CloseSocket() {
	closesocket(m_socket);
	//ResetInGameData();
	//m_state = OBJECT_STATE::OBJST_FREE;
}

 void Player::CloseSocket(string msg) {
	 closesocket(m_socket);
	 //ResetInGameData();
	 //m_state = OBJECT_STATE::OBJST_FREE;
	 cout << msg << endl;
 }

 int Player::GetPrevSize() {
	return m_prev_size;
}

 void Player::SetPrevSize(int prevSize) {
	m_prev_size = prevSize;
}

 EX_OVER* Player::GetOverlapped() {
	return &m_recv_over;
}

 SOCKET* Player::GetSocket() {
	return &m_socket;
}

 bool Player::CheckAbnormalAction(cs_packet_move* packet) {
	 if (m_state != OBJECT_STATE::OBJST_INGAME)
		 return false;
	 // move time check
	 if (m_move_time == 0) {
		 m_move_time = packet->move_time;
	 }
	 if (packet->move_time - m_move_time > 1.5f) {
		 cout << m_id << ": CheckAbnormalAction packet - " << m_move_time << ", " << packet->move_time << endl;
		 //CloseSocket("move time is abnormal" );
		 return true;
	 }
	 m_move_time = packet->move_time;
	 return false;
 }
