#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"

class ServerManager
{
	static HANDLE h_iocp;
	SOCKADDR_IN serverAddr;
	EX_OVER accept_over;
	SOCKET listenSocket;
public:
	ServerManager();
	~ServerManager();

	static void PostQueued(int eventType, WSAOVERLAPPED& over);
	static void Send(void* p, SOCKET& socket);
	static void Recv(SOCKET& socket, EX_OVER& ex_over, int prev_size);
	HANDLE GetIocpHandle();
	SOCKET GetListenSocket();
	static void display_error(const char* msg, int err_no);
};
