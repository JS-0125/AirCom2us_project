#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"Player.h"
#include"Enemy.h"

class ObjectManager
{
	array <Player*, MAX_USER + 1> m_players;
public:
	ObjectManager();
	~ObjectManager();

	array <Player*, MAX_USER + 1>* GetPlayers();
	Player* GetObj(int idx);
	int GetNewPlayerId(SOCKET p_socket);
	static int GetEnemyId(ENEMY_TYPE enemyType);
};

