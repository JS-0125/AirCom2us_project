#include "ObjectManager.h"

 ObjectManager::ObjectManager() {
	for (int i = 0; i < MAX_USER + 1; ++i) {
		auto& obj = m_players[i] = new Player;
		obj->m_id = i;
	}
}

 ObjectManager::~ObjectManager() {
	for (int i = 0; i < MAX_USER + 1; ++i)
		delete m_players[i];
}

 array<Player*, MAX_USER + 1>* ObjectManager::GetPlayers() {
	return &m_players;
}

 Player* ObjectManager::GetObj(int idx) {
	return m_players[idx];
}

 int ObjectManager::GetPlayerInSession(int id) {
	 if (m_players[id]->m_state == OBJECT_STATE::OBJST_INGAME) {
		 return m_players[id]->m_sessionId;
	 }
	 return -1;
 }

 int ObjectManager::GetNewPlayerId(SOCKET p_socket)
{
	for (int i = SERVER_ID + 1; i < MAX_PLAYER_IDX + 1; ++i) {
		if (m_players[i]->IsState(OBJST_FREE)) {
			m_players[i]->Connected(p_socket);
			return i;
		}
	}
	return -1;
}

 int ObjectManager::GetEnemyId(ENEMY_TYPE enemyType) {
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
