#include "SessionManager.h"

 SessionManager::SessionManager() {
	for (int i = 0; i < MAX_SESSION + 1; ++i) {
		auto& session = m_sessions[i] = new Session;
		session->CreateSession(i);
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
		auto enemyData = m_sessions[sessionId]->m_enemyDatas[i];

		int typeId = ObjectManager::GetEnemyId(enemyData.type);
		int objId = typeId + i;

		for (int i = 0; i < sessionPlayers.size(); ++i)
			PacketManager::SendAddObj(objId, *(sessionPlayers[i]->GetSocket())); //send_add_obj_packet(sessionPlayers[i], objId);

																						  // 등장 시간
		Timer::AddEvent(objId - typeId, sessionId, OP_POINT_MOVE, enemyData.time);
	}

	// 서로에게 플레이어 추가
	for (int i = 0; i < sessionPlayers.size(); ++i) {
		for (int j = 0; j < sessionPlayers.size(); ++j)
			if (sessionPlayers[i] != sessionPlayers[j])
				PacketManager::SendAddObj(sessionPlayers[j]->m_id, *(sessionPlayers[i]->GetSocket()));
		//send_add_obj_packet(sessionPlayers[i], sessionPlayers[j]);
	}

	// 세션 세팅 ok
	for (int i = 0; i < sessionPlayers.size(); ++i)
		PacketManager::SendSetSessionOk(sessionId, *(sessionPlayers[i]->GetSocket()));
}
