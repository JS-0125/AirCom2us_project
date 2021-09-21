#pragma once
#include"default.h"
#include"Player.h"
#include"Enemy.h"

enum SESSION_STATE { SESSION_OPEN, SESSION_CLOSE, SESSION_INGAME };

struct SessionData {
	ENEMY_TYPE type;
	int time;
	queue<int> x, y;
};

class Session
{
private:
	int m_sessionID = -1; // 필요 없을 수도? 밖에서 배열로 관리하고 있기 때문에
	int m_playersCnt = 0;
	int m_enemysCnt = 0;
	Player *m_players;
	int m_setPlayerCnt = 0;
	lua_State* L;
	mutex m_sl;

	void GetSessionTable(const char* tableName);
	void GetSessionDatas(int session);
public:
	vector<SessionData> m_enemyDatas;
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;
	int m_sessionType = -1;

	~Session();

	void CreateSession(int id);
	void OpenSession(int playersCnt);
	void SetPlayer(Player& player);
	void SetSession();
	void SetSession(int enemysCnt, vector<SessionData> enemyDatas);
	bool CheckSession();
	vector<int> getPlayerId();
	vector<Player*>& GetPlayers();
	void CloseSession();
};

