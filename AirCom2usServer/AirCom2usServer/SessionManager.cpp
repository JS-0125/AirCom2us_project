#include "SessionManager.h"

SessionManager::SessionManager() {
	int j = 0;
	for (int i = 0; i < MAX_SESSION + 1; ++i) {
		auto& session = m_sessions[i] = new Session;
		if (i % 1000 == 0) {
			++j;
		}
		string ip = "229.1." + to_string(j) + '.' + to_string(i % 1000);
		//cout << ip << "\t";
		session->CreateSession(ip);
	}
}

SessionManager::~SessionManager() {
	for (int i = 0; i < MAX_SESSION + 1; ++i)
		delete m_sessions[i];
}

Session* SessionManager::GetSession(int idx) {
	return m_sessions[idx];
}

array<Session*, MAX_SESSION + 1>* SessionManager::GetSessions() {
	return &m_sessions;
}
void SessionManager::JoinSession(int sessionId, Player* obj) {
	m_sessions[sessionId]->SetPlayer(obj);
}

void SessionManager::OpenSession(int playerCnt, int sessionId, Player* obj) {
	m_sessions[sessionId]->OpenSession(playerCnt);
	m_sessions[sessionId]->SetPlayer(obj);
}

int SessionManager::GetSessionId(SESSION_STATE sessionState, int sessionType)
{
	switch (sessionState)
	{
	case SESSION_OPEN:
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (m_sessions[i]->sessionState == SESSION_STATE::SESSION_OPEN && m_sessions[i]->m_sessionType == sessionType)
				return i;
		break;
	}
	case SESSION_CLOSE:
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (m_sessions[i]->sessionState == SESSION_STATE::SESSION_CLOSE)
				return i;
		break;
	}
	case SESSION_INGAME:
	{
		for (int i = 0; i < MAX_SESSION; ++i)
			if (m_sessions[i]->sessionState == SESSION_STATE::SESSION_INGAME)
				return i;
		break;
	}
	default:
		return -1;
		break;
	}
	return -1;
}

void SessionManager::CheckSession(int sessionId) {
	if (!m_sessions[sessionId]->CheckSession())
		return;
	auto sessionPlayers = m_sessions[sessionId]->GetPlayers();

	for (int i = 0; i < m_sessions[sessionId]->m_enemyDatas.size(); ++i) {
		auto& enemyData = m_sessions[sessionId]->m_enemyDatas[i];

		int typeId = ObjectManager::GetEnemyId(enemyData.type);
		int objId = typeId + i;
		enemyData.id = objId;
		cout << "enemy hp - " << enemyData.hp << endl;
		// enemy ???? ????
		for (int i = 0; i < sessionPlayers.size(); ++i)
			PacketManager::SendAddObj(objId, enemyData.hp, *(sessionPlayers[i]->GetSocket())); //send_add_obj_packet(sessionPlayers[i], objId);

		// ???? ?ð?
		Timer::AddEvent(objId - typeId, sessionId, OP_POINT_MOVE, enemyData.time);
	}

	// ???ο??? ?÷??̾? ?߰?
	for (int i = 0; i < sessionPlayers.size(); ++i) {
		for (int j = 0; j < sessionPlayers.size(); ++j)
			if (sessionPlayers[i] != sessionPlayers[j])
				PacketManager::SendAddObj(sessionPlayers[j]->m_id, 10, *(sessionPlayers[i]->GetSocket()));
	}

	// ???? ???? ok
	for (int i = 0; i < sessionPlayers.size(); ++i) {
		PacketManager::SendSetSessionOk(m_sessions[sessionId]->m_sessionIP, sessionId, *(sessionPlayers[i]->GetSocket()));
		sessionPlayers[i]->m_state = OBJECT_STATE::OBJST_INGAME;
	}
}

bool SessionManager::IsSessionEnd(int sessionId) {
	if (m_sessions[sessionId]->IsSessionEnd())
		return true;
	return false;
}
void SessionManager::Reconnect(int sessionId, int prevId, Player* newObj) {
	m_sessions[sessionId]->Reconnect(prevId, newObj);
	newObj->m_sessionId = sessionId;
	newObj->m_state = OBJECT_STATE::OBJST_INGAME;

	PacketManager::SendReconnectOk(newObj->m_id, sessionId, *(newObj->GetSocket()));
}

// ?ϰ??????? session ????
void SessionManager::EndSession(int sessionId) {
	auto players = m_sessions[sessionId]->GetPlayers();
	for (const auto& player : players)
		PacketManager::SendEndSession(player->m_id, *(player->GetSocket()));
	m_sessions[sessionId]->CloseSession();
}
