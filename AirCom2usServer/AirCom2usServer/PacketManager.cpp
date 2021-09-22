#include "PacketManager.h"

 void PacketManager::SendLoginOk(int p_id, SOCKET& socket)
{
	sc_packet_login_ok p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.EXP = 100;
	p.LEVEL = 1;
	p.x = 0;
	p.y = 0;
	p.id = p_id;
	p.type = SC_LOGIN_OK;
	p.size = sizeof(p);

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendSetSessionOk(int sessionId, SOCKET& socket)
{
	sc_packet_set_session_ok p;

	p.type = SC_SET_SESSION_OK;
	p.size = sizeof(p);
	p.sessionId = sessionId;

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendMove(int move_id, int x, int y, SOCKET& socket)
{
	sc_packet_position p;
	p.id = move_id;
	p.size = sizeof(p);
	p.type = SC_POSITION;
	p.x = x;
	p.y = y;
	p.move_time = 0;

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendAddObj(int obj_id, SOCKET& socket)
{
	sc_packet_add_object p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.id = obj_id;
	p.type = SC_ADD_OBJECT;
	p.size = sizeof(p);

	ServerManager::Send(&p, socket);
}
