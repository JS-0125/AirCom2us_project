#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"ServerManager.h"

class PacketManager
{
public:
	static void SendLoginOk(int p_id, SOCKET& socket);
	static void SendSetSessionOk(int sessionId, SOCKET& socket);
	static void SendMove(int move_id, int x, int y, SOCKET& socket);
	static void SendAddObj(int obj_id, SOCKET& socket);
};

