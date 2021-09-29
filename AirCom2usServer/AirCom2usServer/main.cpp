#include"default.h"
#include"AirCom2us_protocol.h"
#include"Timer.h"
#include"ServerManager.h"
#include"SessionManager.h"
#include"ObjectManager.h"

ServerManager server;
Timer timer;
SessionManager sessionManager;
ObjectManager objManager;

void ProcessPacket(int p_id, unsigned char* p_buf)
{
	auto obj = objManager.GetObj(p_id);
	if (PacketManager::CheckPakcet(obj->m_state, (CS)p_buf[1]) == false) {
		obj->CloseSocket();
		cout << "이상 행동으로 disconect 하였습니다" << endl;
	}

	switch (p_buf[1]) {
	case CS::LOGIN: {
		cout << "CS Login - " << p_id << endl;
		// player 정보
		// 추후 데이터베이스 생성 후 수정 필요
		obj->Init(1, 100, 0, 0);
		if (PacketManager::SendLoginOk(p_id, *(obj->GetSocket())) == false)
			obj->CloseSocket();
		break;
	}
	case CS::MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p_buf);
		// 이상행동 체크
		if (obj->CheckAbnormalAction(packet)) {
			// session 종료
			PacketManager::SendEndSession(obj->m_id, *(obj->GetSocket()));
			sessionManager.RemovePlayer(obj->m_sessionId, obj->m_id);

			// 남은 플레이어들에게 해당 플레이어 제거
			auto players = sessionManager.GetSession(obj->m_sessionId)->GetPlayers();
			for (int i = 0; i < players.size(); ++i)
				PacketManager::SendRemoveObj(players[i]->m_id, obj->m_id, *(players[i]->GetSocket()));

			// 플레이어 게임정보 리셋
			obj->ResetInGameData();
			obj->m_state = OBJECT_STATE::OBJST_CONNECTED;
		}
		break;
	}
	case CS::CREATE_SESSION: {
		cs_packet_create_session* packet = reinterpret_cast<cs_packet_create_session*>(p_buf);

		// open된 session 없음, session 새로 open
		cout << "session type - " << packet->sessionType << endl;
		int openSessionId = sessionManager.GetSessionId(SESSION_STATE::SESSION_OPEN, packet->sessionType);
		if (openSessionId < 0) {
			// session open
			cout << "new session open" << endl;
			int closeSessionId = sessionManager.GetSessionId(SESSION_STATE::SESSION_CLOSE, packet->sessionType);
			sessionManager.OpenSession(packet->sessionType, closeSessionId, obj);
			// session id 플레이어에 할당
			obj->m_sessionId = closeSessionId;
			sessionManager.CheckSession(closeSessionId);
		}
		else {
			// open된 session에 setPlayer
			cout << "join session" << endl;
			sessionManager.JoinSession(openSessionId, obj);
			// session id 플레이어에 할당
			obj->m_sessionId = openSessionId;
			sessionManager.CheckSession(openSessionId);
		}

		break;
	}
	case CS::SESSION_END: {
		// session 정상 종료되는지 확인

		// session 종료
		sessionManager.EndSession(obj->m_sessionId);
		break;
	}
	case CS::RECONNECT: {
		cout << "CS::RECONNECT" << endl;
		cs_packet_reconnect* packet = reinterpret_cast<cs_packet_reconnect*>(p_buf);
		int sessionId = objManager.GetPlayerInSession(packet->id);
		if (sessionId != -1) {
			sessionManager.Reconnect(sessionId, packet->id, obj);
			objManager.GetObj(packet->id)->m_state = OBJECT_STATE::OBJST_FREE;
		}
		break;
	}
	case CS::COLLISION_OCCURRED: {
		cs_packet_collision_occurred* packet = reinterpret_cast<cs_packet_collision_occurred*>(p_buf);
		// 충돌 확인
		// 충돌 처리
		auto session = sessionManager.GetSession(packet->sessionId);
		int hp = session->SessionEnemyCollision(packet->id);
		obj->m_exp += 1;
		auto players = session->GetPlayers();
		for (int i = 0; i < players.size(); ++i) 
			PacketManager::SendCollisionResult(packet->id, hp, players[i]->m_exp, *(players[i]->GetSocket()));
		break;
	}

	default:
		cout << "Unknown Packet Type from Client[" << p_id;
		cout << "] Packet Type [" << (int)p_buf[1] << "]";
		while (true);
	}
}

