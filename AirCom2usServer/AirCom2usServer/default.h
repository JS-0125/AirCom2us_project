#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <array>
#include <mutex>
#include<queue>
#include <chrono>
#include<string>

extern "C" {
#include"lua.h"
#include "lauxlib.h"
#include"lualib.h"
}

#pragma comment( lib, "lua54.lib" )
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;


constexpr int SERVER_ID = 0;
constexpr int MAX_BUFFER = 1024;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT, OP_ATTACK, OP_CHASE, OP_POINT_MOVE, OP_ENEMY_SEND, OP_DEAD, OP_RESPAWN };
enum OBJECT_STATE { OBJST_FREE, OBJST_CONNECTED, OBJST_INGAME };

struct EX_OVER
{
	WSAOVERLAPPED	m_over;
	WSABUF			m_wsabuf[1];
	unsigned char	m_packetbuf[MAX_BUFFER];
	OP_TYPE			m_op;
	SOCKET			m_csocket;					// OP_ACCEPT에서만 사용
};