#include "Player.h"

 Player::Player() {
	m_state = OBJECT_STATE::OBJST_FREE;
}

 void Player::Init(int level, int exp, float x, float y) {
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

 void Player::Connected(SOCKET p_socket) {
	m_state = OBJECT_STATE::OBJST_CONNECTED;
	m_socket = p_socket;
}

 void Player::CloseSocket() {
	closesocket(m_socket);
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
