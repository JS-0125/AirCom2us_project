#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"ServerManager.h"

class PacketManager
{
public:
	static bool SendLoginOk(int p_id, SOCKET& socket);
	static bool SendSetSessionOk(string ip,int sessionId, SOCKET& socket);
	static bool SendMove(int move_id, int x, int y, SOCKET& socket);
	static bool SendAddObj(int obj_id, int hp, SOCKET& socket);
	static bool SendEndSession(int obj_id, SOCKET& socket);
	static bool SendRemoveObj(int obj_id, int remove_obj_id, SOCKET& socket);
	static bool SendReconnectOk(int obj_id, int sessionId, SOCKET& socket);
	static bool SendHeartBeat(SOCKET& socket);
	static bool CheckPakcet(OBJECT_STATE objState, CS packetType);
};

