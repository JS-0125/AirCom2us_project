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
	void CheckHeartBeat();
	static int GetEnemyId(ENEMY_TYPE enemyType);
};

