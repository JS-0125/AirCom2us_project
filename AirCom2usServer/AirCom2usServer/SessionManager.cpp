#include "SessionManager.h"

 SessionManager::SessionManager() {
	for (int i = 0; i < MAX_SESSION + 1; ++i) {
		auto& session = sessions[i] = new Session;
		session->CreateSession(i);
	}
}

 SessionManager::~SessionManager() {
	for (int i = 0; i < MAX_SESSION + 1; ++i)
		delete sessions[i];
}

 Session* SessionManager::GetSession(int idx) {
	return sessions[idx];
}

 array<Session*, MAX_SESSION + 1>* SessionManager::GetSessions() {
	return &sessions;
}

 int SessionManager::get_session_id(SESSION_STATE sessionState, int sessionType)
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

 void SessionManager::checkSession(int sessionId) {
	if (!sessions[sessionId]->CheckSession())
		return;
	auto sessionPlayers = sessions[sessionId]->GetPlayers();

	for (int i = 0; i < sessions[sessionId]->m_enemyDatas.size(); ++i) {
		auto enemyData = sessions[sessionId]->m_enemyDatas[i];

		int typeId = ObjectManager::get_enemy_id(enemyData.type);
		int objId = typeId + i;

		for (int i = 0; i < sessionPlayers.size(); ++i)
			PacketManager::send_add_obj_packet(objId, *(sessionPlayers[i]->GetSocket())); //send_add_obj_packet(sessionPlayers[i], objId);

																						  // 등장 시간
		Timer::add_event(objId - typeId, sessionId, OP_POINT_MOVE, enemyData.time);
	}

	// 서로에게 플레이어 추가
	for (int i = 0; i < sessionPlayers.size(); ++i) {
		for (int j = 0; j < sessionPlayers.size(); ++j)
			if (sessionPlayers[i] != sessionPlayers[j])
				PacketManager::send_add_obj_packet(sessionPlayers[j]->m_id, *(sessionPlayers[i]->GetSocket()));
		//send_add_obj_packet(sessionPlayers[i], sessionPlayers[j]);
	}

	// 세션 세팅 ok
	for (int i = 0; i < sessionPlayers.size(); ++i)
		PacketManager::send_set_session_ok(sessionId, *(sessionPlayers[i]->GetSocket()));
}
