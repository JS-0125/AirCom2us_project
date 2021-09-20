#include"default.h"
#include"AirCom2us_protocol.h"
#include"Player.h"
#include"Session.h"

HANDLE h_iocp;


struct TIMER_EVENT {
	int object;
	OP_TYPE e_type;
	chrono::system_clock::time_point start_time;
	int target_id;

	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (start_time > L.start_time);
	}
};

array <Object*, MAX_USER + 1> objects;
array <Session*, MAX_SESSION + 1> sessions;
priority_queue <TIMER_EVENT> timer_queue;
mutex timer_l;

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

void add_event(int obj, int target_id, OP_TYPE ev_t, int delay_ms)
{
	TIMER_EVENT ev;
	ev.e_type = ev_t;
	ev.object = obj;
	ev.start_time = system_clock::now() + milliseconds(delay_ms);
	ev.target_id = target_id;
	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
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

int get_session_id(SESSION_STATE sessionState, int sessionType)
{
	switch (sessionState)
	{
	case SESSION_OPEN: 
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (sessions[i]->sessionState == SESSION_STATE::SESSION_OPEN && sessions[i]->m_sessionType == sessionType)
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
		return -1;
		break;
	}
	return -1;
}

int get_enemy_id(ENEMY_TYPE enemyType) {
	switch (enemyType) {
	case ENEMY_TYPE::ENEMY_Plane1:
		return MAX_PLAYER_IDX + 1;
		break;
	case ENEMY_TYPE::ENEMY_Plane2:
		return MAX_PLANE1_IDX + 1;
		break;
	case ENEMY_TYPE::ENEMY_Plane3:
		return  MAX_PLANE2_IDX + 1;
		break;
	case ENEMY_TYPE::ENEMY_Boss1:
		return MAX_PLANE3_IDX + 1;
		break;
	case ENEMY_TYPE::ENEMY_Boss2:
		return MAX_BOSS1_IDX + 1;
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

void send_set_session_ok(int p_id, int sessionId)
{
	sc_packet_set_session_ok p;

	p.type = SC_SET_SESSION_OK;
	p.size = sizeof(p);
	p.sessionId = sessionId;

	reinterpret_cast<Player*>(objects[p_id])->Send(&p);
}

void send_move_packet(int c_id, int move_id, int x, int y)
{
	sc_packet_position p;
	p.id = move_id;
	p.size = sizeof(p);
	p.type = SC_POSITION;

	if (x == -1000) {
		p.x = objects[move_id]->m_x;
		p.y = objects[move_id]->m_y;
		p.move_time = (*static_cast<Player*>(objects[move_id])).m_move_time;
	}
	else {
		cout << "enemy" << endl;
		p.x = x;
		p.y = y;
		p.move_time = 0;
	}
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
	if (sessions[static_cast<Player*>(objects[p_id])->m_sessionId]->sessionState == SESSION_STATE::SESSION_INGAME) {
		auto sessionPlayers =  sessions[static_cast<Player*>(objects[p_id])->m_sessionId]->getPlayerId();
		for (const auto& player : sessionPlayers)
			send_move_packet(player, p_id, -1000, -1000);
	}
}

void checkSession(int sessionId) {
	if (!sessions[sessionId]->CheckSession())
		return;
	auto sessionPlayers = sessions[sessionId]->getPlayerId();

	for (int i = 0; i < sessions[sessionId]->m_enemyDatas.size(); ++i) {
		auto enemyData = sessions[sessionId]->m_enemyDatas[i];

		int typeId = get_enemy_id(enemyData.type);
		int objId = typeId + i;
		//sessions[openSessionId]->StartSession(objects[objId]);

		/*objects[objId]->m_state = OBJECT_STATE::OBJST_INGAME;
		objects[objId]->m_move_time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() + enemyData.time);
		objects[objId]->m_x = enemyData.x;
		objects[objId]->m_y = enemyData.y;*/

		for (int i = 0; i < sessionPlayers.size(); ++i) {
			send_add_obj_packet(sessionPlayers[i], objId);
			//add_event(objId, sessionPlayers[i], OP_ENEMY_SEND, enemyData.time + 100);
		}

		add_event(objId - typeId, sessionId, OP_POINT_MOVE, enemyData.time);
	}

	// 서로에게 플레이어 추가
	for (int i = 0; i < sessionPlayers.size(); ++i) {
		for (int j = 0; j < sessionPlayers.size(); ++j) 
			if(sessionPlayers[i] != sessionPlayers[j])
				send_add_obj_packet(sessionPlayers[i], sessionPlayers[j]);
	}

	// 세션 세팅 ok
	for (int i = 0; i < sessionPlayers.size(); ++i) 
		send_set_session_ok(sessionPlayers[i], sessionId);
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
		cs_packet_create_session* packet = reinterpret_cast<cs_packet_create_session*>(p_buf);

		// open된 session 없음, session 새로 open
		cout << "session type - " << packet->sessionType << endl;
		int openSessionId = get_session_id(SESSION_STATE::SESSION_OPEN, packet->sessionType);
		if (openSessionId < 0) {
			// session open
			cout << "new session open" << endl;
			int closeSessionId = get_session_id(SESSION_STATE::SESSION_CLOSE, packet->sessionType);
			sessions[closeSessionId]->OpenSession(packet->sessionType);
			sessions[closeSessionId]->SetPlayer(*static_cast<Player*>(objects[p_id]));

			// session id 플레이어에 할당
			static_cast<Player*>(objects[p_id])->m_sessionId = closeSessionId;

			checkSession(closeSessionId);
		}
		else {
			// open된 session에 setPlayer
			cout << "join session" << endl;
			sessions[openSessionId]->SetPlayer(*static_cast<Player*>(objects[p_id]));

			// session id 플레이어에 할당
			static_cast<Player*>(objects[p_id])->m_sessionId = openSessionId;

			checkSession(openSessionId);
		}

		break;
	}
	default:
		cout << "Unknown Packet Type from Client[" << p_id;
		cout << "] Packet Type [" << (int)p_buf[1] << "]";
		while (true);
	}
}


void do_timer()
{
	using namespace chrono;

	for (;;) {
		timer_l.lock();
		if ((false == timer_queue.empty())
			&& (timer_queue.top().start_time <= system_clock::now())) {
			TIMER_EVENT ev = timer_queue.top();
			timer_queue.pop();
			timer_l.unlock();

			if (ev.e_type == OP_POINT_MOVE) {
				EX_OVER* ex_over = new EX_OVER;
				ex_over->m_op = OP_POINT_MOVE;
				*reinterpret_cast<int*>(ex_over->m_packetbuf) = ev.target_id;
				PostQueuedCompletionStatus(h_iocp, 1, ev.object, &ex_over->m_over);
			}
		}
		else {
			timer_l.unlock();
			this_thread::sleep_for(10ms);
		}
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
		case OP_POINT_MOVE: {
			//auto& enemy = *static_cast<Enemy*>(objects[key]);
			//int time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());
			int sessionId = *reinterpret_cast<int*>(ex_over->m_packetbuf);
			auto enemy = &sessions[sessionId]->m_enemyDatas[key];

			if (enemy->x.empty())
				return;
			cout << "before pop - "<< enemy->x.size() << endl;
			int x = enemy->x.front();
			enemy->x.pop();
			cout << "after pop - " << enemy->x.size() << endl;

			int y = enemy->y.front();
			enemy->y.pop();

			cout << "x,y - " << x << ", " << y << endl;
			//enemy.Move(time);
			auto players = sessions[sessionId]->getPlayerId();
			for (int i = 0; i < players.size(); ++i) 
				send_move_packet(players[i], key + get_enemy_id(enemy->type), x, y);

			if (enemy->x.empty() == false)
				add_event(key, sessionId, OP_POINT_MOVE, 1000);
			
			break;
		}
		}
	}
}


