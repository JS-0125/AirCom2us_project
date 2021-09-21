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
	ServerManager() {
		WSADATA WSAData;
		WSAStartup(MAKEWORD(2, 2), &WSAData);

		wcout.imbue(locale("korean"));
		h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, SERVER_ID, 0);

		memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
		listen(listenSocket, SOMAXCONN);

		accept_over.m_op = OP_ACCEPT;
		memset(&accept_over.m_over, 0, sizeof(accept_over.m_over));
		SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		accept_over.m_csocket = c_socket;
		BOOL ret = AcceptEx(listenSocket, c_socket,
			accept_over.m_packetbuf, 0, 32, 32, NULL, &accept_over.m_over);
		if (FALSE == ret) {
			int err_num = WSAGetLastError();
			if (err_num != WSA_IO_PENDING)
				display_error("AcceptEx Error", err_num);
		}
	}


	~ServerManager() {
		closesocket(listenSocket);
		WSACleanup();
	}

	static void PostQueued(int eventType, WSAOVERLAPPED &over) {
		PostQueuedCompletionStatus(h_iocp, 1, eventType, &over);
	}

	HANDLE GetIocpHandle() {
		return h_iocp;
	}

	SOCKET GetListenSocket() {
		return listenSocket;
	}

	void display_error(const char* msg, int err_no)
	{
		WCHAR* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		cout << msg;
		wcout << lpMsgBuf << endl;
		LocalFree(lpMsgBuf);
	}
};
