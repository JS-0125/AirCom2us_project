#pragma once
#include"default.h"
#include"Player.h"
#include"Enemy.h"

enum SESSION_STATE { SESSION_OPEN, SESSION_CLOSE, SESSION_INGAME };

struct SessionData {
	ENEMY_TYPE type;
	int id;
	int time;
	int hp;
	queue<int> x, y;
};

class Session
{
private:
	int m_playersCnt = 0;
	int m_enemysCnt = 0;
	vector<Player*> m_players;
	lua_State* m_luaState;
	mutex m_sl;

	void GetSessionTable(const char* tableName);
	void GetSessionDatas(int session);
public:
	string m_sessionIP = ""; // udp ip
	vector<SessionData> m_enemyDatas;
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;
	int m_sessionType = -1;
	void CreateSession(string ip);
	void OpenSession(int playersCnt);
	void SetPlayer(Player* player);
	void SetSession();
	void SetSession(int enemysCnt, vector<SessionData> enemyDatas);
	void Reconnect(int prevId, Player* player);
	bool CheckSession();
	int SessionEnemyCollision(int id);
	vector<int> GetPlayerId();
	vector<Player*>& GetPlayers();
	void CloseSession();
	bool IsSessionEnd();
	void RemovePlayer(int playerId);
};

