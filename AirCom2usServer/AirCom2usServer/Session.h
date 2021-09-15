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
	int m_sessionID = -1; // 필요 없을 수도? 밖에서 배열로 관리하고 있기 때문에
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
			// "0" 키를 스택에 푸쉬
			lua_pushstring(L, (to_string(i)).c_str());

			// "0" 키에 해당하는 2차 테이블을 Lua -> stack으로 읽어오기.
			lua_gettable(L, 1);

			// "0"의 property 중 HP를 읽어오기 위해 "HP"키를 스택에 푸쉬
			lua_pushstring(L, "type");
			// "type"에 해당하는 500 값을 Lua -> stack으로 읽어오기.
			// "type" 키로 얻은 2차 테이블은 스택 인덱스 2에 있다.
			lua_gettable(L, 2);

			// 얻어온 값을 적당한 타입으로 캐스팅 후 사용하자.
			SessionData data;
			data.type = (ENEMY_TYPE)lua_tonumber(L, -1);

			// 다 사용하였으면 스택에서 값 제거
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

			//2차 배열 스택에서 제거.
			lua_pop(L, 1);
		}

		// table 제거
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
		// 가상머신 자료구조
		L = luaL_newstate();

		// 기본 라이브러리 로딩
		luaL_openlibs(L);

		// 가상 머신을 stack에 입력
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