void Worker(HANDLE h_iocp, SOCKET l_socket)
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
				ServerManager::DisplayError("GQCS : ", WSAGetLastError());
				cout << "???????" << endl;
				//exit(-1);
			}
			else {
				ServerManager::DisplayError("GQCS : ", WSAGetLastError());
				ServerManager::Disconnect(*(objManager.GetObj(key)->GetSocket()));
			}
		}
		if ((key != SERVER_ID) && (0 == num_bytes)) {
			ServerManager::Disconnect(*(objManager.GetObj(key)->GetSocket()));
			continue;
		}
		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

		switch (ex_over->m_op) {
		case OP_RECV: {
			Player* player = objManager.GetObj(key);
			unsigned char* packet_ptr = ex_over->m_packetbuf;
			int num_data = num_bytes + player->GetPrevSize();

			while (num_data >= packet_ptr[0]) {
				ProcessPacket(key, packet_ptr);
				num_data -= packet_ptr[0];
				packet_ptr += packet_ptr[0];
				if (0 >= num_data) break;
			}
			player->SetPrevSize(num_data);
			if (0 != num_data)
				memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
			ServerManager::Recv(*(player->GetSocket()), *(player->GetOverlapped()), player->GetPrevSize());
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_ACCEPT: {
			cout << "accept" << endl;
			int c_id = objManager.GetNewPlayerId(ex_over->m_csocket);
			cout << "new id :" << c_id << endl;
			Player* player = objManager.GetObj(c_id);

			if (-1 != c_id) {
				player->PlayerAccept(h_iocp, c_id);
				ServerManager::Recv(*(player->GetSocket()), *(player->GetOverlapped()), player->GetPrevSize());
			}
			else {
				player->CloseSocket();
			}

			memset(&ex_over->m_over, 0, sizeof(ex_over->m_over));
			SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ex_over->m_csocket = c_socket;
			AcceptEx(l_socket, c_socket, ex_over->m_packetbuf, 0, 32, 32, NULL, &ex_over->m_over);
			break;
		}
		case OP_POINT_MOVE: {
			int sessionId = *reinterpret_cast<int*>(ex_over->m_packetbuf);
			auto session = sessionManager.GetSession(sessionId);
			auto enemy = &session->m_enemyDatas[key];

			if (enemy->x.empty())
				return;

			int x = enemy->x.front();
			enemy->x.pop();
			int y = enemy->y.front();
			enemy->y.pop();

			auto players = session->GetPlayers();
			vector<int> disconnectIds;
			// send move packet
			for (int i = 0; i < players.size(); ++i) {
				if (PacketManager::SendMove(key + ObjectManager::GetEnemyId(enemy->type), x, y, *(players[i]->GetSocket()))
					== false) {
					disconnectIds.push_back(players[i]->m_id);
				}
			}
			// disconnect된 플레이어 remove
			if (disconnectIds.size() != 0)
				for (const auto& id : disconnectIds)
					session->RemovePlayer(id);

			if (players.size() != 0 && enemy->x.empty() == false)
				timer.AddEvent(key, sessionId, OP_POINT_MOVE, 1000); 
			break;
		}
		case OP_HEARTBEAT: {
			objManager.CheckHeartBeat();
			sessionManager.CheckZombieSession();
			timer.AddEvent(-1, -1, OP_HEARTBEAT, 10000);
			break;
		}
		}
	}
}

int main()
{
	vector <thread> worker_threads;
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(Worker, server.GetIocpHandle(), server.GetListenSocket());
	thread timer_thread{ std::thread([&]() { timer.DoTimer(); }) };

	timer_thread.join();
	cout << "join" << endl;
	for (auto& th : worker_threads)
		th.join();
}