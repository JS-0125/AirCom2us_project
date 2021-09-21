#include "Session.h"

 void Session::GetSessionTable(const char* tableName) {
	lua_getglobal(L, tableName);

	lua_getglobal(L, "tablelength");
	lua_pushvalue(L, -2);
	lua_pcall(L, 1, 1, 0);
	m_enemysCnt = lua_tonumber(L, -1);
	lua_pop(L, 1);

	cout << m_enemysCnt << endl;

	for (int i = 0; i < m_enemysCnt; ++i) {
		SessionData data;

		// "0" Ű�� ���ÿ� Ǫ��
		lua_pushstring(L, (to_string(i)).c_str());

		// "0" Ű�� �ش��ϴ� 2�� ���̺��� Lua -> stack���� �о����.
		lua_gettable(L, 1);

		// "0"�� property �� type�� �о���� ���� "type"Ű�� ���ÿ� Ǫ��
		lua_pushstring(L, "type");
		// "type"�� �ش��ϴ� ���� Lua -> stack���� �о����.
		// "type" Ű�� ���� 2�� ���̺��� ���� �ε��� 2�� �ִ�.
		lua_gettable(L, 2);

		data.type = (ENEMY_TYPE)lua_tonumber(L, -1);

		// �� ����Ͽ����� ���ÿ��� �� ����
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
		for (int i = 1; i < len + 1; ++i) {
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

		//2�� �迭 ���ÿ��� ����.
		lua_pop(L, 1);
	}

	// table ����
	lua_pop(L, 1);
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
	// lua���� ������ ������ ����
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

 vector<int> Session::getPlayerId() {
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
