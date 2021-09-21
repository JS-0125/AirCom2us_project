#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"Player.h"
#include"Enemy.h"

class ObjectManager
{
	array <Player*, MAX_USER + 1> players;
public:
	ObjectManager();
	~ObjectManager();

	array <Player*, MAX_USER + 1>* GetPlayers();
	Player* GetObj(int idx);
	int get_new_player_id(SOCKET p_socket);
	static int get_enemy_id(ENEMY_TYPE enemyType);
};

