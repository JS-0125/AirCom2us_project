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
	int GetSessionId(SESSION_STATE sessionState, int sessionType);
	void CheckSession(int sessionId);
};

