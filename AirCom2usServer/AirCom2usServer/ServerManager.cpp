#include "ServerManager.h"
HANDLE ServerManager::h_iocp;

 ServerManager::ServerManager() {
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	wcout.imbue(locale("korean"));
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), h_iocp, SERVER_ID, 0);

	memset(&m_serverAddr, 0, sizeof(SOCKADDR_IN));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(SERVER_PORT);
	m_serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(m_listenSocket, (struct sockaddr*)&m_serverAddr, sizeof(SOCKADDR_IN));
	listen(m_listenSocket, SOMAXCONN);

	m_acceptOver.m_op = OP_ACCEPT;
	memset(&m_acceptOver.m_over, 0, sizeof(m_acceptOver.m_over));
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	m_acceptOver.m_csocket = c_socket;
	BOOL ret = AcceptEx(m_listenSocket, c_socket,
		m_acceptOver.m_packetbuf, 0, 32, 32, NULL, &m_acceptOver.m_over);
	if (FALSE == ret) {
		int err_num = WSAGetLastError();
		if (err_num != WSA_IO_PENDING)
			DisplayError("AcceptEx Error", err_num);
	}
}

 ServerManager::~ServerManager() {
	closesocket(m_listenSocket);
	WSACleanup();
}

 void ServerManager::PostQueued(int eventType, WSAOVERLAPPED& over) {
	PostQueuedCompletionStatus(h_iocp, 1, eventType, &over);
}

 void ServerManager::Send(void* p, SOCKET& socket) {
	int p_size = reinterpret_cast<unsigned char*>(p)[0];
	int p_type = reinterpret_cast<unsigned char*>(p)[1];
	//cout << "To client [" << p_id << "] : ";
	//cout << "Packet [" << p_type << "]\n";
	EX_OVER* s_over = new EX_OVER;
	s_over->m_op = OP_SEND;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf[0].buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf[0].len = p_size;

	WSASend(socket, s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, 0);
}

 void ServerManager::Recv(SOCKET& socket, EX_OVER& ex_over, int prev_size) {
	ex_over.m_wsabuf[0].buf =
		reinterpret_cast<char*>(ex_over.m_packetbuf)
		+ prev_size;
	ex_over.m_wsabuf[0].len = MAX_BUFFER - prev_size;
	memset(&ex_over.m_over, 0, sizeof(ex_over.m_over));
	DWORD r_flag = 0;
	//WSARecv(m_socket, m_recv_over.m_wsabuf, 1,
	//	NULL, &r_flag, &m_recv_over.m_over, NULL);

	int ret = WSARecv(socket, ex_over.m_wsabuf, 1,
		NULL, &r_flag, &ex_over.m_over, NULL);

	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			DisplayError("WSARecv : ", WSAGetLastError());
	}
}

 void ServerManager::Disconnect(SOCKET& socket)
 {
	 closesocket(socket);
 }

 HANDLE ServerManager::GetIocpHandle() {
	return h_iocp;
}

 SOCKET ServerManager::GetListenSocket() {
	return m_listenSocket;
}

 void ServerManager::DisplayError(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}
