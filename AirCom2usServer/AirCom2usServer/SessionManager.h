#pragma once
#include"default.h"
#include"AirCom2us_protocol.h"
#include"Session.h"
#include"ObjectManager.h"
#include"PacketManager.h"
#include"Timer.h"

class SessionManager
{
	array <Session*, MAX_SESSION + 1> m_sessions;
public:
	SessionManager();
	~SessionManager();

	Session* GetSession(int idx);
	array <Session*, MAX_SESSION + 1>* GetSessions();
	void JoinSession(int sessionId, Player* obj);
	void OpenSession(int playerCnt, int sessionId, Player* obj);
	int GetSessionId(SESSION_STATE sessionState, int sessionType);
	void CheckSession(int sessionId);
	bool IsSessionEnd(int sessionId);
	void Reconnect(int sessionId, int prevId, Player* newObj);
	void CheckZombieSession() {
		for (int i = 0; i < m_sessions.size(); ++i) {
			if (m_sessions[i]->GetPlayers().size() == 0)
				m_sessions[i]->CloseSession();
		}
	}
	// 일괄적으로 session 종료
	void EndSession(int sessionId);
	void RemovePlayer(int sessionId, int playerId) {
		m_sessions[sessionId]->RemovePlayer(playerId);
	}
};