//void udpWorker(SOCKET l_socket) {
//	SOCKET c_socket;
//	int nClient_Size = sizeof(SOCKADDR_IN);
//	SOCKADDR_IN m_ClientInfo;
//
//	int retVal = 0;
//	int nRecvLen = 0;
//	char Buffer[1024] = { 0, };
//
//	while (1)
//	{ // Recv 대기. Send한 Client 정보 저장 
//		cout << "while udp"<< endl;
//
//		nRecvLen = recvfrom(l_socket, reinterpret_cast<char*>(Buffer),
//			1024, 0, (SOCKADDR*)&m_ClientInfo, &nClient_Size);
//
//		if (nRecvLen > 0)
//		{
//			// RECV 성공 시 MSG 처리.. 
//			cout << "udp recv completed " + (int)Buffer[1] << endl;
//		}
//	}
//}

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


	// UDP

	//SOCKET m_ServerSocket;
	//SOCKADDR_IN m_ServerInfo;

	//int retVal = 0;

	//// WSADATA wsa; // Winsock 데이터 구조체

	////WS2_32.DLL 초기화 
	///*if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	//{
	//	WSACleanup(); return -1;
	//}*/

	//// UDP Socket 생성 (SOCK_DGRAM : UDP) 
	//m_ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);

	//m_ServerInfo.sin_family = AF_INET; // IPv4 
	//m_ServerInfo.sin_addr.s_addr = htonl(INADDR_ANY); //Local 
	//m_ServerInfo.sin_port = htons(49151);

	//retVal = bind(m_ServerSocket, (struct sockaddr*)&m_ServerInfo, sizeof(SOCKADDR_IN));

	//if (retVal == SOCKET_ERROR)
	//	return -1;

	vector <thread> worker_threads;
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(worker, h_iocp, listenSocket);

	thread timer_thread{ do_timer };
	//thread udpWorker_thread{ udpWorker,m_ServerSocket };

	//udpWorker_thread.join();
	timer_thread.join();

	for (auto& th : worker_threads)
		th.join();

	
	//closesocket(m_ServerSocket);
	//WSACleanup();

	closesocket(listenSocket);
	WSACleanup();
}
