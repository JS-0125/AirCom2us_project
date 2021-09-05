#include <WS2tcpip.h>
#include <MSWSock.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <array>
#include"AirCom2us_protocol.h"


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;


constexpr int SERVER_ID = 0;
constexpr int MAX_BUFFER = 1024;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT, OP_RANDOM_MOVE, OP_ATTACK, OP_CHASE, OP_POINT_MOVE, OP_DEAD, OP_RESPAWN };
enum PLAYER_STATE { PLST_FREE, PLST_CONNECTED, PLST_INGAME };

struct EX_OVER
{
	WSAOVERLAPPED	m_over;
	WSABUF			m_wsabuf[1];
	unsigned char	m_packetbuf[MAX_BUFFER];
	OP_TYPE			m_op;
	SOCKET			m_csocket;					// OP_ACCEPT에서만 사용
};

struct PLAYER
{
	atomic<PLAYER_STATE> m_state;
	int		id;
	EX_OVER m_recv_over;
	short level;
	int exp;
	SOCKET m_socket;
	int m_prev_size;
	int move_time;
	float x, y;
};

HANDLE h_iocp;

array <PLAYER*, MAX_USER + 1> players;

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

int get_new_player_id(SOCKET p_socket)
{
	for (int i = SERVER_ID + 1; i < MAX_USER; ++i) {
		if (PLST_FREE == players[i]->m_state) {
			players[i]->m_state = PLST_CONNECTED;
			players[i]->m_socket = p_socket;

			return i;
		}
	}
	return -1;
}

void send_packet(int p_id, void* p)
{
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
	int ret = WSASend((*static_cast<PLAYER*>(players[p_id])).m_socket, s_over->m_wsabuf, 1,
		NULL, 0, &s_over->m_over, 0);
	//cout << ret << reinterpret_cast<sc_packet_position*>(p)->id << " ";
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			display_error("WSASend : ", WSAGetLastError());
			//disconnect(p_id);
		}
	}
}

void send_login_ok_packet(int p_id, sc_packet_login_ok p)
{
	players[p_id]->level = p.LEVEL;
	players[p_id]->exp = p.EXP;
	players[p_id]->x = p.x;
	players[p_id]->y = p.y;
	p.type = SC_LOGIN_OK;
	p.id = p_id;
	p.size = sizeof(p);
	send_packet(p_id, &p);
}

void send_move_packet(int c_id, int p_id)
{
	sc_packet_position p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = SC_POSITION;
	p.x = players[p_id]->x;
	p.y = players[p_id]->y;
	p.move_time = (*static_cast<PLAYER*>(players[p_id])).move_time;
	send_packet(c_id, &p);
}


void do_move(int p_id, float x, float y)
{
	//cout << players[p_id]->x << " - " << x << ", " << players[p_id]->y << "- " << y << endl;
	if (abs(x - players[p_id]->x) < 0.001 && abs(y - players[p_id]->y) < 0.001)
		return;

	float vecX = x - players[p_id]->x;
	float vecY = y - players[p_id]->y;

	players[p_id]->x += vecX * 0.2;
	players[p_id]->y += vecY * 0.2;
	send_move_packet(p_id, p_id);
}


void process_packet(int p_id, unsigned char* p_buf)
{
	switch (p_buf[1]) {
	case CS_LOGIN: {
		cout << "CS Login" << endl;
		//cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p_buf);
		sc_packet_login_ok p;

		// player 정보
		// 추후 데이터베이스 생성 후 수정 필요
		p.EXP = 100;
		p.id = 0;
		p.LEVEL = 1;
		p.type = SC_POSITION;
		p.x = 0;
		p.y = 0;
		p.size = sizeof(p);

		send_login_ok_packet(p_id, p);
		break;
	}
	case CS_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p_buf);
		players[p_id]->move_time = packet->move_time;
		do_move(p_id, packet->x, packet->y);
		break;
	}
	default:
		cout << "Unknown Packet Type from Client[" << p_id;
		cout << "] Packet Type [" << (int)p_buf[1] << "]";
		while (true);
	}
}

void do_recv(int key)
{
	players[key]->m_recv_over.m_wsabuf[0].buf =
		reinterpret_cast<char*>(players[key]->m_recv_over.m_packetbuf)
		+ (*static_cast<PLAYER*>(players[key])).m_prev_size;
	players[key]->m_recv_over.m_wsabuf[0].len = MAX_BUFFER - (*static_cast<PLAYER*>(players[key])).m_prev_size;
	memset(&players[key]->m_recv_over.m_over, 0, sizeof(players[key]->m_recv_over.m_over));
	DWORD r_flag = 0;
	int ret = WSARecv((*static_cast<PLAYER*>(players[key])).m_socket, players[key]->m_recv_over.m_wsabuf, 1,
		NULL, &r_flag, &players[key]->m_recv_over.m_over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			display_error("WSARecv : ", WSAGetLastError());
	}
}


void worker(HANDLE h_iocp, SOCKET l_socket)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR ikey;
		WSAOVERLAPPED* over;

		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes,
			&ikey, &over, INFINITE);

		int key = static_cast<int>(ikey);
		if (FALSE == ret) {
			if (SERVER_ID == key) {
				display_error("GQCS : ", WSAGetLastError());
				exit(-1);
			}
			else {
				display_error("GQCS : ", WSAGetLastError());
				//disconnect(key);
			}
		}
		if ((key != SERVER_ID) && (0 == num_bytes)) {
			//disconnect(key);
			continue;
		}
		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

		switch (ex_over->m_op) {
		case OP_RECV: {
			PLAYER* player = static_cast<PLAYER*>(players[key]);

			unsigned char* packet_ptr = ex_over->m_packetbuf;
			int num_data = num_bytes + player->m_prev_size;

			while (num_data >= packet_ptr[0]) {
				process_packet(key, packet_ptr);
				num_data -= packet_ptr[0];
				packet_ptr += packet_ptr[0];
				if (0 >= num_data) break;
			}
			player->m_prev_size = num_data;
			if (0 != num_data)
				memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
			do_recv(key);
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_ACCEPT: {
			cout << "accept" << endl;
			int c_id = get_new_player_id(ex_over->m_csocket);
			PLAYER* player = static_cast<PLAYER*>(players[c_id]);

			if (-1 != c_id) {
				player->m_recv_over.m_op = OP_RECV;
				player->m_prev_size = 0;
				CreateIoCompletionPort(
					reinterpret_cast<HANDLE>(player->m_socket), h_iocp, c_id, 0);
				do_recv(c_id);
			}
			else {
				closesocket(player->m_socket);
			}

			memset(&ex_over->m_over, 0, sizeof(ex_over->m_over));
			SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ex_over->m_csocket = c_socket;
			AcceptEx(l_socket, c_socket,
				ex_over->m_packetbuf, 0, 32, 32, NULL, &ex_over->m_over);

			break;
		}
		}
	}
}


int main()
{

	for (int i = 0; i < MAX_USER + 1; ++i) {
		auto& pl = players[i] = new PLAYER;
		pl->id = i;
		pl->m_state = PLST_FREE;
	}

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	wcout.imbue(locale("korean"));
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, SERVER_ID, 0);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	listen(listenSocket, SOMAXCONN);

	EX_OVER accept_over;
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

	vector <thread> worker_threads;
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(worker, h_iocp, listenSocket);

	for (auto& th : worker_threads)
		th.join();
	closesocket(listenSocket);
	WSACleanup();
}
