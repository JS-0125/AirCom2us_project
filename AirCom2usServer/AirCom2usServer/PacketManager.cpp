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
	p.type = SC::LOGIN_OK;
	p.size = sizeof(p);

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendSetSessionOk(int sessionId, SOCKET& socket)
{
	sc_packet_set_session_ok p;

	p.type = SC::SET_SESSION_OK;
	p.size = sizeof(p);
	p.sessionId = sessionId;

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendMove(int move_id, int x, int y, SOCKET& socket)
{
	sc_packet_position p;
	p.id = move_id;
	p.size = sizeof(p);
	p.type = SC::POSITION;
	p.x = x;
	p.y = y;
	p.move_time = 0;

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendAddObj(int obj_id, int hp, SOCKET& socket)
{
	sc_packet_add_object p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.id = obj_id;
	p.hp = hp;
	p.type = SC::ADD_OBJECT;
	p.size = sizeof(p);

	ServerManager::Send(&p, socket);
}

 void PacketManager::SendEndSession(int obj_id, SOCKET& socket) {
	 sc_packe_end_session p;

	 p.type = SC::END_SESSION;
	 p.size = sizeof(p);

	 ServerManager::Send(&p, socket);
 }

 bool PacketManager::CheckPakcet(OBJECT_STATE objState, CS packetType) {
	 switch (objState)
	 {
	 case OBJST_FREE:
		 break;
	 case OBJST_CONNECTED:
		 if (packetType == CS::CREATE_SESSION)
			 return true;
		 break;
	 case OBJST_INGAME:
		 if (packetType == CS::MOVE)
			 return true;
		 break;
	 default:
		 break;
	 }
	 return true;
 }
