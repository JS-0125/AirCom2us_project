#pragma once
#include"Player.h"
#include"Enemy.h"

enum SESSION_STATE { SESSION_OPEN, SESSION_CLOSE, SESSION_INGAME };
class Session
{
private:
	int m_sessionID = -1; // 필요 없을 수도? 밖에서 배열로 관리하고 있기 때문에
	int m_playersCnt = 0;
	int m_enemysCnt = 0;
	Player*m_players;
	vector<int> m_enemyIds;
	int m_setPlayerCnt = 0;

public:
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;

	~Session() {
		if (m_players != nullptr)
			delete[] m_players;
	}

	void CreateSession(int id) { m_sessionID = id; }

	void OpenSession(int playersCnt) {
		sessionState = SESSION_STATE::SESSION_OPEN;

		m_playersCnt = playersCnt;
		m_players = new Player[m_playersCnt];
	}

	void SetPlayer(Player& player) {
		m_players[m_setPlayerCnt++] = player;
	}

	void SetSession(int enemysCnt, vector<int> enemyIds){
		m_enemysCnt = enemysCnt;
		m_enemyIds = enemyIds;
	}

	void CloseSession() {
		sessionState = SESSION_STATE::SESSION_CLOSE;
		m_sessionID = -1;
		m_playersCnt=0;
		m_enemysCnt=0;
		this->~Session();
	}
};

