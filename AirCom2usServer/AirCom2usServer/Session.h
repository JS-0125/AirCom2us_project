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

	void GetSessionTable(const char* tableName) {
		lua_getglobal(L, tableName);

		lua_getglobal(L, "tablelength");
		lua_pushvalue(L, -2);
		lua_pcall(L, 1, 1, 0);
		m_enemysCnt = lua_tonumber(L, -1);
		lua_pop(L, 1);

		cout << m_enemysCnt << endl;

		for (int i = 0; i < m_enemysCnt; ++i) {
			SessionData data;

			// "0" 키를 스택에 푸쉬
			lua_pushstring(L, (to_string(i)).c_str());

			// "0" 키에 해당하는 2차 테이블을 Lua -> stack으로 읽어오기.
			lua_gettable(L, 1);

			// "0"의 property 중 type를 읽어오기 위해 "type"키를 스택에 푸쉬
			lua_pushstring(L, "type");
			// "type"에 해당하는 값을 Lua -> stack으로 읽어오기.
			// "type" 키로 얻은 2차 테이블은 스택 인덱스 2에 있다.
			lua_gettable(L, 2);

			data.type = (ENEMY_TYPE)lua_tonumber(L, -1);

			// 다 사용하였으면 스택에서 값 제거
			lua_pop(L, 1);

			// time
			lua_pushstring(L, "time");
			lua_gettable(L, 2);
			data.time = lua_tonumber(L, -1);
			lua_pop(L, 1);

			//positions
			// x
			lua_pushstring(L, "x");
			lua_gettable(L, 2);
			
			size_t len = lua_rawlen(L, -1);
			cout << "x len - " << len << endl;
			for (int i = 1; i < len+1; ++i) {
				lua_rawgeti(L, -1, i); 
				int x = lua_tonumber(L, -1);
				data.x.push(x);
				cout << "x - " << x << endl;
				lua_pop(L, 1);
			}
			lua_pop(L, 1);

			//y
			lua_pushstring(L, "y");
			lua_gettable(L, 2);

			len = lua_rawlen(L, -1);
			cout << "y len - " << len << endl;
			for (int i = 1; i < len + 1; ++i) {
				lua_rawgeti(L, -1, i);
				int y = lua_tonumber(L, -1);
				data.y.push(y);
				cout << "y - " << y << endl;
				lua_pop(L, 1);
			}
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
			break;
		case 1:
			GetSessionTable("enemys1");
			break;
		case 2:
			GetSessionTable("enemys2");
			break;
		default:
			cout << "session is not exist" << endl;
			break;
		}
	}
public:
	vector<SessionData> m_enemyDatas;
	SESSION_STATE sessionState = SESSION_STATE::SESSION_CLOSE;
	int m_sessionType = -1;

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
		m_sessionType = playersCnt;

		cout << "m_playersCnt - " << m_playersCnt << endl;
	}

	void SetPlayer(Player& player) {
		m_players[m_setPlayerCnt++] = player;
		if (m_setPlayerCnt == m_playersCnt)
			SetSession();
	}

	void SetSession() {
		// lua에서 가져올 데이터 결정
		GetSessionDatas(0);
		sessionState = SESSION_STATE::SESSION_INGAME;
	}

	void SetSession(int enemysCnt, vector<SessionData> enemyDatas){
		m_enemysCnt = enemysCnt;
		m_enemyDatas = enemyDatas;
	}

	void StartSession(Object *enemy) {

	}

	bool CheckSession() {
		if (m_setPlayerCnt == m_playersCnt)
			return true;
		return false;
	}

	vector<int> getPlayerId() {
		vector<int> tmp;
		for (int i = 0; i < m_playersCnt; ++i)
			tmp.push_back(m_players[i].m_id);

		return tmp;
	}

	void CloseSession() {
		sessionState = SESSION_STATE::SESSION_CLOSE;
		m_sessionID = -1;
		m_playersCnt=0;
		m_enemysCnt=0;
		this->~Session();
	}
};

