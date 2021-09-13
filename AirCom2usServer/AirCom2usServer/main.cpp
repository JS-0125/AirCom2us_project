#include"default.h"
#include"AirCom2us_protocol.h"
#include"Player.h"
#include"Session.h"

HANDLE h_iocp;

array <Object*, MAX_USER + 1> objects;
array <Session*, MAX_SESSION + 1> sessions;

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
	for (int i = SERVER_ID + 1; i < MAX_PLAYER_IDX + 1; ++i) {
		if(reinterpret_cast<Player*>(objects[i])->isState(OBJST_FREE)){
			reinterpret_cast<Player*>(objects[i])->Connected(p_socket);
			return i;
		}
	}
	return -1;
}

int get_session_id(SESSION_STATE sessionState)
{
	switch (sessionState)
	{
	case SESSION_OPEN: 
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (sessions[i]->sessionState == SESSION_STATE::SESSION_OPEN)
				return i;
		break;
	}
	case SESSION_CLOSE: 
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (sessions[i]->sessionState == SESSION_STATE::SESSION_CLOSE)
				return i;
		break;
	}
	case SESSION_INGAME: 
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (sessions[i]->sessionState == SESSION_STATE::SESSION_INGAME)
				return i;
		break;
	}
	default:
		break;
	}
}

int get_enemy_id(ENEMY_TYPE enemyType) {
	switch (enemyType) {
	case ENEMY_TYPE::ENEMY_Plane1:
		for (int i = MAX_PLAYER_IDX + 1; i < MAX_PLANE1_IDX + 1; ++i)
			if (objects[i]->m_state == OBJECT_STATE::OBJST_FREE)
				return i;
		break;
	case ENEMY_TYPE::ENEMY_Plane2:
		for (int i = MAX_PLANE1_IDX + 1; i < MAX_PLANE2_IDX + 1; ++i)
			if (objects[i]->m_state == OBJECT_STATE::OBJST_FREE)
				return i;
		break;
	case ENEMY_TYPE::ENEMY_Plane3:
		for (int i = MAX_PLANE2_IDX + 1; i < MAX_PLANE3_IDX + 1; ++i)
			if (objects[i]->m_state == OBJECT_STATE::OBJST_FREE)
				return i;
		break;
	case ENEMY_TYPE::ENEMY_Boss1:
		for (int i = MAX_PLANE3_IDX + 1; i < MAX_BOSS1_IDX + 1; ++i)
			if (objects[i]->m_state == OBJECT_STATE::OBJST_FREE)
				return i;
		break;
	case ENEMY_TYPE::ENEMY_Boss2:
		for (int i = MAX_BOSS1_IDX+1; i < MAX_BOSS2_IDX+1; ++i)
			if (objects[i]->m_state == OBJECT_STATE::OBJST_FREE)
				return i;
		break;
	}
}

void send_login_ok_packet(int p_id)
{
	sc_packet_login_ok p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.EXP = 100;
	p.LEVEL = 1;
	p.x = 0;
	p.y = 0;
	p.id = p_id;
	p.type = SC_LOGIN_OK;
	p.size = sizeof(p);

	reinterpret_cast<Player*>(objects[p_id])->Init(p.LEVEL, p.EXP, p.x, p.y);
	reinterpret_cast<Player*>(objects[p_id])->Send(&p);
}

void send_set_session_ok(int p_id)
{
	sc_packet_set_session_ok p;

	p.type = SC_SET_SESSION_OK;
	p.size = sizeof(p);

	reinterpret_cast<Player*>(objects[p_id])->Send(&p);
}


void send_move_packet(int c_id, int p_id)
{
	sc_packet_position p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = SC_POSITION;
	p.x = objects[p_id]->m_x;
	p.y = objects[p_id]->m_y;
	p.move_time = (*static_cast<Player*>(objects[p_id])).m_move_time;

	reinterpret_cast<Player*>(objects[c_id])->Send(&p);
}

