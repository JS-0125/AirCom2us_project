#include "Session.h"

 void Session::GetSessionTable(const char* tableName) {
	lua_getglobal(m_luaState, tableName);

	lua_getglobal(m_luaState, "tablelength");
	lua_pushvalue(m_luaState, -2);
	lua_pcall(m_luaState, 1, 1, 0);
	m_enemysCnt = lua_tonumber(m_luaState, -1);
	lua_pop(m_luaState, 1);

	cout << m_enemysCnt << endl;

	for (int i = 0; i < m_enemysCnt; ++i) {
		SessionData data;

		// "0" 키를 스택에 푸쉬
		lua_pushstring(m_luaState, (to_string(i)).c_str());

		// "0" 키에 해당하는 2차 테이블을 Lua -> stack으로 읽어오기.
		lua_gettable(m_luaState, 1);

		// "0"의 property 중 type를 읽어오기 위해 "type"키를 스택에 푸쉬
		lua_pushstring(m_luaState, "type");
		// "type"에 해당하는 값을 Lua -> stack으로 읽어오기.
		// "type" 키로 얻은 2차 테이블은 스택 인덱스 2에 있다.
		lua_gettable(m_luaState, 2);

		data.type = (ENEMY_TYPE)lua_tonumber(m_luaState, -1);

		// 다 사용하였으면 스택에서 값 제거
		lua_pop(m_luaState, 1);

		// time
		lua_pushstring(m_luaState, "time");
		lua_gettable(m_luaState, 2);
		data.time = lua_tonumber(m_luaState, -1);
		lua_pop(m_luaState, 1);

		//positions
		// x
		lua_pushstring(m_luaState, "x");
		lua_gettable(m_luaState, 2);

		size_t len = lua_rawlen(m_luaState, -1);
		cout << "x len - " << len << endl;
		for (int i = 1; i < len + 1; ++i) {
			lua_rawgeti(m_luaState, -1, i);
			int x = lua_tonumber(m_luaState, -1);
			data.x.push(x);
			cout << "x - " << x << endl;
			lua_pop(m_luaState, 1);
		}
		lua_pop(m_luaState, 1);

		//y
		lua_pushstring(m_luaState, "y");
		lua_gettable(m_luaState, 2);

		len = lua_rawlen(m_luaState, -1);
		cout << "y len - " << len << endl;
		for (int i = 1; i < len + 1; ++i) {
			lua_rawgeti(m_luaState, -1, i);
			int y = lua_tonumber(m_luaState, -1);
			data.y.push(y);
			cout << "y - " << y << endl;
			lua_pop(m_luaState, 1);
		}
		lua_pop(m_luaState, 1);

		m_enemyDatas.push_back(data);

		//2차 배열 스택에서 제거.
		lua_pop(m_luaState, 1);
	}

	// table 제거
	lua_pop(m_luaState, 1);
}

 void Session::GetSessionDatas(int session) {
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

 Session::~Session() {
	if (m_players != nullptr)
		delete[] m_players;
}

 void Session::CreateSession(int id) {
	m_sessionID = id;
	// 가상머신 자료구조
	m_luaState = luaL_newstate();

	// 기본 라이브러리 로딩
	luaL_openlibs(m_luaState);

	// 가상 머신을 stack에 입력
	int error = luaL_loadfile(m_luaState, "session.lua") ||
		lua_pcall(m_luaState, 0, 0, 0);

	if (error != 0) {
		cout << "error" << endl;
	}
	lua_getglobal(m_luaState, "set_uid");
	lua_pushnumber(m_luaState, m_sessionID);
	lua_pcall(m_luaState, 1, 1, 0);
	lua_pop(m_luaState, 1);// eliminate set_uid from stack after call
}

 void Session::OpenSession(int playersCnt) {
	sessionState = SESSION_STATE::SESSION_OPEN;

	m_playersCnt = playersCnt;
	m_players = new Player[m_playersCnt];
	m_sessionType = playersCnt;

	cout << "m_playersCnt - " << m_playersCnt << endl;
}

 void Session::SetPlayer(Player& player) {
	m_players[m_setPlayerCnt++] = player;
	if (m_setPlayerCnt == m_playersCnt)
		SetSession();
}

 void Session::SetSession() {
	// lua에서 가져올 데이터 결정
	GetSessionDatas(0);
	sessionState = SESSION_STATE::SESSION_INGAME;
}

 void Session::SetSession(int enemysCnt, vector<SessionData> enemyDatas) {
	m_enemysCnt = enemysCnt;
	m_enemyDatas = enemyDatas;
}

 bool Session::CheckSession() {
	if (m_setPlayerCnt == m_playersCnt)
		return true;
	return false;
}

 vector<int> Session::GetPlayerId() {
	vector<int> tmp;
	for (int i = 0; i < m_playersCnt; ++i)
		tmp.push_back(m_players[i].m_id);

	return tmp;
}

 vector<Player*>& Session::GetPlayers() {
	vector<Player*> tmp;
	for (int i = 0; i < m_playersCnt; ++i)
		tmp.emplace_back(&m_players[i]);

	return tmp;
}

 void Session::CloseSession() {
	sessionState = SESSION_STATE::SESSION_CLOSE;
	m_sessionID = -1;
	m_playersCnt = 0;
	m_enemysCnt = 0;
	this->~Session();
}
