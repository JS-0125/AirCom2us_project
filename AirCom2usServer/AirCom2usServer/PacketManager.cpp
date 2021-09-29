#include "PacketManager.h"

bool PacketManager::SendLoginOk(int p_id, SOCKET& socket)
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

	cout << p_id<< " - SendLoginOk" << endl;
	if (ServerManager::Send(&p, socket)) {
		cout << "SendLoginOk true" << endl;
		return true;
	}
	cout << "SendLoginOk false" << endl;
	ServerManager::Disconnect(socket);
	return false;
}

bool PacketManager::SendSetSessionOk(string ip,int sessionId, SOCKET& socket)
{
	sc_packet_set_session_ok p;

	p.type = SC::SET_SESSION_OK;
	strcpy_s(p.ip, ip.c_str());
	p.sessionId = sessionId;
	p.size = sizeof(p);

	if (ServerManager::Send(&p, socket)) {
		return true;
		cout << "SendSetSessionOk" << endl;
	}
	ServerManager::Disconnect(socket);

	return false;
}

bool PacketManager::SendMove(int move_id, int x, int y, SOCKET& socket)
{
	sc_packet_position p;
	p.type = SC::POSITION;
	p.id = move_id;
	p.x = x;
	p.y = y;
	p.move_time = 0;
	p.size = sizeof(p);
	if (ServerManager::Send(&p, socket))
		return true;
	cout << "move_id - " << move_id << endl;
	ServerManager::Disconnect(socket);
	return false;
}

bool PacketManager::SendAddObj(int obj_id, int hp, SOCKET& socket)
{
	sc_packet_add_object p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.id = obj_id;
	p.hp = hp;
	p.type = SC::ADD_OBJECT;
	p.size = sizeof(p);

	if (ServerManager::Send(&p, socket))
		return true;
	ServerManager::Disconnect(socket);

	return false;
}

bool PacketManager::SendEndSession(int obj_id, SOCKET& socket) {
	sc_packe_end_session p;

	p.type = SC::END_SESSION;
	p.size = sizeof(p);

	if (ServerManager::Send(&p, socket))
		return true;
	ServerManager::Disconnect(socket);

	return false;
}

bool PacketManager::SendRemoveObj(int obj_id, int remove_obj_id, SOCKET& socket) {
	sc_packet_remove_object p;

	p.type = SC::REMOVE_OBJECT;
	p.size = sizeof(p);
	p.id = remove_obj_id;

	if (ServerManager::Send(&p, socket))
		return true;
	ServerManager::Disconnect(socket);

	return false;
}

bool PacketManager::SendReconnectOk(int obj_id, int sessionId, SOCKET& socket)
{
	cout << "SendReconnectOk" << endl;
	sc_packet_reconnect_ok p;

	p.type = SC::RECONNECT_OK;
	p.size = sizeof(p);
	p.sessionId = sessionId;

	if (ServerManager::Send(&p, socket)) {
		cout << "SendReconnectOk true" << endl;
		return true;
	}
	ServerManager::Disconnect(socket);
	return false;
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