void send_add_obj_packet(int p_id, int obj_id)
{
	sc_packet_add_object p;

	// player 정보
	// 추후 데이터베이스 생성 후 수정 필요
	p.id = obj_id;
	p.type = SC_ADD_OBJECT;
	p.size = sizeof(p);

	reinterpret_cast<Player*>(objects[p_id])->Send(&p);
}

void do_move(int p_id, float x, float y)
{
	//cout << objects[p_id]->x << " - " << x << ", " << objects[p_id]->y << "- " << y << endl;
	if (abs(x - objects[p_id]->m_x) < 0.001 && abs(y - objects[p_id]->m_y) < 0.001)
		return;

	float vecX = x - objects[p_id]->m_x;
	float vecY = y - objects[p_id]->m_y;

	objects[p_id]->m_x += vecX * 0.2;
	objects[p_id]->m_y += vecY * 0.2;
	send_move_packet(p_id, p_id);
}


void process_packet(int p_id, unsigned char* p_buf)
{
	switch (p_buf[1]) {
	case CS_LOGIN: {
		cout << "CS Login" << endl;
		//cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p_buf);

		send_login_ok_packet(p_id);
		break;
	}
	case CS_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p_buf);
		objects[p_id]->m_move_time = packet->move_time;
		do_move(p_id, packet->x, packet->y);
		break;
	}
	case CS_CREATE_SESSION: {
		//session test
		// session open
		int closeSessionId = get_session_id(SESSION_STATE::SESSION_CLOSE);
		sessions[closeSessionId]->OpenSession(1);

		// open된 session에 setPlayer
		int openSessionId = get_session_id(SESSION_STATE::SESSION_OPEN);
		sessions[openSessionId]->SetPlayer(*static_cast<Player*>(objects[p_id]));

		vector<ENEMY_TYPE> tmp;
		tmp.push_back(ENEMY_TYPE::ENEMY_Plane1);
		tmp.push_back(ENEMY_TYPE::ENEMY_Plane2);
		tmp.push_back(ENEMY_TYPE::ENEMY_Plane3);

		vector<int> enemyIds;

		for (const auto& enemy : tmp) {
			enemyIds.push_back(get_enemy_id(enemy));
		}
		
		for (const auto& enemyId : enemyIds) {
			cout << enemyId << endl;
			send_add_obj_packet(p_id, enemyId);
		}
		
		sessions[openSessionId]->SetSession(1, enemyIds);

		send_set_session_ok(p_id);
		break;
	}
	default:
		cout << "Unknown Packet Type from Client[" << p_id;
		cout << "] Packet Type [" << (int)p_buf[1] << "]";
		while (true);
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
			Player* player = static_cast<Player*>(objects[key]);

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
			player->Recv();
			//do_recv(key);
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_ACCEPT: {
			cout << "accept" << endl;
			int c_id = get_new_player_id(ex_over->m_csocket);
			Player* player = static_cast<Player*>(objects[c_id]);

			if (-1 != c_id) {
				player->PlayerAccept(h_iocp, c_id);
				player->Recv();
			}
			else {
				player->CloseSocket();
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
		if (i < MAX_PLAYER_IDX + 1) {
			auto& obj = objects[i] = new Player;
			obj->m_id = i;
		}
		else if (i < MAX_PLANE1_IDX + 1) {
			auto& obj = objects[i] = new Plane1;
			obj->m_id = i;
		}
		else if (i < MAX_PLANE2_IDX + 1) {
			auto& obj = objects[i] = new Plane2;
			obj->m_id = i;
		}
		else if (i < MAX_PLANE3_IDX + 1) {
			auto& obj = objects[i] = new Plane3;
			obj->m_id = i;
		}
		else if (i < MAX_BOSS1_IDX + 1) {
			auto& obj = objects[i] = new Boss1;
			obj->m_id = i;
		}
		else if (i < MAX_BOSS2_IDX + 1) {
			auto& obj = objects[i] = new Boss2;
			obj->m_id = i;
		}
	}

	for (int i = 0; i < MAX_SESSION + 1; ++i) {
		auto& session = sessions[i] = new Session;
		session->CreateSession(i);
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
