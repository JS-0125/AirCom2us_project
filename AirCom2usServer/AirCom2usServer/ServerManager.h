#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"

class ServerManager
{
	static HANDLE h_iocp;
	SOCKADDR_IN m_serverAddr;
	EX_OVER m_acceptOver;
	SOCKET m_listenSocket;
public:
	ServerManager();
	~ServerManager();

	static void PostQueued(int eventType, WSAOVERLAPPED& over);
	static bool Send(void* p, SOCKET& socket);
	static void Recv(SOCKET& socket, EX_OVER& ex_over, int prev_size);
	static void Disconnect(SOCKET& socket);

	HANDLE GetIocpHandle();
	SOCKET GetListenSocket();
	static void DisplayError(const char* msg, int err_no);
};
