#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"Player.h"
#include"Enemy.h"
#include"PacketManager.h"

class ObjectManager
{
	array <Player*, MAX_USER + 1> m_players;
public:
	ObjectManager();
	~ObjectManager();

	array <Player*, MAX_USER + 1>* GetPlayers();
	Player* GetObj(int idx);
	int GetPlayerInSession(int id);
	int GetNewPlayerId(SOCKET p_socket);
	void CheckHeartBeat() {
		for (int i = 0; i < m_players.size(); ++i) {
			if (m_players[i]->m_state != OBJECT_STATE::OBJST_FREE) {
				if (PacketManager::SendHeartBeat(*(m_players[i]->GetSocket())) == false) {
					cout << "no HeartBeat - " << i << endl;
					m_players[i]->m_state = OBJECT_STATE::OBJST_FREE;
				}
			}
		}
	}
	static int GetEnemyId(ENEMY_TYPE enemyType);
};

