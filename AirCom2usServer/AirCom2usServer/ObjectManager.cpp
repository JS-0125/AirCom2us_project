#include "ObjectManager.h"

 ObjectManager::ObjectManager() {
	for (int i = 0; i < MAX_USER + 1; ++i) {
		auto& obj = players[i] = new Player;
		obj->m_id = i;
	}
}

 ObjectManager::~ObjectManager() {
	for (int i = 0; i < MAX_USER + 1; ++i)
		delete players[i];
}

 array<Player*, MAX_USER + 1>* ObjectManager::GetPlayers() {
	return &players;
}

 Player* ObjectManager::GetObj(int idx) {
	return players[idx];
}

 int ObjectManager::get_new_player_id(SOCKET p_socket)
{
	for (int i = SERVER_ID + 1; i < MAX_PLAYER_IDX + 1; ++i) {
		if (players[i]->isState(OBJST_FREE)) {
			players[i]->Connected(p_socket);
			return i;
		}
	}
	return -1;
}

 int ObjectManager::get_enemy_id(ENEMY_TYPE enemyType) {
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
