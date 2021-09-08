#pragma once
#include"Player.h"
#include"Enemy.h"

enum SESSION_STATE { SESSION_OPEN, SESSION_CLOSE };
class Session
{
private:
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;
	int m_sessionID;
	int m_playersCnt;
	int m_enemysCnt;
	Player*m_players;
	Enemy *m_enemys;

public:
	~Session() {
		if (m_players != nullptr)
			delete[] m_players;

		if (m_enemys != nullptr)
			delete[] m_enemys;
	}

	void CreateSession(int id) { m_sessionID = id; }

	void SetSession(int playersCnt, int enemysCnt, vector<Player> players, vector<ENEMY_TYPE> enemyTypes){
		m_enemysCnt = enemysCnt;
		m_playersCnt = playersCnt;

		m_players = new Player[m_playersCnt];
		for (int i = 0; i < m_playersCnt; ++i)
			m_players[i] = players[i];

		m_enemys = new Enemy[m_enemysCnt];
		for (int i = 0; i < m_enemysCnt; ++i) {
			switch (enemyTypes[i])
			{
			case ENEMY_Plane1:
				m_enemys[i] = *new Plane1;
				break;
			case ENEMY_Plane2:
				m_enemys[i] = *new Plane2;
				break;
			case ENEMY_Plane3:
				m_enemys[i] = *new Plane3;
				break;
			case ENEMY_Boss1:
				m_enemys[i] = *new Boss1;
				break;
			case ENEMY_Boss2:
				m_enemys[i] = *new Boss2;
				break;
			default:
				break;
			}
		}
	}

	void CloseSession() {
		sessionState = SESSION_STATE::SESSION_CLOSE;
		m_sessionID = -1;
		m_playersCnt=0;
		m_enemysCnt=0;
		this->~Session();
	}
};

