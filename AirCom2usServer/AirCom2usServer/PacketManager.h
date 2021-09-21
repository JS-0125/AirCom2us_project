#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"ServerManager.h"

class PacketManager
{
public:
	static void send_login_ok_packet(int p_id, SOCKET& socket);
	static void send_set_session_ok(int sessionId, SOCKET& socket);
	static void send_move_packet(int move_id, int x, int y, SOCKET& socket);
	static void send_add_obj_packet(int obj_id, SOCKET& socket);
};

