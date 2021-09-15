#pragma once
#include"default.h"
#include"Player.h"
#include"Enemy.h"

enum SESSION_STATE { SESSION_OPEN, SESSION_CLOSE, SESSION_INGAME };

struct SessionData {
	ENEMY_TYPE type;
	int time;
	int x, y;
};

class Session
{
private:
	int m_sessionID = -1; // �ʿ� ���� ����? �ۿ��� �迭�� �����ϰ� �ֱ� ������
	int m_playersCnt = 0;
	int m_enemysCnt = 0;
	Player*m_players;
	int m_setPlayerCnt = 0;
	lua_State* L;
	mutex m_sl;

	void GetSessionTable(const char* tableName) {
		lua_getglobal(L, tableName);

		lua_getglobal(L, "tablelength");
		lua_pushvalue(L, -2);
		lua_pcall(L, 1, 1, 0);
		m_enemysCnt = lua_tonumber(L, -1);
		lua_pop(L, 1);

		cout << m_enemysCnt << endl;

		for (int i = 0; i < m_enemysCnt; ++i) {
			// "0" Ű�� ���ÿ� Ǫ��
			lua_pushstring(L, (to_string(i)).c_str());

			// "0" Ű�� �ش��ϴ� 2�� ���̺��� Lua -> stack���� �о����.
			lua_gettable(L, 1);

			// "0"�� property �� HP�� �о���� ���� "HP"Ű�� ���ÿ� Ǫ��
			lua_pushstring(L, "type");
			// "type"�� �ش��ϴ� 500 ���� Lua -> stack���� �о����.
			// "type" Ű�� ���� 2�� ���̺��� ���� �ε��� 2�� �ִ�.
			lua_gettable(L, 2);

			// ���� ���� ������ Ÿ������ ĳ���� �� �������.
			SessionData data;
			data.type = (ENEMY_TYPE)lua_tonumber(L, -1);

			// �� ����Ͽ����� ���ÿ��� �� ����
			lua_pop(L, 1);

			// time
			lua_pushstring(L, "time");
			lua_gettable(L, 2);
			data.time = lua_tonumber(L, -1);
			lua_pop(L, 1);

			// x
			lua_pushstring(L, "x");
			lua_gettable(L, 2);
			data.x = lua_tonumber(L, -1);
			lua_pop(L, 1);

			//y
			lua_pushstring(L, "y");
			lua_gettable(L, 2);
			data.y = lua_tonumber(L, -1);
			lua_pop(L, 1);

			m_enemyDatas.push_back(data);

			//2�� �迭 ���ÿ��� ����.
			lua_pop(L, 1);
		}

		// table ����
		lua_pop(L, 1);
	}
	void GetSessionDatas(int session) {
		switch (session)
		{
		case 0:
			GetSessionTable("enemys0");
			for (int i = 0; i < m_enemyDatas.size(); ++i) 
				cout << "time: " << m_enemyDatas[i].time << " type: " << m_enemyDatas[i].type 
				<< " " << m_enemyDatas[i].x << ", " << m_enemyDatas[i].y << endl;
			break;
		default:
			cout << "session is not exist" << endl;
			break;
		}
	}
public:
	vector<SessionData> m_enemyDatas;
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;

	~Session() {
		if (m_players != nullptr)
			delete[] m_players;
	}

	void CreateSession(int id) { 
		m_sessionID = id; 
		// ����ӽ� �ڷᱸ��
		L = luaL_newstate();

		// �⺻ ���̺귯�� �ε�
		luaL_openlibs(L);

		// ���� �ӽ��� stack�� �Է�
		int error = luaL_loadfile(L, "session.lua") ||
			lua_pcall(L, 0, 0, 0);

		if (error != 0) {
			cout << "error" << endl;
		}
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, m_sessionID);
		lua_pcall(L, 1, 1, 0);
		lua_pop(L, 1);// eliminate set_uid from stack after call
	}

	void OpenSession(int playersCnt) {
		sessionState = SESSION_STATE::SESSION_OPEN;

		m_playersCnt = playersCnt;
		m_players = new Player[m_playersCnt];
	}

	void SetPlayer(Player& player) {
		m_players[m_setPlayerCnt++] = player;
	}

	void SetSession(int session) {
		GetSessionDatas(session);
	}

	void SetSession(int enemysCnt, vector<SessionData> enemyDatas){
		m_enemysCnt = enemysCnt;
		m_enemyDatas = enemyDatas;
	}


	void CloseSession() {
		sessionState = SESSION_STATE::SESSION_CLOSE;
		m_sessionID = -1;
		m_playersCnt=0;
		m_enemysCnt=0;
		this->~Session();
	}
};

